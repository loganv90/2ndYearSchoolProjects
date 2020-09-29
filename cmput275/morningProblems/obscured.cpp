#include <iostream>
#include <stack>
using namespace std;

int main() {
	int people;
	cin >> people;

	int height[people];
	cin >> height[0];

	stack<int> indexs;
	indexs.push(0);

	cout << "X";

	for (int i=1; i<people; i++) {
		cin >> height[i];

		while (!indexs.empty()) {
			if (height[indexs.top()] >= height[i]) {
				cout << " " << indexs.top();
				indexs.push(i);
				break;
			}
			else {
				indexs.pop();
			}
		}

		if (indexs.size() == 0) {
			indexs.push(i);
			cout << " " << "X";
		}
	}

	cout << " ";

	return 0;
}