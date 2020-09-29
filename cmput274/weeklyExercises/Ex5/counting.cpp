// Name: LOGAN VAUGHAN
// ID #:  1577489
// CMPUT  274  EA1

// Exercise 5: Counting Lights


#include <Arduino.h>
#define Increment 6
#define Decrement 7
#define Memory 5


// Global Variables:

int pinArray[5] = {9, 10, 11, 12, 13};
int ledArray[5] = {LOW, LOW, LOW, LOW, LOW};
int memoryArray[5];
bool memoryState = false;


// Turns on pull up resistors, sets pin modes.

void setup() {
	init();

	Serial.begin(9600);

	pinMode(Increment, INPUT);
	pinMode(Decrement, INPUT);
	pinMode(Memory, INPUT);

	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);

	digitalWrite(Increment, HIGH);
	digitalWrite(Decrement, HIGH);
	digitalWrite(Memory, HIGH);
}


// Increments 'ledArray' by 1.

int Add() {
	int i = 0;
	while(i < 5) {
		if(ledArray[i] == LOW) {
			ledArray[i] = HIGH;
			i = 5;
		} else {
			ledArray[i] = LOW;
			i += 1;
		}
	}

	return 0;
}


// Decrements 'ledArray' by 1.

int Subtract() {
	int i = 0;
	while(i < 5) {
		if(ledArray[i] == LOW) {
			ledArray[i] = HIGH;
			i += 1;
		} else {
			ledArray[i] = LOW;
			i = 5;
		}
	}

	return 0;
}


// Copies/saves 'ledArray' to 'memoryArray' and then sets all elements of 'ledArray' to LOW.
// Sets 'memoryState' so that the program will run the 'Remember' function the next time the memory button is pressed.

int Save() {
	int i;
	for(i = 0; i < 5; i++) {
		memoryArray[i] = ledArray[i];
		ledArray[i] = LOW;
	}

	memoryState = true;

	return 0;
}


// Replaces the values in 'ledArray' with the ones prevously saved to 'memoryArray'.
// Sets 'memoryState' so that the program will run the 'Save' function the next time the memory button is pressed.

int Remember() {
	int i;
	for(i = 0; i < 5; i++) {
		ledArray[i] = memoryArray[i];
	}

	memoryState = false;

	return 0;
}



int main() {
	setup();


	// Each if statement in this while loop is activated when its corresponding button is pressed. For example, 
	// 'digitalRead(Increment)' will be LOW when the incrementing button is pressed which will initiate the if 
	// statement. When the button is pressed, the program is held up by another while loop until that button is 
	// released. When the button is released, the rest of the if statement will execute.

	while(true) {
		if(digitalRead(Increment) == LOW) {
			bool buttonDown = true;
			while(buttonDown) {
				if(digitalRead(Increment) == HIGH) {
					buttonDown = false;
				}
			}

			Add();
		}

		if(digitalRead(Decrement) == LOW) {
			bool buttonDown = true;
			while(buttonDown) {
				if(digitalRead(Decrement) == HIGH) {
					buttonDown = false;
				}
			}

			Subtract();
		}

		if(digitalRead(Memory) == LOW) {
			bool buttonDown = true;
			while(buttonDown) {
				if(digitalRead(Memory) == HIGH) {
					buttonDown = false;
				}
			}

			if(memoryState == false) {
				Save();
			} else {
				Remember();
			}
		}


		// Updates the lights based on any changes that have been made to 'ledArray'.

		int i;
		for(i = 0; i < 5; i++) {
			digitalWrite(pinArray[i], ledArray[i]);
		}
	}

	Serial.flush();
	return 0;
}