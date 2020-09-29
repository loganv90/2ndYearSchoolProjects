#include <utility>
#include <stack>
#include <iostream>
using namespace std;


// global variable for the stack
stack <pair<long long, long long>> pile;


// function that executes operations on elements in the stack
void fixPile(string Op, string type) {
	// takes first element off the stack and makes a varibale for the new element
	pair <long long, long long> newCompNum;
	pair <long long, long long> firstCompNum = pile.top();
	pile.pop();

	// if a unary operator is inputted
	if (type == "U") {
		// if unary operator is conjuagtion
		if (Op == "c") {
			newCompNum.first = firstCompNum.first;
			newCompNum.second = -firstCompNum.second;
		}
		// if unary operator is negation
		else if (Op == "-") {
			newCompNum.first = -firstCompNum.first;
			newCompNum.second = -firstCompNum.second;
		}
	}
	// if a binary operator is inputted
	else if (type == "B") {
		// takes second element off the stack
		pair <long long, long long> secondCompNum = pile.top();
		pile.pop();

		// if binary operator is subtraction
		if (Op == "-") {
			newCompNum.first = secondCompNum.first - firstCompNum.first;
			newCompNum.second = secondCompNum.second - secondCompNum.first;
		}
		// if binary operator is addition
		else if (Op == "+") {
			newCompNum.first = secondCompNum.first + firstCompNum.first;
			newCompNum.second = secondCompNum.second + firstCompNum.second;
		}
		// if binary operator is multiplication
		else if (Op == "*") {
			newCompNum.first = secondCompNum.first*firstCompNum.first - secondCompNum.second*firstCompNum.second;
			newCompNum.second = secondCompNum.first*firstCompNum.second + secondCompNum.second*firstCompNum.first;
		}
	}

	// put new number onto stack
	pile.push(newCompNum);
}


int main() {
	// input commands
	string command;
	string binaryOp;
	string unaryOp;

	// variable of class pair to store complex numbers
	// First variable in pair will contain the real part of the complex number and second variable will contain the imaginary part
	pair <long long, long long> complexNumber;

	while (true) {
		cin >> command;

		// if inputted command was V then take input for a number and 
		if (command == "V") {
			cin >> complexNumber.first >> complexNumber.second;
			pile.push(complexNumber);
		}
		// if inputted command was B then take input for a binary operator and call fixPile
		else if (command == "B") {
			cin >> binaryOp;
			fixPile(binaryOp, "B");
		}
		// if inputted command was U then take input for a unary operator and call fixPile
		else if (command == "U") {
			cin >> unaryOp;
			fixPile(unaryOp, "U");
		}
		// if inputted command was S then break the while loop (end the program)
		else if (command == "S") {
			break;
		}
	}

	// output answer of inputted equations
	complexNumber = pile.top();
	cout << complexNumber.first << " " << complexNumber.second << endl;

	return 0;
}