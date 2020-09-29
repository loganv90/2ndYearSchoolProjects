#define SD_CS 10
#define JOY_VERT  A9 // should connect A9 to pin VRx
#define JOY_HORIZ A8 // should connect A8 to pin VRy
#define JOY_SEL   53

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <SPI.h>
#include <SD.h>
#include <TouchScreen.h>
#include "lcd_image.h"
#include <stdlib.h>

MCUFRIEND_kbv tft;

#define DISPLAY_WIDTH  480
#define DISPLAY_HEIGHT 320
#define YEG_SIZE 2048

lcd_image_t yegImage = { "yeg-big.lcd", YEG_SIZE, YEG_SIZE };

#define JOY_CENTER   512
#define JOY_DEADZONE 64

#define CURSOR_SIZE 9

// touch screen pins, obtained from the documentaion
#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

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

//  These  constants  are  for  the  2048 by 2048  map.
#define  MAP_WIDTH  2048
#define  MAP_HEIGHT  2048
#define  LAT_NORTH  5361858l
#define  LAT_SOUTH  5340953l
#define  LON_WEST  -11368652l
#define  LON_EAST  -11333496l

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// different than SD
Sd2Card card;

// the cursor position on the display
int cursorX, cursorY;

// coordinate of YEG image to be navigated. defaults at the middle.
int yegX = YEG_SIZE/2 - (DISPLAY_WIDTH - 60)/2;
int yegY = YEG_SIZE/2 - DISPLAY_HEIGHT/2;

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

  tft.setTextSize(1);

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



//  These  functions  convert  between x/y map  position  and  lat/lon
// (and  vice  versa.)
int32_t  x_to_lon(int16_t x) {
  return  map(x, 0, MAP_WIDTH , LON_WEST , LON_EAST);
}
int32_t  y_to_lat(int16_t y) {
  return  map(y, 0, MAP_HEIGHT , LAT_NORTH , LAT_SOUTH);
}
int16_t  lon_to_x(int32_t  lon) {
  return  map(lon , LON_WEST , LON_EAST , 0, MAP_WIDTH);
}
int16_t  lat_to_y(int32_t  lat) {
  return  map(lat , LAT_NORTH , LAT_SOUTH , 0, MAP_HEIGHT);
}



struct  restaurant {
  int32_t  lat; //  Stored  in 1/100 ,000  degrees
  int32_t  lon; //  Stored  in 1/100 ,000  degrees
  uint8_t  rating; // 0-10: [2 = 1 star , 10 = 5 stars]
  char  name [55];  //  already  null  terminated  on the SD card
};

struct  RestDist {
  uint16_t  index; // index  of  restaurant  from 0 to  NUM_RESTAURANTS -1
  uint16_t  dist;  //  Manhatten  distance  to  cursor  position
};

// restaurant global variables
restaurant restCache[8];
uint32_t cachedBlock = 0;
RestDist rest_dist[NUM_RESTAURANTS];



void getRestaurant(uint16_t restIndex, restaurant* restPtr) {
  uint32_t blockNum = REST_START_BLOCK + restIndex/8;
  restaurant restBlock[8];

  if (blockNum == cachedBlock) {
    *restPtr = restCache[restIndex % 8];
  }
  else {
    while (!card.readBlock(blockNum, (uint8_t*) restBlock)) {
    Serial.println("Read block failed, trying again.");
    }

    for (uint8_t i=0; i<8; i++) {
      restCache[i] = restBlock[i];
    }
    cachedBlock = blockNum;

    *restPtr = restBlock[restIndex % 8];
  }
}



void loadRest() {
  restaurant restVar;

  for (uint16_t i=0; i<NUM_RESTAURANTS; i++) {
    getRestaurant(i, &restVar);

    rest_dist[i].index = i;
    rest_dist[i].dist = abs(cursorX + yegX - lon_to_x(restVar.lon)) + abs(cursorY + yegY - lat_to_y(restVar.lat));
  }
}



