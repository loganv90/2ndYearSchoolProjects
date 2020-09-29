#include <iostream>
using namespace std;

int main() {
	int time;
	int cars;

	cin >> time >> cars;

	int array[cars];
	int speed;

	for (int i=0; i<cars; i++) {
		cin >> speed;
		array[i] = i+speed*time;
	}

	int counter = 0;

	for (int i=0; i<cars; i++) {
		for (int j=i+1; j<cars; j++) {
			if (array[j] < array[i]) {
				counter++;
			}
		}
	}

	cout << counter << endl;

	return 0;
}
