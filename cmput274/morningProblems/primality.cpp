#include <iostream>
#include <cmath> // for sqrt

using namespace std;

// given a positive integer n <= 4,000,000 this returns the smallest
// integer d such that d*d > n
//
// not needed to solve the problem, but some might find it helpful
unsigned int upper_sqrt(unsigned int n) {
    unsigned int d = sqrt((double) n);

    // should iterate at most once or twice
    while (d*d <= n) {
        ++d;
    }

    return d;
}

int main() {
    unsigned int n;
    cin >> n;

    int ans = 0;

    int d;

    d = ceil(sqrt((double) n)) + 1;

    if(n == 1) {
        cout << "not prime" << endl;
    }
    else if(n == 0) {
        cout << "not prime" << endl;
    }
    else if(n == 2) {
        cout << "prime" << endl;
    }
    else {
        for(int i = 2; i < d; i++) {
            if(n%i == 0) {
                ans += 1;
            }
        }

        if(ans == 0) {
            cout << "prime" << endl;
        }
        else {
            cout << "not prime" << endl;
        }
    }
        
    return 0;
}
