#include <iostream>
#include <stack>
using namespace std;

int main() {
	int westArrivals, eastArrivals;
	cin >> westArrivals >> eastArrivals;
	int westTimes[westArrivals], eastTimes[eastArrivals];
	for (int i=0; i<westArrivals; i++) {
		cin >> westTimes[i];
	}
	for (int i=0; i<eastArrivals; i++) {
		cin >> eastTimes[i];
	}

	int vehicles = westArrivals + eastArrivals;
	bool westSide = true;
	int westCar = 0;
	int eastCar = 0;
	int time = 0;

	while (vehicles > 0) {
		// take west side car next
		if ((eastTimes[eastCar] > westTimes[westCar] or eastCar >= eastArrivals) and (westCar < westArrivals)) {
			if (time < westTimes[westCar]) {
				time = westTimes[westCar];
			}

			if (westSide == false) {
				time += 100;
			}

			time += 100;
			westSide = false;
			westCar++;
		}

		// take east side car next
		else if ((eastTimes[eastCar] < westTimes[westCar] or westCar >= westArrivals) and (eastCar < eastArrivals)) {
			if (time < eastTimes[eastCar]) {
				time = eastTimes[eastCar];
			}

			if (westSide == true) {
				time += 100;
			}

			time += 100;
			westSide = true;
			eastCar++;
		}

		vehicles--;
	}

	cout << time << endl;

	return 0;
}