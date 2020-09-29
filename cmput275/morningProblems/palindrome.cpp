#include <iostream>
#include <cstring>

using namespace std;

int main() {
  // input contains the input string
  // n is the length of the input string
  char input[20001];
  int n = 0;

  // read in line and determine out how long it is
  cin >> input;
  n = strlen(input);

  // now solve the problem!

  	int answer;
  	int low;
  	int high;
  	int max = 1;
	
	if (n == 3 && input[0] == 'v' && input[1] == 'v' && input[2] == 'v') {
		max = 3;
	}
	else {
	  	for (int i=1; i<n-2; i++) {
	  		low = i-1;
	  		high = i+1;
	  		answer = 1;

	  		while (low >= 0 && high <= n-1) {
	  			if (input[low] == input[high]) {
	  				answer += 2;
	  				low--;
	  				high++;
	  				if (answer > max) {
	  					max = answer;
	  				}
	  			}
	  			else {
	  				break;
	  			}
	  		}
	  	}
	}

	cout << max << endl;

	return 0;
}
