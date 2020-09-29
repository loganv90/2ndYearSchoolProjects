/*
CMPUT 275 EB1
Names: Andrew Culberson, Logan Vaughnan
ID #s: 1571644         , 1577489
Final Project: Build Your Own Tower Defense (BYOTD)
*/


#include <Arduino.h>
#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include "serial_handling.h"
#include "consts.h"

#define SD_CS 10

// touch screen pins, obtained from the documentaion
#define YP A3 // must be an analog pin, use "An" notation!
#define XM A2 // must be an analog pin, use "An" notation!
#define YM 9  // can be a digital pin
#define XP 8  // can be a digital pin

// width/height of the display when rotated horizontally
#define TFT_WIDTH  480
#define TFT_HEIGHT 320

// calibration data for the touch screen, obtained from documentation
// the minimum/maximum possible readings from the touch point
#define TS_MINX 90 //100
#define TS_MINY 120 //110
#define TS_MAXX 960 //960
#define TS_MAXY 910 //910

// thresholds to determine if there was a touch
#define MINPRESSURE   10
#define MAXPRESSURE 1000

MCUFRIEND_kbv tft;

// a multimeter reading says there are 300 ohms of resistance across the plate,
// so initialize with this to get more accurate readings
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

shared_vars shared;

// current number of towers on screen
int8_t num_towers = 0;
// money used to buy items in game
int8_t balance = 20;
// determines what player is building. 0: nothing, 1: place walls, 2: place towers, 3: upgrade towers
int8_t actionType = 0;
// current round
int8_t roundNumber = 1;
// global variable that initializes defense stage
bool defense;
// global variable that ends the game
bool gameEnd;




void setup() {
    init();

    Serial.begin(9600);

    uint16_t ID = tft.readID();
    tft.begin(ID);

    tft.setRotation(1);
    tft.setTextSize(2);
}




// draws the go button
void drawGo() {
    tft.fillRect(10,270,40,40,0);
    tft.setCursor(20,280);
    tft.println("Go");
}

// draws the wall button
void drawWall() {
    tft.fillRect(80,270,80,40,0);
    tft.setCursor(90,280);
    tft.println("Wall");
}

// draws the tower button
void drawTower() {
    tft.fillRect(170,270,80,40,0);
    tft.setCursor(180,280);
    tft.println("Tower");
}

// draws the upgrade button
void drawUpgrade() {
    tft.fillRect(260,270,140,40,0);
    tft.setCursor(270,280);
    tft.println("Upgrade");
}

// redraws and updates balance for the user. Max balance is 99
void updateMoney() {
    if (balance > 99) {
        balance = 99;
    }

    tft.fillRect(420,270,50,40,0);
    tft.setCursor(430,280);
    tft.print("$");
    tft.println(balance);
}

//uses the action type to update button GUI
void buttonChange() {
    if (actionType != 1) {
        drawWall();
    }
    if (actionType != 2) {
        drawTower();
    }
    if (actionType != 3) {
        drawUpgrade();
    }
}




// evaluates if the player can afford a given item
bool buy(int8_t price) {
    if (price > balance) {
        return false;
    }
    else{
        return true;
    }
}

// Allows player to buy and sell walls. When a wall is bought, the grid variable is changed accordingly.
void wallChange(int x, int y) {
    objects currObject = shared.grid[x][y];

    if (currObject == N && buy(1)) {
        shared.grid[x][y] = W;
        tft.fillRect(x*10,y*10,10,10,TFT_BLACK);
        balance--;
    }
    else if (currObject == W) {
        shared.grid[x][y] = N;
        tft.fillRect(x*10,y*10,10,10,0xFFF);
        balance++;
    }

    updateMoney();
}

