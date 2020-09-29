#include <iostream>
using namespace std;

int main() {
	int n;
	int array[1000];
	int answer = 0;
	cin >> n;

	for(int i = 0; i < n; ++i) {
		cin >> array[i];
	}

	if(n == 1) {
		cout << 0 << endl;
	}

	else {
		int ansArray[1000];

		for(int i = 0; i < n; i++) {
			int count = 0;

			for(int j = i+1; j < n; j++) {
				if(array[j-1] < array[j]) {
					count += 1;
				}
				else {
					break;
				}
			}

			ansArray[i] = count;
		}

		cout << ansArray[0];
		for(int i = 1; i < n; i++) {
			cout << " " << ansArray[i];
		}
		cout << endl;
	}

	return 0;
}