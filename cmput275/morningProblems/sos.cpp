#include <iostream>
#include <cmath> // for sqrt

using namespace std;

// Compute the greatest integer d such that d*d <= n
// (i.e. the floor of the square root).
//
// You may use this function or ignore it, it's up to you.
unsigned int integer_sqrt(unsigned int x) {
  unsigned int d = sqrt(x);

  // should iterate at most once, probably none
  while ((d+1)*(d+1) <= x) {
    ++d;
  }

  // probably does not iterate even once
  while (d*d > x) {
    --d;
  }

  // now at this point we know (d+1)*(d+1) > x yet d*d <= x

  return d;
}

int main() {
	unsigned int n;
	bool ans = false;

	cin >> n;

	unsigned int a = integer_sqrt(n);

	for (unsigned int i=0; i<=a; i++) {
		unsigned int b = integer_sqrt(n - i*i);

		if (i*i + b*b == n) {
			ans = true;
		}
	}

	if (ans == true) {
		cout << "sum of squares" << endl;
	}
	else {
		cout << "not sum of squares" << endl;
	}


	return 0;
}