// Allows player to buy, sell, and upgrade towers. When a tower is bought, sold, or upgraded, the grid variable is changed accordingly.
// Towers can be upgraded twice, increasing their damage by 1 each time. The first upgrade costs $3 and the second upgrade costs $4.
void towerChange(int x, int y) {
    objects currObject = shared.grid[x][y];

    if (currObject == N && buy(5) && actionType == 2) {
        if (num_towers < 20) {
            shared.grid[x][y] = T;
            tft.fillRect(x*10,y*10,10,10,0xF0F);
            balance -= 5;
            num_towers++;
        }
        else {
            tft.fillRect(10,270,400,40,0);
            tft.setCursor(20,280);
            tft.print("TOWER LIMIT REACHED");
            delay(1000);

            tft.fillRect(0,260,410,60,0xABF);
            drawGo();
            drawWall();
            drawTower();
            drawUpgrade();
        }
    }
    else if (currObject == T && buy(3) && actionType == 3) {
        shared.grid[x][y] = TU;
        tft.fillRect(x*10,y*10,10,10,0xF81F);
        balance -= 3;
    }
    else if (currObject == TU && buy(4) && actionType == 3) {
        shared.grid[x][y] = TUU;
        tft.fillRect(x*10,y*10,10,10,0xFFE0);
        balance -= 4;
    }
    else if ((currObject == T || currObject == TU || currObject == TUU) && actionType == 2) {
        shared.grid[x][y] = N;
        tft.fillRect(x*10,y*10,10,10,0xFFF);
        balance += 5;
        num_towers--;
    }

    updateMoney();
}




// handles all of the input from the user on the touchscreen
void processTouchScreen() {
    TSPoint touch = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE) {
        return;
    }

    int8_t prevX = -1;
    int8_t prevY = -1;
    int16_t screen_x = map(touch.y, TS_MINX, TS_MAXX, TFT_WIDTH-1, 0);
    int16_t screen_y = map(touch.x, TS_MINY, TS_MAXY, TFT_HEIGHT-1, 0);
    int8_t x = screen_x/10;
    int8_t y = screen_y/10;

    if (screen_y < 260) {
        // makes it possible to place a line of walls or towers by dragging (must not drag too fast)
        while (touch.z > MINPRESSURE and screen_y < 260) {
            if (prevX != x or prevY != y) {
                if (actionType == 1) {
                    wallChange(x,y);
                }
                else if (actionType == 2 or actionType == 3) {
                    towerChange(x,y);
                }
            }

            touch = ts.getPoint();
            pinMode(YP, OUTPUT);
            pinMode(XM, OUTPUT);

            if (touch.z < MINPRESSURE || touch.z > MAXPRESSURE) {
                return;
            }

            prevX = x;
            prevY = y;
            screen_x = map(touch.y, TS_MINX, TS_MAXX, TFT_WIDTH-1, 0);
            screen_y = map(touch.x, TS_MINY, TS_MAXY, TFT_HEIGHT-1, 0);
            x = screen_x/10;
            y = screen_y/10;

            delay(20);
        }
    }

    // go button functionality
    else if (screen_x < 60) {
        defense = true;
        actionType = 0;
        buttonChange();
        tft.fillRect(0,260,60,60,0xABF);
    }

    // wall button functionality
    else if (screen_x < 170) {
        actionType = 1;
        buttonChange();
        tft.fillRect(80,270,80,40,0x0F0);
        tft.setCursor(90,280);
        tft.println("$1");
    }

    // tower button functionality
    else if (screen_x < 260) {
        actionType = 2;
        buttonChange();
        tft.fillRect(170,270,80,40,0x0F0);
        tft.setCursor(180,280);
        tft.println("$5");
    }

    // upgrade button functionality
    else if (screen_x < 420) {
        actionType = 3;
        buttonChange();
        tft.fillRect(260,270,140,40,0x5550);
        tft.setCursor(270,280);
        tft.println("1:$3, 2:$4");
    }

    delay(100);
}




