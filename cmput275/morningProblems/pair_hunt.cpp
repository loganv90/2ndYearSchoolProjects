#include <iostream>
#include <unordered_set>

using namespace std;

int main() {
	int items;
	int total;

	cin >> items >> total;

	int price;
	unordered_set<int> mySet;

	for (int i=0; i<items; i++) {
		cin >> price;

		mySet.insert(price);
	}


	int requiredPrice;
	int counter = 0;

	for (auto element : mySet) {
		requiredPrice = total - element;

		if (mySet.find(requiredPrice) != mySet.end()) {
			counter++;
		}
	}

	cout << counter/2 << endl;

	return 0;
}
