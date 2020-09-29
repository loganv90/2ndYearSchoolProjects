#include <iostream>

using namespace std;

int main() {
	int n, k, s;

	int seat;
	int total = 0;

	cin >> n >> k >> s;

	int values[n];
	int friends[n];

	for (int i=0; i<n; i++) {
		cin >> values[i];
	}

	for (int i=0; i<k; i++) {
		cin >> friends[i];
	}

	for (int i=0; i<k; i++) {
		seat = (friends[i] + s)%n;
		total += values[seat];
	}

	cout << total << endl;

	return 0;
}
