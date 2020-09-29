/*
CMPUT 275 EB1
Names: Andrew Culberson, Logan Vaughnan
ID #s: 1571644         , 1577489
Final Project: Build Your Own Tower Defense (BYOTD)
*/


#include "serial_handling.h"


extern shared_vars shared;




int updateGrid() {
  /*
    This function handles the serial communication to the server
    to tell the server where walls have been placed. The server uses this
    information to calculate a path. It also tells the server the start
    and end vertices.

    The protocol here is very similar to assignment 2 part 2, but in the opposite direction
    as the server requires information from the client.
  */


  // initializing timeout and varibles
  Serial.setTimeout(25);
  int currentTime;
  int startTime;
  bool wall;
  String input = "";


  // request being sent
  Serial.print("X ");
  Serial.print(shared.startVertex.x);
  Serial.print(" ");
  Serial.print(shared.startVertex.y);
  Serial.print(" ");
  Serial.print(shared.endVertex.x);
  Serial.print(" ");
  Serial.println(shared.endVertex.y);


  // Wait for response. 1 second timeout.
  startTime = millis();
  while (Serial.available()<1) {
    currentTime = millis();

    if (currentTime - startTime > 1000) {
      Serial.flush();
      return 0;
    }
  }


  // read server response and make sure it's 'A'
  input = Serial.readStringUntil('\n');
  if (input[0] != 'A') {
    Serial.flush();
    return 0;
  }
  Serial.flush();


  // sends the server the walls in the game
  for (int i=0;i<48;i++) {
    for (int j=0;j<26;j++) {
      wall = false;

      // send wall
      if(shared.grid[i][j] == W or shared.grid[i][j] == T or shared.grid[i][j] == TU or shared.grid[i][j] == TUU) {
        Serial.print('W');
        Serial.print(' ');
        Serial.print(i);
        Serial.print(' ');
        Serial.println(j);
        wall = true;
      }

      if (wall) {
      	// Wait for response. 1 second timeout.
      	startTime = millis();
        while (Serial.available()<1) {
          currentTime = millis();

          if (currentTime - startTime > 1000) {
            Serial.flush();
            return 0;
          }
        }

        // read server response and make sure it's 'A'
        input = Serial.readStringUntil('\n');
        if (input[0] != 'A') {
          Serial.flush();
          return 0;
        }
        Serial.flush();
      }
    }
  }


  // sends end message
  Serial.println('E');
  Serial.flush();
  return 1;
}




int get_waypoints(int startx, int starty, int endx, int endy) {
  /*
    This function grabs the waypoints for the path calculated by the server using serial
    communcation. Uses a protocol similar to assignment 2 part 2.
  */


  // initializing timeout and varibles
  Serial.setTimeout(25);
  int currentTime;
  int startTime;
  String numPoints = "";
  String endMssg = "";
  String waypoint = "";


  // Wait for server to calculate and send path. 10 second timeout.
  startTime = millis();
  while (Serial.available()<1) {
    currentTime = millis();

    if (currentTime - startTime > 10000) {
      Serial.flush();
      return 0;
    }
  }


  // read the server message, make sure it's correct, and give response
  numPoints = Serial.readStringUntil('\n');
  if (numPoints[0] != 'N') {
    Serial.flush();
    return 0;
  }
  Serial.flush();
  Serial.println("A");


  // loop is the number of waypoints the server will send
  int loop = (numPoints.substring(1, numPoints.length())).toInt();


  // if there are too many waypoints
  if(loop > 2000){
    Serial.flush();
    return 1;
  }


  // change value of shared variable
  shared.num_waypoints = loop;


  // read waypoints
  for (int i=0; i<loop; i++) {
  	// Wait for server to send waypoint. 1 second timeout.
    startTime = millis();
    while (Serial.available()<1) {
      currentTime = millis();

      if (currentTime - startTime > 1000) {;
        Serial.flush();
        return 0;
      }
    }

    // read waypoint
    waypoint = Serial.readStringUntil('\n');

    // change shared variable accordingly
    if (waypoint[0] == 'W') {
      int firstSpace = 1;
      int secondSpace = waypoint.indexOf(' ', firstSpace+1);

      long long xi = (waypoint.substring(firstSpace+1, secondSpace)).toInt();
      long long yi = (waypoint.substring(secondSpace+1)).toInt();

      shared.waypoints[i].x = xi;
      shared.waypoints[i].y = yi;
    }
    else {
      Serial.flush();
      return 0;
    }
    Serial.flush();

    // send response
    Serial.println("A");
  }


  // Wait for server to send end message. 1 second timeout.
  startTime = millis();
  while (Serial.available()<1) {
    currentTime = millis();

    if (currentTime - startTime > 1000) {
      Serial.flush();
      return 0;
    }
  }


  // read and check end message
  endMssg = Serial.readStringUntil('\n');
  if (endMssg[0] == 'E') {
    Serial.flush();
    return 1;
  }
  else {
    Serial.flush();
    return 0;
  }
}
