#include <iostream>
using namespace std;

int main() {
	int n;
	cin >> n;

	int array[1000000];
	int max = 0;

	for (int i=0; i<n; i++) {
		array[i] = 0;
	}

	int height;
	for (int i=0; i<n; i++) {
		cin >> height;
		array[height] += 1;
		if (height > max) {
			max = height;
		}
	}

	int atHeight = n;

	for (int i=0; i<max; i++) {
		atHeight -= array[i];
		cout << atHeight << endl;
	}

	return 0;
}
