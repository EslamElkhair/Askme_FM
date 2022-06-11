//AskMe - Big Picture
/*
 ● People ask questions and others answers them

 ● This is files-based system
 ○ Every time the user run it, you see the previous state (users/questions/answers)

 ○ Multiple users can use it in same time (parallel)
 ■ You send question. I see it
 ■ I answer the question or remove it, you see this update

 ● Login/Sign up/Sign Out
 ○ Any system allow a user to sign up (name, email, password, username)
 ○ Also: As a user do you allow anonymous questions?

 ● User operations
 ○ Print questions: From you, To you, Feed Questions
 ○ Ask and answer questions. System support threads
 ○ Delete a question (which might be a thread)
 */

//______________________________________________________
//AskMe: Design
/*
 ● Every user has a system generated ID
 ○ Same for every question
 ○ We can view these IDs on the site (not realistic)

 ● A thread question
 ○ If a question is answered. Others can ask questions in the original question thread
 ○ E.g. original question is parent for all of them
 ○ If this parent question is removed, the whole thread is removed

 ● Correctness
 ○ You can’t do something wrong!
 ○ E.g. you can’t remove a question asked to someone else!
 ○ You can only see/edit the details of your question
 */

#include"helperMethods.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<assert.h>
#include<vector>
#include<string>
#include<map>
using namespace std;

//Reperesenting a single user
struct user {
	int id;
	string name, password, confirmed_password, display_name, email;
	char allow_anonymous_questions;   // here

	user() {
		id = -1;
		allow_anonymous_questions = 'n';
	}

	user(string line) {
		vector<string> user_input_data = SplitString(line);
		assert(user_input_data.size() == 6);

		id = inFile_str_to_int(user_input_data[0]);
		name = user_input_data[1];
		confirmed_password = user_input_data[2];
		display_name = user_input_data[3];
		email = user_input_data[4];
		allow_anonymous_questions = inFile_str_to_char(user_input_data[5]);
	}

	string inFile_userdata_to_str_format() {
		ostringstream oss;
		oss << id << "," << name << "," << confirmed_password << ","
				<< display_name << "," << email << ","
				<< allow_anonymous_questions;

		return oss.str();
	}

};

//Handling a lot of different users
struct UsersManager {
	user current_user;   // only single user
	map<string, user> users_names_map;  // map for all users_names
	bool trails_expired;
	int last_id;

	UsersManager() {
		trails_expired = false;
		last_id = 0;
	}

	void load_Users_Database() {

		//clear everything
		last_id = 0;
		users_names_map.clear();

		//get the whole content of the file
		vector<string> lines = read_file_lines("users.txt");

		//itterate on the file line by line
		for (auto &line : lines) {

			user user(line);  // read current userdata in the current line
			users_names_map[user.name] = user;
			last_id = max(last_id, user.id); // update the current max id
		}
	}

	//Log in - Sign up Features
	bool access_system() {
		int choice = show_menu_choices( { "Sign Up", "Log In", "Exit" });

		if (choice == 1) {
			signup();
			return true;
		}

		else if (choice == 2) {
			login();
			return true;
		}

		return false;
	}

	void login() {

		load_Users_Database();

		for (int trails = 3; trails > 0; trails--) {

			cout << "\nEnter user name: ";
			cin >> current_user.name;

			cout << "\nEnter password: ";
			cin >> current_user.confirmed_password;

			if (!users_names_map.count(current_user.name)) {

				if (trails == 1) {
					cout
							<< "\n\tYou have tried several times. Try again later\n";
					trails_expired = true;
					break;
				}
				cout << "\n\tInvalid username or password. Try again\n";
				continue;
			}

			//Check username and password correctness
			user exist_user = users_names_map[current_user.name];


			if( current_user.confirmed_password!= exist_user.confirmed_password) {
				cout << "\n\tInvalid username or password. Try again\n";
				continue;
			}
			//if assured -> Welcome screen
			current_user = exist_user;
//			users_names_map[current_user.name] = current_user;
			cout << "\n\t\t\t_________________________________\n";
			cout << "\n\t\t\t     Welcome back, " << current_user.display_name
					<< "\n";
			cout << "\t\t\t_________________________________\n";
			break;
		}
	}

