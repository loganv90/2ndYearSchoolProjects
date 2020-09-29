/*
	Demonstrating cursor movement over the map of Edmonton. You will improve over
  what we do in the next weekly exercise.
*/

#define SD_CS 10
#define JOY_VERT  A9 // should connect A9 to pin VRx
#define JOY_HORIZ A8 // should connect A8 to pin VRy
#define JOY_SEL   53

#include <Arduino.h>

// core graphics library (written by Adafruit)
#include <Adafruit_GFX.h>

// Hardware-specific graphics library for MCU Friend 3.5" TFT LCD shield
#include <MCUFRIEND_kbv.h>

// LCD and SD card will communicate using the Serial Peripheral Interface (SPI)
// e.g., SPI is used to display images stored on the SD card
#include <SPI.h>

// needed for reading/writing to SD card
#include <SD.h>

#include "lcd_image.h"


MCUFRIEND_kbv tft;

#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320
#define YEG_SIZE 2048

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

#define JOY_CENTER   512
#define JOY_DEADZONE 64

#define CURSOR_SIZE 9

// the cursor position on the display
int cursorX, cursorY;

// forward declaration for redrawing the cursor
void redrawCursor(uint16_t colour);


void setup() {
  init();

  Serial.begin(9600);

	pinMode(JOY_SEL, INPUT_PULLUP);

	//    tft.reset();             // hardware reset
  uint16_t ID = tft.readID();    // read ID from display
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);
  if (ID == 0xD3D3) ID = 0x9481; // write-only shield
  
  // must come before SD.begin() ...
  tft.begin(ID);                 // LCD gets ready to work

	Serial.print("Initializing SD card...");
	if (!SD.begin(SD_CS)) {
		Serial.println("failed! Is it inserted properly?");
		while (true) {}
	}
	Serial.println("OK!");

	tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  // draws the centre of the Edmonton map, leaving the rightmost 60 columns black
	int yegMiddleX = YEG_SIZE/2 - (DISPLAY_WIDTH - 60)/2;
	int yegMiddleY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;
	lcd_image_draw(&yegImage, &tft, yegMiddleX, yegMiddleY,
                 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);

  // initial cursor position is the middle of the screen
  cursorX = (DISPLAY_WIDTH - 60)/2;
  cursorY = DISPLAY_HEIGHT/2;

  redrawCursor(TFT_RED);
}


//Redraws the Edmonton map in the space the cursor previously occupied. Because the variables cursorX and cursorY represent
//the center of the cursor, 4 must be subtracted from these variables to get the top left corner of the cursor.
void redrawMap() {
  int mapPointX = YEG_SIZE/2 - (DISPLAY_WIDTH - 60)/2 + cursorX - 4;
  int mapPointY = YEG_SIZE/2 - DISPLAY_HEIGHT/2 + cursorY - 4;

  lcd_image_draw(&yegImage, &tft, mapPointX, mapPointY,
                 cursorX - 4, cursorY - 4, CURSOR_SIZE, CURSOR_SIZE);
}


//Redraws the cursor in a given colour
void redrawCursor(uint16_t colour) {
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
               CURSOR_SIZE, CURSOR_SIZE, colour);
}


//Processes joystick movements
void processJoystick() {
  int xVal = analogRead(JOY_HORIZ);
  int yVal = analogRead(JOY_VERT);
  int buttonVal = digitalRead(JOY_SEL);

  redrawMap();

  //the more extreme the xVal and yVal values are the more the cursor will move according to these if statements
  if (yVal < JOY_CENTER - JOY_DEADZONE) {
    cursorY -= 1;
    if (yVal < 300) {
    	cursorY -= 2;
    	if(yVal < 100) {
    		cursorY -= 6;
    	}
    }
  }
  else if (yVal > JOY_CENTER + JOY_DEADZONE) {
    cursorY += 1;
    if (yVal > 700) {
    	cursorY += 2;
    	if(yVal > 950) {
    		cursorY += 6;
    	}
    }
  }

  if (xVal < JOY_CENTER - JOY_DEADZONE) {
    cursorX -= 1;
    if (xVal < 300) {
    	cursorX -= 2;
    	if(xVal < 100) {
    		cursorX -= 6;
    	}
    }
  }
  else if (xVal > JOY_CENTER + JOY_DEADZONE) {
    cursorX += 1;
    if (xVal > 700) {
    	cursorX += 2;
    	if(xVal > 950) {
    		cursorX += 6;
    	}
    }
  }

  //these constrain funcitons prevent the cursor from traveling beyond the border of the lcd screen
  //Because cursorX and cursorY represent the middle of the cursor, cursorX's min value must be 4 instead so the entire cursor remains on the screen
  cursorX = constrain(cursorX, 4, 415);
  cursorY = constrain(cursorY, 4, 315);

  redrawCursor(TFT_RED);

  delay(20);
}


int main() {
	setup();

  while (true) {
  	int xVal = analogRead(JOY_HORIZ);
    int yVal = analogRead(JOY_VERT);

    //what's inside the if function is only executed if the joystick has beed moved beyond the deadzone
    if (xVal > JOY_CENTER + JOY_DEADZONE || xVal < JOY_CENTER - JOY_DEADZONE || yVal > JOY_CENTER + JOY_DEADZONE || yVal < JOY_CENTER - JOY_DEADZONE) {
    	processJoystick();
    }
  }

	Serial.end();
	return 0;
}