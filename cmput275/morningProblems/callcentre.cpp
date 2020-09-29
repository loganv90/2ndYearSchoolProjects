#include <iostream>
#include <unordered_map>
using namespace std;

int main() {
	int seconds;
	int calls;

	cin >> seconds >> calls;


	int sumArray[seconds];
	int num;
	int total = 0;

	sumArray[0] = 0;
	for (int i=1; i<seconds+1; i++) {
		cin >> num;
		total += num;
		sumArray[i] = total;
	}


	int start;
	int end;
	int totalCall[calls];

	for (int i=0; i<calls; i++) {
		cin >> start >> end;
		cout << sumArray[end] - sumArray[start-1] << endl;
	}


	return 0;
}