	void signup() {

		for (int trails = 3; trails > 0; --trails) {
			cout << "\nEnter user name (No spaces) : ";
			cin >> current_user.name;

			if (users_names_map.count(current_user.name)) {

				if (trails == 1) {
					cout << "\nYou have tried several times. Try again later\n";
					trails_expired = true;
					return;
				}
				cout << "\nEror: Already exist. Try again\n";
			} else
				break;

		}
		for (int trails = 3; trails > 0 && trails_expired == false; --trails) {
			cout << "\nEnter password: ";
			cin >> current_user.password;
			cout << "\nEnter password again: ";
			cin >> current_user.confirmed_password;

			if (!compare_confirmed_password(current_user.password,
					current_user.confirmed_password)) {

				if (trails == 1) {
					cout << "\nyou have tried several times. try again later\n";
					trails_expired = true;
					break;
				}
				cout << "\nError: Not the same password. Try again\n";
				continue;
			}
			break;
		}

		if (trails_expired == false) {
			cout << "\nEnter Display Name : ";
			cin.ignore();
			getline(cin, current_user.display_name);

			cout << "\nEnter E-mail : ";
			cin >> current_user.email;
		}

		for (int trails = 3; trails > 0 && trails_expired == false; --trails) {
			cout << "\nAllow anonymous Questions(1 - 0) ? ";
			int status = current_user.allow_anonymous_questions;
			cin >> status;

			if (!(//status == 'y' || status == 'Y' || status == 'n'
					status == 0 || status == 1)) {

				if (trails == 1) {
					cout << "\nYou have tried several times. Try again later\n";
					trails_expired = true;
					break;
				}

				cout << "\nError: Invalid input. Try again\n";
				continue;
			} else
				break;
		}

		if (trails_expired == false) {
			current_user.id = ++last_id;
			// What happens in 2 parallel sessions if they signed up? -> They are given same id. This is wrong handling
			users_names_map[current_user.name] = current_user;
			// if assured -> Welcome screen
			cout << "\n\t\t\t_________________________________\n";
			cout << "\n\t\t\t\t Welcome, " << current_user.display_name << "\n";
			cout << "\t\t\t_________________________________\n";

			update_Users_Database(current_user);
		}
	}
	void update_Users_Database(user &user) {
		string line = user.inFile_userdata_to_str_format();
		vector<string> lines(1, line);
		write_file_lines("users.txt", lines);
	}

	void list_users() {
	}
};

//repreesenting a single question
struct Question {

	int question_id;
	string question_text;
	string answer_text;

	Question() {
		question_id=-1;
	}

};

//Handling a lot of different questions
struct QuestionsManager {

	void print_user_questions() {
	}
	void print_others_questions() {
	}
	void ask_question() {
	}
	void answer_question() {
	}
	void delete_question() {
	}
	void list_feed() {
	}
};

struct askMeSystem {
	QuestionsManager questions_manager;
	UsersManager users_manager;

	void LoadDatabase() {
		users_manager.load_Users_Database();
	}

	// Here is what the user is interfacing with
	void run() {
		LoadDatabase();
		if (!users_manager.access_system() || users_manager.trails_expired)
			return;

		vector<string> menu;
		menu.push_back("My Questions");
		menu.push_back("Others Questions");
		menu.push_back("Ask a Question");
		menu.push_back("Answer a Question");
		menu.push_back("Delete a Question");
		menu.push_back("List Users");
		menu.push_back("Feed");
		menu.push_back("Logout");

		while (true && !cin.fail()) {

			int choice = show_menu_choices(menu);
			LoadDatabase();

			if (choice == 1) {
				questions_manager.print_user_questions();
			} else if (choice == 2) {
				questions_manager.print_others_questions();
			} else if (choice == 3) {
				questions_manager.ask_question();
			} else if (choice == 4) {
				questions_manager.answer_question();
			} else if (choice == 5) {
				questions_manager.delete_question();
			} else if (choice == 6) {
				users_manager.list_users();
			} else if (choice == 7) {
				questions_manager.list_feed();
			} else
				break;
		}
		run();
	}
};

int main() {

	askMeSystem service;
	service.run();
	return 0;
}

