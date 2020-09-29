#include <iostream>
using namespace std;

int main() {
	long long start, size, instructions;
	cin >> start >> size >> instructions;

	string character;
	long long array[size];
	for (int i=0; i<size; i++) {
		array[i] = 0;
	}

	long long val, addr, addr1, addr2, current, answer;
	for (int i=0; i<instructions; i++) {
		cin >> character;

		if (character == "S") {
			cin >> addr >> val;
			array[addr-start] = val;
		}
		else if (character == "P") {
			cin >> addr1 >> addr2;
			array[addr1-start] = addr2;
		}
		else if (character == "C") {
			cin >> addr;

			current = addr;
			answer = 0;
			int i = 0;
			while (i<size) {
				current = array[current - start];

				if (current < 0) {
					answer = 1;
					cout << current << endl;
					break;
				}
				else if (current<start) {
					answer = 2;
					cout << "Out of bounds" << endl;
					break;
				}
				else if (current>start+size-1) {
					answer = 2;
					cout << "Out of bounds" << endl;
					break;
				}

				i++;
			}

			if (answer == 0) {
				cout << "There was a cycle" << endl;
			}
		}
	}

	return 0;
}