void sortRest(RestDist* restPtr, uint16_t length) {
  uint16_t j;
  uint16_t i = 1;
  uint16_t tempDist;
  uint16_t tempIndex;

  while (i < length) {
    j = i;

    while (j > 0 and restPtr[j-1].dist > restPtr[j].dist) {
      tempDist = restPtr[j].dist;
      tempIndex = restPtr[j].index;

      restPtr[j].dist = restPtr[j-1].dist;
      restPtr[j].index = restPtr[j-1].index;

      restPtr[j-1].dist = tempDist;
      restPtr[j-1].index = tempIndex;

      j--;
    }

    i++;
  }
}



//Redraws the cursor in a given colour
void redrawCursor(uint16_t colour) {
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
               CURSOR_SIZE, CURSOR_SIZE, colour);
}



//Redraws the patch under the old position of the cursor when it is moved.
void drawPatch(uint16_t cursorX, uint16_t cursorY) {
  lcd_image_draw(&yegImage, &tft, yegX + cursorX-CURSOR_SIZE/2, yegY + cursorY-CURSOR_SIZE/2,
                  cursorX-CURSOR_SIZE/2, cursorY-CURSOR_SIZE/2, CURSOR_SIZE, CURSOR_SIZE);
}



//Calculates the speed of the cursor based on how far the joystick is moved. 3 and 8 were
//chosen arbitrarily so that the difference in speed is visible between the 2 modes.
uint8_t calcSpeed(uint16_t joyVAL) {
  if(joyVAL <= 100 or joyVAL >= 940) {
    return 20;
  }
  else {
    return 1;
  }
}



