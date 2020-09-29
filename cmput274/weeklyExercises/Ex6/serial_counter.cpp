// Name: LOGAN VAUGHAN
// ID #:  1577489
// CMPUT  274  EA1

// Exercise 6: Serial Counter


#include <Arduino.h>


int pinArray[5] = {9, 10, 11, 12, 13};


void setup() {
	init();

	Serial.begin(9600);

	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);
}


// Updates the LEDs using a for loop and bitwise operators. 'binaryNum' is the value of the 8 bit integer 'total' that
// has been shifted by 3+i to the left. For every iteration of the for loop, 'binaryNum' is compared to 128 which is
// 10000000 in binary. If 'binaryNum' is bigger or equal to 128 then position 4-i in the binary representation of 'total'
// must be a 1. If 'binaryNum' is smaller then 128 then the position is a 0.

void updateLEDs(uint8_t total) {
	for(int i = 0; i < 5; i++) {
		uint8_t binaryNum = (total << (3+i));

		if(binaryNum >= 128 ) {
			digitalWrite(pinArray[4 - i], HIGH);
		}
		else {
			digitalWrite(pinArray[4 - i], LOW);
		}
    }
}


// Gets the hexadecimal value of the character 'digit'. Hex value of 0-9 is 0-9. Hex value of a-f or A-F is 10-15. Other
// characters will have the funciton return -1.

uint8_t getHexValue(char digit) {
    uint8_t asciiValue = int(digit);
    
    if(asciiValue >= 97 && asciiValue <= 102) {
        return asciiValue - 87;
    }
    else if(asciiValue >= 65 && asciiValue <= 70) {
        return asciiValue - 55;
    }
    else if(asciiValue >= 48 && asciiValue <= 58) {
        return asciiValue - 48;
    }
    else {
    	return -1;
    }
}


// 'total' keeps track of all the numbers that have been typed in the serial monitor and can be reset to 0 by pressing the 
// spacebar or enter key in the serial monitor. The main function waits until something is typed into the serial monitor 
// then checks if the character typed was space or enter. If not then it will try to find the hex value of the character and 
// then it will update the LEDs accordingly. If the hex value is returned as -1 (-1 becomes 255 when returned as an unsigned 
// 8 bit integer) then the LEDs will not be updated and 'total' will not be changed.

int main() {
	setup();
	uint8_t total = 0;

	while(true) {

		if(Serial.available() > 0) {
			char character = Serial.read();

			if(int(character) == 32 or int(character) == 13) {
				total = 0;
				updateLEDs(total);
			}
			else {
				uint8_t value = getHexValue(character);

				if(value != 255) {
					total += value;
					updateLEDs(total);
				}
			}
		}
	}

	Serial.flush();
	return 0;
}