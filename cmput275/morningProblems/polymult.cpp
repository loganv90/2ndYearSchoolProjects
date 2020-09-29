#include <iostream>

using namespace std;

int main() {

	int m, n;

	cin >> m >> n;


	int pol1[2000];
	int pol2[2000];

	for (int i=0; i<m+1; i++) {
		cin >> pol1[i];
	}

	for (int i=0; i<n+1; i++) {
		cin >> pol2[i];
	}


	int answer[4000];
	int var;
	int j;

	for (int k=0; k<m+n+1; k++) {
		var = 0;

		for (int i=0; i<m+1; i++) {
			if(i <= k) {
				j = k-i;
				var += pol1[i]*pol2[j];
			}
		}

		answer[k] = var;
	}

	cout << answer[0];
	for (int i=1; i<m+n+1; i++) {
		cout << " " << answer[i];
	}
	cout << endl;

	return 0;
}