// handles all of the actual gameplay during rounds
void watchMode() {
    // grab waypoints from the server 
    while (get_waypoints(0,0,30,10) == 0) {};

    // if there is no path from the spawn to the base, notify the user and let them change their design
    if (shared.num_waypoints == 0) {
        tft.fillRect(10,270,400,40,0);
        tft.setCursor(20,280);
        tft.print("NO PATH FROM ENEMY SPAWN TO BASE");
        delay(3000);

        tft.fillRect(0,260,410,60,0xABF);
        drawGo();
        drawWall();
        drawTower();
        drawUpgrade();

        return;
    }

    // arrays that hold all of the enemies and towers
    Enemy enemies[10];
    Tower towers[20];

    // fill in array of towers
    int8_t towerCount = 0;
    for (int8_t y=0; y<26; y++) {
        for (int8_t x=0; x<48; x++) {
            // T is a tower, TU is a tower with 1 upgrade, TUU is a tower with 2 upgrades
            if (shared.grid[x][y] == T) {
                Tower temp = {1, 4, {x,y}};
                towers[towerCount] = temp;
                towerCount++;
            }
            else if (shared.grid[x][y] == TU) {
                Tower temp = {2, 4, {x,y}};
                towers[towerCount] = temp;
                towerCount++;   
            }
            else if (shared.grid[x][y] == TUU) {
                Tower temp = {3, 4, {x,y}};
                towers[towerCount] = temp;
                towerCount++;
            }
        }
    }

    // fill in array of enemies
    for (int8_t i=0; i<10; i++) {
        Enemy temp = {roundNumber*roundNumber, 0, shared.waypoints[0]};
        enemies[i] = temp;
    }


    // gameplay for one round
    int8_t enemiesSpawned = 0;
    int8_t enemiesKilled = 0;
    gameEnd = false;
    while(!gameEnd) {
        // spawns enemies
        if (enemiesSpawned < 10) {
            enemiesSpawned++;
        }

        // moves and damages spawned enemies
        for (int8_t i=0; i<enemiesSpawned; i++) {
            // if enemy is alive (health is greater than 0) do the following
            if (enemies[i].health > 0) {
                // update enemy position
                Point oldCoord = enemies[i].coord;
                enemies[i].index++;
                enemies[i].coord = shared.waypoints[enemies[i].index];

                // update enemy position visually
                tft.fillRect(oldCoord.x*10, oldCoord.y*10, 10, 10, 0xFFF);
                tft.fillRect(enemies[i].coord.x*10, enemies[i].coord.y*10, 10, 10, 0x0BB);
                tft.fillRect(shared.waypoints[0].x*10, shared.waypoints[0].y*10, 10, 10, 0xFFFF);

                // if enemy has reached the base the game is over
                if (enemies[i].coord.x == shared.endVertex.x and enemies[i].coord.y == shared.endVertex.y) {
                    gameEnd = true;
                }

                // damages enemies within range of towers
                for (int8_t j=0; j<num_towers; j++) {
                    int distance = abs(enemies[i].coord.x - towers[j].coord.x) + abs(enemies[i].coord.y - towers[j].coord.y);

                    if (distance <= towers[j].range && enemies[i].health != -50) {
                        enemies[i].health -= towers[j].damage;
                        tft.fillRect(enemies[i].coord.x*10, enemies[i].coord.y*10, 10, 10, 0xF800);

                        // if enemy is now dead (health is less than 0) do the following
                        if (enemies[i].health <= 0) {
                            enemies[i].health = -50;
                            enemiesKilled++;

                            balance++;
                            updateMoney();
                        }
                    }
                }
            }
        }

        // if all of the enemies are dead, end the round
        if (enemiesKilled == 10) {
            roundNumber++;
            break;
        }

        delay(200);
    }

    // clean path
    for (int8_t i=0; i<10; i++) {
        tft.fillRect(enemies[i].coord.x*10, enemies[i].coord.y*10, 10, 10, 0xFFF);
    }
    tft.fillRect(shared.waypoints[0].x*10, shared.waypoints[0].y*10, 10, 10, 0xFFFF);
    tft.fillRect(shared.waypoints[shared.num_waypoints-1].x*10, shared.waypoints[shared.num_waypoints-1].y*10, 10, 10, 0x07E0);
}




