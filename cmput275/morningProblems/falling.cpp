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

	int fall = 0;
	int max = 0;

	for (int i=1; i<n; i++) {
		if (array[i - 1] > array[i]) {
			fall += (array[i - 1] - array[i]);
		}
		else {
			fall = 0;
		}

		if (fall > max) {
			max = fall;
		}
	}

	cout << max << endl;

  return 0;
}
