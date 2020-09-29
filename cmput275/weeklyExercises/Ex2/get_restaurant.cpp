/*
  Template for exercise 2. All required constants and tft/sd/touchscreen
  initializations are here, as is an implementation of getRestaurant
  (the slow version) that should be similar to what is covered in the lecture.

  Use it freely as a starting point for exercise 2.
*/

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SD.h>
#include <TouchScreen.h>

#define SD_CS 10

// physical dimensions of the tft display (# of pixels)
#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320

// touch screen pins, obtained from the documentaion
#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

// dimensions of the part allocated to the map display
#define MAP_DISP_WIDTH (DISPLAY_WIDTH - 60)
#define MAP_DISP_HEIGHT DISPLAY_HEIGHT

#define REST_START_BLOCK 4000000
#define NUM_RESTAURANTS 1066

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 940
#define TS_MAXY 920

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

MCUFRIEND_kbv tft;

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// different than SD
Sd2Card card;


struct restaurant {
  int32_t lat;
  int32_t lon;
  uint8_t rating; // from 0 to 10
  char name[55];
};


// restaurant cache global variables
restaurant restCache[8];
uint32_t cachedBlock = 0;


void setup() {
  init();
  Serial.begin(9600);

  // tft display initialization
  uint16_t ID = tft.readID();
  tft.begin(ID);

  tft.fillScreen(TFT_BLACK);
  tft.setRotation(1);

  tft.drawRect(420, 0, 60, 160, TFT_RED);
  tft.drawRect(420, 160, 60, 160, TFT_RED);

  tft.setTextSize(2);

  tft.setCursor(446, 40);
  tft.print("S");
  tft.setCursor(446, 60);
  tft.print("L");
  tft.setCursor(446, 80);
  tft.print("O");
  tft.setCursor(446, 100);
  tft.print("W");
  tft.setCursor(446, 200);
  tft.print("F");
  tft.setCursor(446, 220);
  tft.print("A");
  tft.setCursor(446, 240);
  tft.print("S");
  tft.setCursor(446, 260);
  tft.print("T");

  tft.setCursor(5, 5);
  tft.print("RECENT SLOW RUN:");
  tft.setCursor(5, 55);
  tft.print("SLOW RUN AVG:");
  tft.setCursor(5, 105);
  tft.print("RECENT FAST RUN:");
  tft.setCursor(5, 155);
  tft.print("FAST RUN AVG:");

  // SD card initialization for raw reads
  Serial.print("Initializing SPI communication for raw reads...");
  if (!card.init(SPI_HALF_SPEED, SD_CS)) {
    Serial.println("failed! Is the card inserted properly?");
    while (true) {}
  }
  else {
    Serial.println("OK!");
  }
}


// the implementation from class
void getRestaurant(int restIndex, restaurant* restPtr) {
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;
  restaurant restBlock[8];

  while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
    Serial.println("Read block failed, trying again.");
  }

  *restPtr = restBlock[restIndex % 8];
}


// fast version of the function getRestaurant
void getRestaurantFast(int restIndex, restaurant* restPtr) {
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;
  restaurant restBlock[8];

  if (blockNum == cachedBlock) {
  	*restPtr = restCache[restIndex % 8];
  }
  else {
  	while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
  	Serial.println("Read block failed, trying again.");
  	}

  	for (int i=0; i<8; i++) {
  		restCache[i] = restBlock[i];
  	}
  	cachedBlock = blockNum;

  	*restPtr = restBlock[restIndex % 8];
  }
}


// updates the times on the tft display
// if fast is true then it will update the times of the fast runs if fast is false then it will update the slow runs
void textUpdate(int time, int totalTime, int runs, bool fast) {
	int yVal = 25;
	if (fast) {
		yVal = 125;
	}

	tft.fillRect(5, yVal, 300, 30, TFT_BLACK);
	tft.setCursor(5, yVal);
    tft.print(restCache[0].lon);
    tft.print(" ms");

	tft.fillRect(5, yVal + 50, 200, 30, TFT_BLACK);
    tft.setCursor(5, yVal + 50);
    tft.print(restCache[0].lat);
    tft.print(" ms");
}


int main() {
  setup();

  int fastTotalTime = 0;
  int slowTotalTime = 0;
  int fastRuns = 0;
  int slowRuns = 0;
  bool fast;

  restaurant restVar;
  int start;
  int time;

  while (true) {
  	// queries touchscreen and resets pin states
  	TSPoint touch = ts.getPoint();
  	pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    // only executes if statements when a button is pressed
  	if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE && touch.y < 190) {
  		if (touch.x <= 510) {
  			start = millis();

  			for (int i=0; i<NUM_RESTAURANTS; i++) {
  				getRestaurantFast(i, &restVar);
  			}

  			time = millis() - start;

  			fastTotalTime += time;
  			fastRuns += 1;
  			fast = true;
  			textUpdate(time, fastTotalTime, fastRuns, fast);
  		}
  		else {
  			start = millis();

  			for (int i=0; i<NUM_RESTAURANTS; ++i) {
    			getRestaurant(i, &restVar);
  			}

  			time = millis() - start;

  			slowTotalTime += time;
  			slowRuns += 1;
  			fast = false;
  			textUpdate(time, slowTotalTime, slowRuns, fast);
  		}
  	}
  }

  Serial.end();
  return 0;
}