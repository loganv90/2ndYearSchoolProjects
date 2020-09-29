/*
CMPUT 275 EB1
Names: Andrew Culberson, Logan Vaughnan
ID #s: 1571644         , 1577489
Final Project: Build Your Own Tower Defense (BYOTD)
*/


#ifndef __CONSTS_H
#define __CONSTS_H

#include <MCUFRIEND_kbv.h>


// point structure for coordinates of points on grid
struct Point {
    int x;
    int y;
};


// used with grid. N: nothing, W: wall, T: tower, TU: tower (1 upgrade), TUU: tower (2 upgrades), E: enemy spawn, B: base
enum objects {N,W,T,TU,TUU,E,B};


struct Tower {
    // damage tower does
    int8_t damage;
    // range of tower
    uint8_t range;
    // tower position
    Point coord;
};


struct Enemy {
    // health enemy has
    int32_t health;
    // index of enemy along path
    uint8_t index;
    // coordinates of enemy
    Point coord;
};


struct shared_vars {
	// point for start vertex (enemy spawn)
	Point startVertex;
	// point for end vertex (base)
	Point endVertex;
	// grid to identify objects on screen
	objects grid[48][26];
	// array for waypoints
    Point waypoints[500];
    // number of waypoints
    int num_waypoints = 0;
};


#endif
