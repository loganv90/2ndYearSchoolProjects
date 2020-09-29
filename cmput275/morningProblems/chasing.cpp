#include <iostream>

using namespace std;

int main() {
	int64_t start;
	int64_t length;
	int64_t array[1000];
	cin >> start >> length;

	for (int i=0; i<length; i++) {
		cin >> array[i];
	}

	int64_t current = start;
	bool answer;
	int64_t value = 0;
	int i=0;
	while (i<1000) {
		current = array[current - start];

		if (current < 0) {
			answer = true;
			value = current;
			break;
		}

		if (current == start) {
			answer = false;
			break;
		}

		i++;
	}

	if (answer == false) {
		cout << "There was a cycle" << endl;
	}
	else {
		cout << value << endl;
	}

	return 0;
}