void cityDraw() {
    lcd_image_draw(&yegImage, &tft, yegX, yegY,
                 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
}



void loadChunk() {
    bool load = false;

    if(cursorX == CURSOR_SIZE/2 and yegX != 0){
        yegX -= DISPLAY_WIDTH-60;
        cursorX = DISPLAY_WIDTH - 90;
        load = true;
    }
    if(cursorX == DISPLAY_WIDTH-61-CURSOR_SIZE/2 and yegX != YEG_SIZE-DISPLAY_WIDTH){
        yegX += DISPLAY_WIDTH-60;
        cursorX = 90;
        load = true;
    }
    if(cursorY == CURSOR_SIZE/2 and yegY != 0){
        yegY -= DISPLAY_HEIGHT;
        cursorY = DISPLAY_HEIGHT - 90;
        load = true;
    }
    if(cursorY == DISPLAY_HEIGHT-CURSOR_SIZE/2 and yegY != YEG_SIZE - DISPLAY_HEIGHT){
        yegY += DISPLAY_HEIGHT;
        cursorY = 90;
        load = true;
    }

    yegX = constrain(yegX, 0, YEG_SIZE - DISPLAY_WIDTH-60);
    yegY = constrain(yegY, 0, YEG_SIZE - DISPLAY_HEIGHT);

    if (load) {
        cityDraw();
    }
}



void exitSelection(int32_t lon, int32_t lat){
	int32_t restX = lon_to_x(lon);
	int32_t restY = lat_to_y(lat);

    if (restX < DISPLAY_WIDTH/2) {
    	cursorX = restX;
    	yegX = 0;
    }
    else if (restX > YEG_SIZE - (DISPLAY_WIDTH-60)) {
    	cursorX = DISPLAY_WIDTH+60 - (YEG_SIZE - restX);
    	yegX = YEG_SIZE - DISPLAY_WIDTH-60;
    }
    else {
    	cursorX = (DISPLAY_WIDTH-60)/2;
    	yegX = restX - (DISPLAY_WIDTH-60)/2;
    }

    if (restY < DISPLAY_HEIGHT/2) {
    	cursorY = restY;
    	yegY = 0;
    }
    else if (restY > YEG_SIZE - (DISPLAY_HEIGHT)) {
    	cursorY = DISPLAY_HEIGHT - (YEG_SIZE - restY);
    	yegY = restY - DISPLAY_HEIGHT;
    }
    else {
    	cursorY = DISPLAY_HEIGHT/2;
    	yegY = restY - DISPLAY_HEIGHT/2;
    }
    
    yegX = constrain(yegX, 0, YEG_SIZE - DISPLAY_WIDTH-60);
    yegY = constrain(yegY, 0, YEG_SIZE - DISPLAY_HEIGHT);
    cityDraw();
}



void drawRest(){
	restaurant restVar;
    loadRest();

    for(uint16_t i=0; i<NUM_RESTAURANTS; i++) {
    	int32_t circleX = lon_to_x(restVar.lon);
    	int32_t circleY = lat_to_y(restVar.lat);
		getRestaurant(rest_dist[i].index, &restVar);

		if(circleX >= yegX && circleX <= yegX + DISPLAY_WIDTH - 60) {
            if(circleY >= yegY && circleY <= yegY + DISPLAY_HEIGHT) {
    			tft.fillCircle(circleX - yegX, circleY - yegY, 3, 420);
			}
		}
    }
}




void changeSel(int8_t selectedRest) {
  tft.setCursor(0, 0); //  where  the  characters  will be  displayed

  for (int16_t i = 0; i < 21; i++) {
    restaurant r;
    getRestaurant(rest_dist[i].index , &r);

    if (i !=  selectedRest) { // not  highlighted
      //  white  characters  on  black  background
      tft.setTextColor (0xFFFF , 0x0000);
    } 
    else { //  highlighted
      //  black  characters  on  white  background
      tft.setTextColor (0x0000 , 0xFFFF);
    }

    tft.print(r.name);
    tft.print("\n");
  }

  tft.print("\n");
}



void restarauntSelect() {
  tft.fillScreen(0);
  tft.setTextWrap(false);

  loadRest();
  sortRest(rest_dist, NUM_RESTAURANTS);

  changeSel(0);

  bool selection = false;
  int8_t restSelected = 0;
  while (selection == false) {
    uint16_t yVal = analogRead(JOY_VERT);

    if (yVal <= 450) {
      restSelected--;
      restSelected = constrain(restSelected, 0, 20);
      changeSel(restSelected);
    }
    else if (yVal >= 550) {
      restSelected++;
      restSelected = constrain(restSelected, 0, 20);
      changeSel(restSelected);
    }

    if (digitalRead(JOY_SEL) == LOW) {
      selection = true;
      delay(500);
    }
  }

  restaurant restVar;
  getRestaurant(rest_dist[restSelected].index, &restVar);
  exitSelection(restVar.lon, restVar.lat);
}



void processJoystick() {
  uint16_t xVal = analogRead(JOY_HORIZ);
  uint16_t yVal = analogRead(JOY_VERT);
  uint8_t xSpeed = calcSpeed(xVal);
  uint8_t ySpeed = calcSpeed(yVal);
  int oldX = cursorX;
  int oldY = cursorY;
  bool pressed = false;

  TSPoint touch = ts.getPoint();
  pinMode(YP, OUTPUT);
  pinMode(XM, OUTPUT);

  if (digitalRead(JOY_SEL) == LOW) {
  	restarauntSelect();
  }

  if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
  	drawRest();
  }
 
  if (yVal < JOY_CENTER - JOY_DEADZONE) {
    cursorY -= ySpeed; // decrease the y coordinate of the cursor based on calcSpeed
    pressed = true;
  }
  else if (yVal > JOY_CENTER + JOY_DEADZONE) {
    cursorY += ySpeed;
    pressed = true;
  }

  // remember the x-reading increases as we push left
  if (xVal > JOY_CENTER + JOY_DEADZONE) {
    cursorX -= xSpeed;
    pressed = true;
  }
  else if (xVal < JOY_CENTER - JOY_DEADZONE) {
    cursorX += xSpeed;
    pressed = true;
  }

  //61 is used here, since the cursors width is 9 and this is not integer divisible by 2.
  //leaving it at 60 resulted in a weird visual bug.
  cursorX = constrain(cursorX, CURSOR_SIZE/2, DISPLAY_WIDTH-61-CURSOR_SIZE/2);
  cursorY = constrain(cursorY, CURSOR_SIZE/2, DISPLAY_HEIGHT-CURSOR_SIZE/2);

  //Only redraw if the joystick has been moved.
  //Redraws the small patch of edmonton where the cursor used to be.
  if(pressed) {
    drawPatch(oldX, oldY);
  }

  // drawing a red rectangle at the new cursor position
  redrawCursor(TFT_RED);

  delay(20);
}



int main() {
  setup();

  while (true) {
    processJoystick();
    loadChunk();
  }

  Serial.end();
  return 0;
}