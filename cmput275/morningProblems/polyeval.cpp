#include <iostream>

using namespace std;

int main() {
  // Declare your variables

  // Read the input

  // Solve the problem


	unsigned int d, x, m;

	cin >> d >> x >> m;

	unsigned int array[100000];

	for (int i=0; i<=d; i++) {
		cin >> array[i];
	}

	unsigned int var;
	unsigned int total = 0;
	for (unsigned int i=0; i<=d; i++) {
		if (i == 0) {
			var = 1;
		}
		else {
			var *= x;
			var = var%m;
		}

		total += array[i]*var;
		total = total%m;
	}


	unsigned int answer = total%m;

	cout << answer << endl;

	return 0;
}
