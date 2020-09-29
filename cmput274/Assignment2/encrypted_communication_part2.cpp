#include <Arduino.h>


//Global variables for pin names.
int server = 13;
int randomPin = A1;


void setup() {
	//Initializing serial ports and pin states.

    init();

    Serial.begin(9600);
    Serial3.begin(9600);
    pinMode(server,INPUT);
    pinMode(randomPin,INPUT);
}


uint32_t mulmod(uint32_t a, uint32_t b, uint32_t m) {
	//Uses the algorithm described in the worksheet on eclass to compute (a*b)%m.

    uint32_t total = 0;
    uint32_t num1 = a%m;
    uint32_t num2 = b%m;
    
    while (num2 > 0) {
        if (num2%2 == 1)
            total = (total + num1)%m;
            
        num1 = (num1 * 2)%m;
        num2 /= 2;
    }
    
    return total%m;
}


uint32_t powmod(uint32_t x, uint32_t pow, uint32_t m) {
	//This is the function provided in powmod.cpp with the respective mulmod lines replced with the function mulmod.

    uint32_t ans = 1;
    uint32_t pow_x = x;

    while (pow > 0) {
        if (pow&1 == 1) {
            ans = mulmod(ans,pow_x,m);
        }

        pow_x = mulmod(pow_x,pow_x,m);
        pow >>= 1;
    }

    return ans;
}


bool wait_on_serial3(uint8_t nbytes , long timeout) {
    unsigned long deadline = millis() + timeout; // wraparound not a problem

    while (Serial3.available() < nbytes && (timeout < 0 || millis() < deadline)) {
        delay (1); // be nice , no busy loop
    }

    return Serial3.available() >= nbytes;
}


uint32_t randomNum(uint32_t bits) {
	//This function calculates a random number of a given number of bits by using the least significant bits of multiple signals from randomPin.
	//This function re-uses the powmod funciton with a pre-set mod and therefore can only create random numbers up to 15 bits.

    uint32_t result = 0;

    for(uint32_t i = 0; i < bits-1; i++) {
        result += (analogRead(randomPin) & 1)*powmod(2,i,32768);
        delay(5);
    }

    result += pow(2,bits-1);

    return result;
}


bool isPrime(uint32_t num) {
	//Tests if the input num is a prime number by testing for divisibility.
	//The function is not designed to compute the primality of 2, 1, or 0.

    bool prime = true;

    for(uint32_t i = 2; i*i < num; i++) {
        if(num%i == 0) {
            prime = false;
        }
    }

    return prime;
}


uint32_t genPrime(uint32_t bits) {
	//This function used the randomNum and isPrime functions to generate a prime of a given number of bits.

    uint32_t var = randomNum(bits);

    while(not(isPrime(var))) {
        var = randomNum(bits);
    }

    return var;
}


uint32_t reduce_mod(uint32_t r, uint32_t q) {
	//Computes the mod of all numbers and includes a special case for negative numbers.

    int32_t result;

    if (r%q < 0) {
        result =  r%q;
        while(result < 0) {
            result += q;
        }
    }
    else {
        result = r%q;
    }

    return result;
}


int32_t findD(uint32_t phin, uint32_t e) {
	//Uses Euclid's Algorithm to find the value of d in the equation e*d=1mod(phin).

    uint32_t i=1;
    uint32_t r[40];
    int32_t s[40];
    int32_t t[40];
    uint32_t q;

    r[0] = phin;
    r[1] = e;
    s[0] = 1;
    s[1] = 0;
    t[0] = 0;
    t[1] = 1;

    while(r[i] > 0) {
        q = r[i-1]/r[i]; // integer division
        r[i+1] = reduce_mod(r[i-1],r[i]); // same as r[i-1] mod r[i]
        s[i+1] = s[i-1] - q*s[i];
        t[i+1] = t[i-1] - q*t[i];
        i = i+1;
    }

    return t[i-1];
}


uint32_t gcd_euclid_fast(uint32_t a, uint32_t b) {
	//This is the fast gcd function that we learned in class.

	while (b > 0) {
		a %= b;

    	// now swap them
    	uint32_t tmp = a;
    	a = b;
    	b = tmp;
	}

	return a; // b is 0
}


uint32_t findCoPrime(uint32_t phin) {
	//This function used the randomNum and gcd_euclid_fast functions to generate find the coprime of phin.

    uint32_t e = randomNum(15);

    while(gcd_euclid_fast(phin,e) != 1) {
        e = randomNum(15);
    }

    return e;
}


void  uint32_to_serial3(uint32_t  num) {
    Serial3.write((char) (num  >> 0));
    Serial3.write((char) (num  >> 8));
    Serial3.write((char) (num  >> 16));
    Serial3.write((char) (num  >> 24));
}


