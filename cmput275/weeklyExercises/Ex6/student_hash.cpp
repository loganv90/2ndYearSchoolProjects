#include <iostream>
#include <unordered_set>
using namespace std;


// student record struct with overwritten == operator
struct StudentRecord {
	string name;
	int id, grade;

	bool operator==(const StudentRecord & other) const {
		if (id == other.id) {
			return true;
		}
		else {
			return false;
		}
	}
};


// hash the struct by id
namespace std {
	template<> struct hash<StudentRecord> {
		size_t operator()(const StudentRecord & other) const {
			return hash<int>()(other.id);
		}
	};
}


int main() {
	// struct variables
	unordered_set<StudentRecord> students;
	StudentRecord currentStudent;

	// struct property variables
	string Name;
	int ID;
	int Grade;

	// variables for while loop functionality
	string command;
	string command2;
	bool found;


	while (true) {
		cin >> command;
		found = false;


		// if inputted command was I then take input for a new student and insert new student (if ID is not yet used)
		if (command == "I") {
			cin >> Name >> ID >> Grade;
			currentStudent = {Name, ID, Grade};

			// if statement true if a student with the same ID is found
			if (students.find(currentStudent) != students.end()) {
				cout << "Error: Cannot insert duplicate ID" << endl;
			}
			else {
				students.insert(currentStudent);
			}
		}


		// if inputted command was R then take input ID and remove student with that ID (if they exist)
		else if (command == "R") {
			cin >> ID;
			currentStudent = {"Name", ID, 0};

			// if statement true if a student with this ID is found
			if (students.find(currentStudent) != students.end()) {
				students.erase(*students.find(currentStudent));
			}
			else {
				cout << "Error: Cannot remove non-existent ID" << endl;
			}
		}


		// if inputted command was Q then take second command
		else if (command == "Q") {
			cin >> command2;
			found = false;

			// if second command was i then print student with ID from input
			if (command2 == "i") {
				cin >> ID;
				currentStudent = {"Name", ID, 0};

				if (students.find(currentStudent) != students.end()) {
					StudentRecord element = *students.find(currentStudent);
					cout << "Name: " << element.name << ", ID: " << element.id << ", Grade: " << element.grade << endl;
					found = true;
				}
			}
			// if second command was n then print all students with Name from input
			else if (command2 == "n") {
				cin >> Name;
				for (auto element : students) {
					if (element.name == Name) {
						cout << "Name: " << element.name << ", ID: " << element.id << ", Grade: " << element.grade << endl;
						found = true;
					}
				}
			}
			// if second command was g then print all students with Grade from input
			else if (command2 == "g") {
				cin >> Grade;
				for (auto element : students) {
					if (element.grade == Grade) {
						cout << "Name: " << element.name << ", ID: " << element.id << ", Grade: " << element.grade << endl;
						found = true;
					}
				}
			}

			// if a student with the inputted information is not found
			if (found == false) {
				cout << "Error: No matches found" << endl;
			}
		}


		// if inputted command was S then break the while loop (end the program)
		else if (command == "S") {
			break;
		}
	}


	return 0;
}