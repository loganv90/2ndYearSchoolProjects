#include <iostream>

using namespace std;

int main() {
	int a[100], b[100], product[200];
	int n, m;

	cin >> n >> m;

	for (int i=n-1; i>-1; i--) {
		cin >> a[i];
	}
	for (int i=m-1; i>-1; i--) {
		cin >> b[i];
	}
	for (int i=0; i<200; i++) {
		product[i] = 0;
	}

	int carry;
	int multiplication;

	for (int i=0; i<n; i++) {

		carry = 0;

		for (int j=0; j<m; j++) {

			product[j+i] += a[i]*b[j];
			carry = product[j+i]/10;
			product[j+i] = product[j+i]%10;

			product[j+i+1] += carry;

		}
	}

	
	cout << product[m+n-1];
	for (int i=m+n-2; i>-1; i--) {
		cout << " " << product[i];
	}
	cout << endl;

	return 0;
}