uint32_t  uint32_from_serial3() {
    uint32_t  num = 0;
    num = num | (( uint32_t) Serial3.read()) << 0;
    num = num | (( uint32_t) Serial3.read()) << 8;
    num = num | (( uint32_t) Serial3.read()) << 16;
    num = num | (( uint32_t) Serial3.read()) << 24;

    return  num;
}


int main() {
    setup();

    bool isServer = digitalRead(server);

    //Creates the names of the different states used in the handshaking process.
    enum StateNames {start, waitingForAckc, dataExchange, listen, waitingForKey1, waitingForKey2, waitingForAcks};

    //Creates the randomly generates keys.
    uint32_t p = genPrime(14);
    uint32_t q = genPrime(15);
    uint32_t n = p*q;
    uint32_t phin = (p-1)*(q-1);
    uint32_t e = findCoPrime(phin);
    int32_t d = findD(phin, e);

    //Corrects d if d is negative.
    while(d<0) {
        d = d + phin;
    }

    //Sets variables for the current state of the handshaking process and for sending/recieving keys.
    StateNames currentState;
    char CR = 'C';
    char ACK = 'A';
    uint32_t skey;
    uint32_t smod;
    uint32_t ckey;
    uint32_t cmod;

    //Code for the different states of the handshaking process.
    //Code is modeled after the state diagrams in the assignment description.
    if(isServer) {
    	//Runs if Arduino is the server.

        currentState = listen;
        skey = e;
        smod = n;

        while(not(currentState == dataExchange)) {
        	//Exits while loop when sync is reached.

            if(currentState == listen) {
            	if(wait_on_serial3(1,1000)) {
                	if(Serial3.read() == CR) {
                   		currentState = waitingForKey1;
                	}
            	}
            }

            if(currentState = waitingForKey1) {
                if(wait_on_serial3(8,1000)) {
                    ckey = uint32_from_serial3();
                    cmod = uint32_from_serial3();
                    Serial3.write(ACK);
                    uint32_to_serial3(skey);
                    uint32_to_serial3(smod);
                    currentState = waitingForAcks;
                }
                else {
                    currentState = listen;
                }
            }

            if(currentState = waitingForAcks) {
                if(wait_on_serial3(1,1000)) {
                    char character1 = Serial3.read();

                    if(character1 == ACK) {
                        currentState = dataExchange;
                    }

                    if(character1 == CR) {
                        currentState = waitingForKey2;
                    }
                }
                else {
                    currentState = listen;
                }
            }

            if(currentState == waitingForKey2){
                if(wait_on_serial3(8,1000)){
                    ckey = uint32_from_serial3();
                    cmod = uint32_from_serial3();
                    currentState = waitingForAcks;
                }
                else {
                    currentState = listen;
                }
            }
        }   
    }
    else {
    	//Runs if Arduino is the client.

        currentState = start;
        ckey = e;
        cmod = n;

        while(not(currentState == dataExchange)){
        	//Exits while loop when sync is reached.

            if(currentState == start){
                Serial3.write(CR);
                uint32_to_serial3(ckey);
                uint32_to_serial3(cmod);
                currentState = waitingForAckc;
            }

            if (currentState == waitingForAckc) {
                if(wait_on_serial3(9,1000)) {
                    char character2 = Serial3.read();
                    skey = uint32_from_serial3();
                    smod = uint32_from_serial3();

                    if(character2 == ACK) {
                    	currentState = dataExchange;
                        Serial3.write(ACK);
                    }
                    else {
                        currentState = start;
                    }
                }
                else {
                    currentState = start;
                }
            }
        }
    }

    Serial.println("Synchronization Successful");
    Serial3.flush();

    //Implements recieved variables.
    uint32_t m;
    if(isServer) {
        e = ckey;
        m = cmod;
    }
    else {
        e = skey;
        m = smod;
    }

    //The following code is the encryption and decryption done is part 1.
    while(true){
        if(Serial.available() > 0) {
            uint8_t byte1 = Serial.read();
            Serial.write(byte1);

            uint32_t encrypted_byte = powmod(byte1, e, m);

            uint32_to_serial3(encrypted_byte);

            if(byte1 == '\r'){
                Serial.write('\n');
                uint8_t newline = '\n';
                uint32_t sendnewline = powmod(newline,e,m);
                uint32_to_serial3(sendnewline);
            }
        }

        if(Serial3.available() >= 4) {
            uint32_t undecrypted_byte = uint32_from_serial3();

            uint32_t byte2 = powmod(undecrypted_byte, d, n);

            Serial.write(byte2);
        }
    }

    Serial.flush();
    Serial3.flush();

    return 0;
}