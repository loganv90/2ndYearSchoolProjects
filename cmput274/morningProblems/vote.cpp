 #include <iostream>

using namespace std;

int main() {
    int n = 0;
    int vote[1001];

    // keep reading until we see 0
    while (cin >> vote[n] && vote[n] != 0) {
        ++n;
    }

    // now n == # votes and vote[i] is the i'th vote
    // for 0 <= i <= n-1

    // solve the problem and print the answer
    // good luck! (and remember to vote next election) :)

    int num[1000] = {};

    for(int i = 0; i < n; i++) {
    	int var = vote[i];
    	num[var] += 1;
    }


    int biggest = 0;
    bool tie = false;
    int index;

    for(int i = 1; i < 1000; i++) {
    	if(num[i] >= biggest) {
    		if(num[i] != biggest) {
    			tie = false;
    		}
    		else {
    			tie = true;
    		}

    		biggest = num[i];
    		index = i;
    	}
    }

    if(tie) {
    	cout << "tie" << endl;
    }
    else {
    	cout << index << endl;
    }

    return 0;
}
