#include <iostream>
#include <queue> // may or may not be useful

using namespace std;

int main() {
	int players;
	int passes;

	cin >> players >> passes;

	queue <int> circle;
	for (int i=1; i<players; i++) {
		circle.push(i);
	}
	circle.push(0);

	int element;
	while (circle.size() > 1) {
		for (int i=0; i<passes-1; i++) {
			element = circle.front();
			circle.pop();
			circle.push(element);
		}
		circle.pop();
	}

	cout << circle.front() << endl;

	return 0;
}
