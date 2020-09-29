/*
==========================
ASSIGNMENT 1 PART 1: Restaurant Finder
Andrew Culberson, Logan Vaughn
CMPUT 275 Winter term 2020
==========================
*/

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
// forward declaration for drawing sorting selections
void drawSelect();

// global variables for the states of the sort and rating buttons
int sortButton = 0;
int ratingButton = 0;

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

  tft.setTextSize(2);

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

  // draws the buttons in the right side of the screen
  tft.fillRect(DISPLAY_WIDTH-58,2,56,156,TFT_RED);
  tft.fillRect(DISPLAY_WIDTH-58,162,56,156,0xF00F);
  drawSelect();

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
uint16_t restToSort = 0;



void getRestaurant(uint16_t restIndex, restaurant* restPtr) {
  /*
    Grabs a restaurant from the SD card using the caching method described
    in a previous weekly assignment.
  */
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
  /*
    Loads restaurants distance and index into an array that will be used by other functions.
    This function uses the state of the rating button to filter out restaurants
  */
  restaurant restVar;
  restToSort = 0;

  for (uint16_t i=0; i<NUM_RESTAURANTS; i++) {
    getRestaurant(i, &restVar);
    if(max((restVar.rating+1)/2,1) >= ratingButton + 1){
      rest_dist[restToSort].index = i;
      rest_dist[restToSort].dist = abs(cursorX + yegX - lon_to_x(restVar.lon)) + abs(cursorY + yegY - lat_to_y(restVar.lat));
      restToSort++;
    }
  }
}



