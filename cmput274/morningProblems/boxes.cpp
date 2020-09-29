#include <iostream>
using namespace std;

int main() {
    // read in the input
    int n, m;
    cin >> n >> m;
    int a[n];
    for (int i = 0; i < m; i++) {
        cin >> a[i];
    }
    int result;

    // now solve the problem and put your answer in result

    int smallest = a[0];
    for(int i = 1; i < m; i++) {
        if(smallest > a[i]) {
            smallest = a[i];
        }
    }
    
    result = n - smallest + 1;

    // output the result
    cout << result << endl;
}