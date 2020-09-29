#include <iostream>

using namespace std;


void flipFlopSort(unsigned int array[200], int start, int end) {
	int length = end - start + 1;
	//takes the ceiling of (2*length/3) witout using float values:
	int elements = (2*length/3) + (2*length%3 != 0);
	unsigned int temp;

	if (length <= 2) {
		if (array[start] > array[end]) {
			//swaps array[start] and array[end]:
			temp = array[start];
			array[start] = array[end];
			array[end] = temp;
		}
	}
	else {
		flipFlopSort(array, start, start + elements - 1);
		flipFlopSort(array, end - elements + 1, end);
		flipFlopSort(array, start, start + elements - 1);
	}
}


int main() {
	//initializing vars:
	int n;
	unsigned int array[200];

	//input:
	cin >> n;
	for (int i=0; i<n; i++) {
		cin >> array[i];
	}

	//computation:
	flipFlopSort(array, 0, n-1);

	//output:
	cout << array[0];
	for (int i=1; i<n; i++) {
		cout << " " << array[i];
	}
	cout << endl;

	return 0;
}