// Name: LOGAN VAUGHAN
// ID #:  1577489
// CMPUT  274  EA1

// Exercise  7: Euler’s Phi Function


#include <iostream>
#include <iomanip>


using namespace std;


unsigned int firstPrimeDivisor(unsigned int num) {
	//This function uses a while loop to iterate through all numbers from 2 to sqrt(num)
	//and to check if they divide num. If a number is found then that number is the first 
	//prime divisor and if not then the first prime divisor is set to num.

	bool noPrime = true;
	unsigned int i = 1, prime;

	while(noPrime) {
		i++;

		if(num%i == 0) {
			prime = i;
			noPrime = false;
		}
		else if(i*i > num) {
			prime = num;
			noPrime = false;
		}
	}

	return prime;
}


unsigned int phi(unsigned int n) {
	//This function will calculate phi(n) by finding all the prime divisors of n with the
	//firstPrimeDivisor function and by changing n on every iteration of the while loop.
	//On every iteration of the while loop phiofn (the variable name for phi(n)) will also 
	//be updated.

	unsigned int phiofn = n;

	while(n != 1) {
		unsigned int p = firstPrimeDivisor(n);

		while(n%p == 0) {
			n /= p;
		}

		phiofn /= p;
		phiofn *= p-1;
	}

	return phiofn;
}


unsigned int oneGCDs(unsigned int n) {
	//This function is used to count all the numbers k where 2^14<=k<2^15 and gcd(n,k)=1.
	//The while loop finds the GCD of k and n and then count is increased for every iteration
	//of the for loop if this GCD is 1. Note: 2^14=16384 and 2^15=32768.

	unsigned int count = 0;

	for(unsigned int k = 16384; k < 32768; k++) {
		unsigned int num1 = n, num2 = k;

		while(num2 > 0) {
			num1 %= num2;

			unsigned int temp = num1;
			num1 = num2;
			num2 = temp;
		}

		if(num1 == 1) {
			count += 1;
		}
	}

	return count;
}


int main() {
	unsigned int n;
	cin >> n;

	unsigned int phiofn = phi(n);

	cout << "n           = " << n << endl;
	cout << "phi(n)      = " << phiofn << endl;
	cout << "phi(n)/n    = " << fixed << setprecision(5) << (double)phiofn/(double)n << endl;
	
	if(n >= 32768) {
		unsigned int numofk = oneGCDs(n);

		cout << "15-bit test = " << (double)numofk/16384 << endl;
	}

	return 0;
}