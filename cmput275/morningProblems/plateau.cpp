#include <iostream>
#include <algorithm> // for max()

using namespace std;

int main() {
	int n;

	cin >> n;

	int array[n];

	for (int i=0; i<n; i++) {
		cin >> array[i];
	}

	int count = 1;
	int max = 0;
	for (int i=1; i<n; i++) {
		if (array[i - 1] == array[i]) {
			count += 1;
		}
		else {
			count = 1;
		}

		if (count > max) {
			max = count;
		}
	}

	cout << max << endl;

	return 0;
}