// allows player to choose where their base and the enemy spawner are for the duration of the game
void chooseBase(){
    tft.fillRect(0,0,480,260,0xFFF);
    tft.fillRect(0,260,480,60,0xABF);


    //notify user to place the base
    tft.fillRect(10,270,460,40,0);
    tft.setCursor(20,280);
    tft.print("CHOOSE BASE LOCATION");

    int16_t screen_x = 260;
    int16_t screen_y = 260;
    int8_t x;
    int8_t y;

    TSPoint touch = ts.getPoint();
    pinMode(YP, OUTPUT);
    pinMode(XM, OUTPUT);

    //make sure the base is placed in a valid spot
    while (screen_y > 260 || touch.z < MINPRESSURE || touch.z > MAXPRESSURE) {
        touch = ts.getPoint();
        pinMode(YP, OUTPUT);
        pinMode(XM, OUTPUT);

        screen_x = map(touch.y, TS_MINX, TS_MAXX, TFT_WIDTH-1, 0);
        screen_y = map(touch.x, TS_MINY, TS_MAXY, TFT_HEIGHT-1, 0);
        x = screen_x/10;
        y = screen_y/10;
    }

    shared.grid[x][y] = B;
    tft.fillRect(x*10,y*10,10,10,0x07E0);

    shared.endVertex.x = x;
    shared.endVertex.y = y;

    delay(200);


    //notify the user to place the enemy spawn
    tft.fillRect(10,270,460,40,0);
    tft.setCursor(20,280);
    tft.print("CHOOSE ENEMY START LOCATION");

    //make sure the enemy spawner is in a valid spot
    while(screen_y > 260 || touch.z < MINPRESSURE || touch.z > MAXPRESSURE || shared.grid[x][y] == B){
        touch = ts.getPoint();
        pinMode(YP, OUTPUT);
        pinMode(XM, OUTPUT);

        screen_x = map(touch.y, TS_MINX, TS_MAXX, TFT_WIDTH-1, 0);
        screen_y = map(touch.x, TS_MINY, TS_MAXY, TFT_HEIGHT-1, 0);
        x = screen_x/10;
        y = screen_y/10;
    }

    shared.grid[x][y] = E;
    tft.fillRect(x*10,y*10,10,10,0xFFFF);

    shared.startVertex.x = x;
    shared.startVertex.y = y;

    delay(200);


    //draw buttons for the main game
    tft.fillRect(0,260,480,60,0xABF);
    drawGo();
    drawWall();
    drawTower();
    drawUpgrade();
    updateMoney();
}




// prints messages and resets game variables to initial values
void resetGame(){
    // notify the user they have lost
    tft.fillRect(10,270,400,40,0);
    tft.setCursor(20,280);
    tft.print("GAME OVER. SURVIVED ");
    tft.print(roundNumber - 1);
    tft.print(" ROUNDS");
    delay(3000);

    for (int8_t i=0; i<48; i++) {
        for (int8_t j=0; j<26; j++) {
            shared.grid[i][j] = N;
        }
    }

    balance = 20;
    updateMoney();

    gameEnd = false;
    roundNumber = 1;
    num_towers = 0;

    chooseBase();
}




int main() {
    //setup the game
    setup();
    chooseBase();

    while (true) {
        defense = false;

        // use touch screen to build and upgrade
        processTouchScreen();

        // if defense is true, gameplay starts
        if (defense) {
            // enemy path is calculated
            while (updateGrid() == 0) {};

            // enemies spawn, towers attack, etc.
            watchMode();

            if (gameEnd) {
                // if the user has lost, reset the game
                resetGame();
            }
            else {
                drawGo();
            }
        }
    }

    Serial.end();
    return 0;
}
