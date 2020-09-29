#include <iostream>

using namespace std;

int main() {
	int friends;

	cin >> friends;

	int numOfFav[friends];
	int favNums[1000000];
	int numCounter = 0;

	for (int i=0; i<friends; i++) {
		cin >> numOfFav[i];

		for (int j=0; j<numOfFav[i]; j++) {
			cin >> favNums[numCounter];
			numCounter++;
		}
	}

	int friendNum;
	cin >> friendNum;

	int numsBefore = 0;

	for (int i=0; i<friendNum; i++) {
		numsBefore += numOfFav[i];
	}

	cout << favNums[numsBefore];
	for (int i=numsBefore+1; i<numsBefore+numOfFav[friendNum]; i++) {
		cout << " " << favNums[i];
	}
	cout << endl;

	return 0;
}
