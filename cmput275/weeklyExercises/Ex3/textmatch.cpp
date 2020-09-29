#include <iostream>
#include <cstring>

using namespace std;



void textmatch(const char *s, const char *t) {
	//universal vars:
	int len1 = strlen(s);
	int len2 = strlen(t);
	int step[len1];


	//phase1:
	step[0] = -1;
	int k = -1;

	for (int i=1; i<len1; i++) {
		while (k>=0 && s[k+1] != s[i]) {
			k = step[k];
		}

		if (s[k+1] == s[i]) {
			k += 1;
		}

		step[i] = k;
	}


	//phase2:
	int m = -1;

	for (int i=0; i<len2; i++) {
		while (m>=0 && s[m+1] != t[i]) {
			m = step[m];
		}

		if (s[m+1] == t[i]) {
			m += 1;
		}

		if (m == len1 - 1) {
			cout << i + 1 - len1 << " ";
			m = step[m];
		}
	}

	cout << endl;
}



int main() {
	//uses strings to take input then changes them to character arrays:
	std::string str1;
	std::string str2;

	cin >> str1 >> str2;

	const char *s = str1.c_str();
	const char *t = str2.c_str();

	textmatch(s, t);

	return 0;
}