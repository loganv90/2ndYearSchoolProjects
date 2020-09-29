#include <iostream>

using namespace std;

int main() {
    // Read the input
    int d1, d2, d3, d4;
    cin >> d1 >> d2 >> d3 >> d4;
    bool correct;

    // Put your code here!

    correct = false;
    int product = d1*1 + d2*2 + d3*3 + d4*4;
    if(product%5 == 0) {
        correct = true;
    }

    // Output the solution
    if (correct) {
        cout << "yes" << endl;
    } else {
        cout << "no" << endl;
    }
    return 0;
}
