#include <iostream>

using namespace std;

int main() {
  // Declare your variables

  // Read the input

  // Solve the problem
  // Last one! Good luck :)

	int numbers;

	cin >> numbers;

	int num1[numbers - 1];
	int num2[numbers - 1];
	int result[numbers - 1];
	int car = 0;

	for(int i = 0; i < numbers; i++) {
		cin >> num1[i];
	}

	for(int i = 0; i < numbers; i++) {
		cin >> num2[i];
	}

	for(int i = 0; i < numbers; i++) {
		int var = num1[numbers - i - 1] + num2[numbers - i - 1] + car;

		car = 0;

		if(var >= 10) {
			var -= 10;
			car = 1;
		}

		result[numbers - i - 1] = var;
	}

	if(car == 1) {
		cout << 1 << " ";
	}

	cout << result[0];
	for(int i = 1; i < numbers; i++) {
		cout << " " << result[i];
	}
	cout << endl;

	return 0;
}