void sortRest(RestDist* restPtr, uint16_t length) {
  /*
    This function uses insertion sort to sort an array of restaurants by their distance
    to the cursor.
  */
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


void swap(RestDist* array, int a, int b) {
  // this function swaps elements of the RestDist structure

  RestDist temp = array[a];
  array[a] = array[b];
  array[b] = temp;
}


uint16_t pivot(RestDist* array, int start, int end) {
  // the pivot function as described in the lecture notes. Finds the new pivot.
  // Is part of the quicksort algorithm

  int lo = start;
  int hi = end-1;
 
  while(lo<=hi) {
    if (array[lo].dist <= array[end].dist) {
        lo++;
    }
    else if (array[hi].dist > array[end].dist) {
        hi--;
    }
    else {
        swap(array, lo, hi);
    }
  }
 
  swap(array, lo, end);
 
  return lo;
}


void quickSortRest(RestDist* array, int start, int end){
  // quicksort algorithm for elements of the RestDist structure

  if(end-start+1 <= 1){
    return;
  }

  int pi = pivot(array, start, end);

  quickSortRest(array, start, pi-1);
  quickSortRest(array, pi+1, end);
}



void redrawCursor(uint16_t colour) {
  //Redraws the cursor in a given colour
  tft.fillRect(cursorX - CURSOR_SIZE/2, cursorY - CURSOR_SIZE/2,
               CURSOR_SIZE, CURSOR_SIZE, colour);
}




void drawPatch(uint16_t cursorX, uint16_t cursorY) {
  //Redraws the patch under the old position of the cursor when it is moved.
  lcd_image_draw(&yegImage, &tft, yegX + cursorX-CURSOR_SIZE/2, yegY + cursorY-CURSOR_SIZE/2,
                  cursorX-CURSOR_SIZE/2, cursorY-CURSOR_SIZE/2, CURSOR_SIZE, CURSOR_SIZE);
}



uint8_t calcSpeed(uint16_t joyVAL) {
  /*
    Calculates the speed of the cursor based on how far the joystick is moved. 3 and 8 were
    chosen arbitrarily so that the difference in speed is visible between the 2 modes.
  */
  if(joyVAL <= 100 or joyVAL >= 940) {
    return 50;
  }
  else {
    return 1;
  }
}



void cityDraw() {
    //Updates the image of the city displayed on the tft.
    lcd_image_draw(&yegImage, &tft, yegX, yegY,
                 0, 0, DISPLAY_WIDTH - 60, DISPLAY_HEIGHT);
}

void drawSelect(){
  // draws the letters on the sort and rating buttons

  tft.setTextColor(0xFFFF);
  if (sortButton == 0) {
      tft.setCursor(446, 40);
      tft.print("B");
      tft.setCursor(446, 60);
      tft.print("O");
      tft.setCursor(446, 80);
      tft.print("T");
      tft.setCursor(446, 100);
      tft.print("H");
  }
  else {
      if (sortButton == 1) {
          tft.setCursor(446, 40);
          tft.print("I");
      }
      else if (sortButton == 2) {
          tft.setCursor(446, 40);
          tft.print("Q");
      }
      tft.setCursor(446, 60);
      tft.print("S");
      tft.setCursor(446, 80);
      tft.print("O");
      tft.setCursor(446, 100);
      tft.print("R");
      tft.setCursor(446, 120);
      tft.print("T");
  }

  tft.setCursor(446, 220);
  tft.print(ratingButton + 1);

}



void loadChunk() {
  /*
    Handles the scrolling of the cursor. If the cursor reaches the edge of the screen,
    and it isnt at the boundary of the image, it calls cityDraw() to load the next
    chunk of the city.
  */
    bool load = false;

    if(cursorX == CURSOR_SIZE/2 and yegX != 0){
        yegX -= DISPLAY_WIDTH-60;
        cursorX = DISPLAY_WIDTH - 90;
        load = true;
    }
    if(cursorX == DISPLAY_WIDTH-61-CURSOR_SIZE/2 and yegX != YEG_SIZE-DISPLAY_WIDTH+60){
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

    yegX = constrain(yegX, 0, YEG_SIZE - DISPLAY_WIDTH+60);
    yegY = constrain(yegY, 0, YEG_SIZE - DISPLAY_HEIGHT);

    if (load) {
        cityDraw();
    }
}



void exitSelection(int32_t lon, int32_t lat){
    /*
      Handles the exiting of the restaurant selection screen. Also handles the cases of
      the restaurants being off the screen, or in an area where the map cannot be
      centered on it due to the limited size of the image. In these cases, it attempts
      to center the screen as much as possible, and then places the cursor directly on the
      restaurant.
    */
    int32_t restX = lon_to_x(lon);
    int32_t restY = lat_to_y(lat);

    if (restX < DISPLAY_WIDTH/2) {
        cursorX = restX;
        yegX = 0;
    }
    else if (restX > YEG_SIZE - (DISPLAY_WIDTH-60)) {
        yegX = YEG_SIZE - (DISPLAY_WIDTH-60);
        cursorX = restX-yegX;
    }
    else {
        yegX = restX - (DISPLAY_WIDTH-60)/2;
        cursorX = (DISPLAY_WIDTH-60)/2;
    }

    if (restY < DISPLAY_HEIGHT/2) {
        yegY = 0;
        cursorY = restY;
    }
    else if (restY > YEG_SIZE - (DISPLAY_HEIGHT/2)) {
        yegY = YEG_SIZE - DISPLAY_HEIGHT;
        cursorY = restY-yegY;
    }
    else {
        yegY = restY - DISPLAY_HEIGHT/2;
        cursorY = DISPLAY_HEIGHT/2;
    }
    
    yegX = constrain(yegX, 0, YEG_SIZE - DISPLAY_WIDTH+60);
    yegY = constrain(yegY, 0, YEG_SIZE - DISPLAY_HEIGHT);
    cursorY = constrain(cursorY,0,DISPLAY_HEIGHT);
    cursorX = constrain(cursorX,0,DISPLAY_WIDTH-60);
    cityDraw();
}



void drawRest(){
    /*
      Called when the screen is touched. Draws all of the restaurants as small circles that
      are within the bounds of the screen.
    */
    restaurant restVar;
    loadRest();

    for(uint16_t i=0; i<restToSort; i++) {
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



void changeSel(int8_t selectedRest, int8_t prevRest, int16_t scrollVal) {
  /*
    Highlights the restaurant selected in the selection menu.
  */
  if(selectedRest < 0 or selectedRest > 20){
    return;
  }

  restaurant select;

  //Cover the old selection back up
  tft.setCursor(0,prevRest*15);
  getRestaurant(rest_dist[prevRest+scrollVal].index , &select);
  tft.setTextColor(0xFFFF,0x0000);
  tft.print(select.name);

  //Highlight new selection
  tft.setCursor(0, selectedRest*15); //  where  the  characters  will be  displayed
  getRestaurant(rest_dist[selectedRest+scrollVal].index , &select);
  tft.setTextColor (0x0000 , 0xFFFF);
  tft.print(select.name);

}

void initSel(int16_t scrollVal){
  /*
    Prints the initial menu when you click the joystick.
  */
  tft.setCursor(0, 0); //  where  the  characters  will be  displayed
  uint16_t selCursor = 0; // This gets adjusted to move the cursor to print the next restaurant
  for (int16_t i = 0; i < 21; i++) {
    restaurant r;
    getRestaurant(rest_dist[scrollVal+i].index , &r);
    tft.setCursor(0,selCursor);

    if(max((r.rating+1)/2,1) >= ratingButton + 1){

      if (i !=  0) { // not  highlighted
        //  white  characters  on  black  background
        tft.setTextColor (0xFFFF , 0x0000);
      }
      else { //  highlighted
        //  black  characters  on  white  background
        tft.setTextColor (0x0000 , 0xFFFF);
      }

      tft.print(r.name);
      selCursor += 15;
    }
  }


}



void restarauntSelect() {
  /*
    Handles the restaurant selection screen interaction with the joystick. If the joystick
    is moved it uses helper function changeSel to change the selection.
    Also prints the running time of the selected sorting algorithms in the serial monitor 
    when the joystick is pressed.
  */

  tft.fillScreen(0);
  tft.setTextWrap(false);

  // time of insertion sort
  int insertionTime;

  // time of quicksort
  int quickTime;

  // displacement from the 0th restaurant. Used in scrolling
  uint16_t scrollVal = 0;

  loadRest();

  // selection of the sorting algorithm with the value of sortButton
  if(sortButton == 0){
    insertionTime = millis();
    sortRest(rest_dist, restToSort);
    insertionTime = millis() - insertionTime;

    Serial.print("Insertion Sort running time: ");
    Serial.println(insertionTime);

    loadRest();

    quickTime = millis();
    quickSortRest(rest_dist,0,restToSort-1);
    quickTime = millis() - quickTime;

    Serial.print("Quick Sort running time: ");
    Serial.println(quickTime);
  }
  else if(sortButton == 1){
    insertionTime = millis();
    sortRest(rest_dist, restToSort);
    insertionTime = millis() - insertionTime;

    Serial.print("Insertion Sort running time: ");
    Serial.println(insertionTime);
  }
  else{
    quickTime = millis();
    quickSortRest(rest_dist,0,restToSort-1);
    quickTime = millis() - quickTime;

    Serial.print("Quick Sort running time: ");
    Serial.println(quickTime);
  }

  initSel(0);

  bool selection = false;
  int8_t restSelected = 0;
  uint8_t prevRest = 0;

  // implementation of scrolling in the restaurant menu
  while (selection == false) {
    uint16_t yVal = analogRead(JOY_VERT);

    prevRest = restSelected;

    if (yVal <= 450) {
      restSelected--;
      restSelected = constrain(restSelected,-1,21);
      changeSel(restSelected,prevRest,scrollVal);
    }
    else if (yVal >= 550) {
      restSelected++;
      restSelected = constrain(restSelected,-1,21);
      changeSel(restSelected,prevRest,scrollVal);
    }

    if (digitalRead(JOY_SEL) == LOW) {
      selection = true;
      delay(50);
    }

    if(restSelected > 20 and scrollVal != restToSort-20){
      scrollVal += 21;
      scrollVal = constrain(scrollVal,0,restToSort-20);
      tft.fillRect(0,0,DISPLAY_WIDTH,DISPLAY_HEIGHT,TFT_BLACK);
      initSel(scrollVal);
      restSelected = 0;
    }
    if(restSelected < 0 and scrollVal != 0){
      scrollVal -= 21;
      scrollVal = constrain(scrollVal,0,restToSort-20);
      tft.fillRect(0,0,DISPLAY_WIDTH,DISPLAY_HEIGHT,TFT_BLACK);
      initSel(scrollVal);
      restSelected = 0;
    }
  }

  // redraw of city and buttons after restaurant selection
  restaurant restVar;
  getRestaurant(rest_dist[restSelected+scrollVal].index, &restVar);
  exitSelection(restVar.lon, restVar.lat);
  tft.fillRect(DISPLAY_WIDTH-60,0,60,DISPLAY_HEIGHT,TFT_BLACK);
  tft.fillRect(DISPLAY_WIDTH-58,2,56,156,TFT_RED);
  tft.fillRect(DISPLAY_WIDTH-58,162,56,156,0xF00F);
  drawSelect();
}

void sortSelection(int16_t screen_y){
  //Changes the selection for the rating or for the sorting algorithm
  //based on where on the screen you touched.
  if(screen_y <= DISPLAY_HEIGHT/2){
    sortButton = (sortButton+1)%3;
    tft.fillRect(DISPLAY_WIDTH-58,2,56,156,TFT_RED);
  }
  else{
    ratingButton = (ratingButton+1)%5;
    tft.fillRect(DISPLAY_WIDTH-58,162,56,156,0xF00F);
  }
  drawSelect();
}



void processJoystick() {
  /*
    This function handles all input by the user. This includes the joystick and the
    touch screen. Depending on the user input, this function branches out to different areas
    of the code, such as the functions that handle the restaurant menu.
  */
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
  int16_t screen_x = map(touch.y, TS_MINX, TS_MAXX, DISPLAY_WIDTH-1, 0);
  int16_t screen_y = map(touch.x, TS_MINY,TS_MAXY,DISPLAY_HEIGHT,0);
  //If the joystick is pressed, go to the restaurant selection menu
  if (digitalRead(JOY_SEL) == LOW) {
      restarauntSelect();
  }
  //If the screen is pressed, draw all of the restaurants that will appear within
  //the dimensions of the screen.
  if (touch.z >= MINPRESSURE && touch.z <= MAXPRESSURE) {
    if(screen_x <= DISPLAY_WIDTH - 60){
      drawRest();
    }
    else{
      sortSelection(screen_y);
    }
  }

  //All joystick handling is done here
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
  //leaving it at 60 resulted in a weird visual bug so this had to be changed.
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
  //This while loop runs constantly while the program is running, constantly updating
  //the program with user input, and checking if the screen has to be scrolled.
  while (true) {
    processJoystick();
    loadChunk();
  }

  Serial.end();
  return 0;
}



