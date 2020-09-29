/*
CMPUT 275 EB1
Names: Andrew Culberson, Logan Vaughnan
ID #s: 1571644         , 1577489
Final Project: Build Your Own Tower Defense (BYOTD)
*/


#ifndef __SERIAL_HANDLING_H
#define __SERIAL_HANDLING_H


#include <Arduino.h>
#include "consts.h"


int updateGrid();


int get_waypoints(int startx,int starty,int endx,int endy);


#endif
