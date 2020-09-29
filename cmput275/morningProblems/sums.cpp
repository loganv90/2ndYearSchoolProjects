#include <iostream>
#include <bits/stdc++.h>

using namespace std;

int main() {
	int n;

	cin >> n;

	unordered_set <int> lastrow;
	int list1[1000];
	int list2[1000];
	int list3[1000];

	for (int i=0; i<n; i++) {
		cin >> list1[i];
	}

	for (int i=0; i<n; i++) {
		cin >> list2[i];
	}

	for (int i=0; i<n; i++) {
		cin >> list3[i];
	}

	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			lastrow.insert(list1[i] + list2[j]);
		}
	}


	int count = 0;

	for (int i=0; i<n; i++) {
		if (lastrow.find(list3[i]) == lastrow.end()) {
		}
		else {
			count++;
		}
	}

	cout << count << endl;

    return 0;
}