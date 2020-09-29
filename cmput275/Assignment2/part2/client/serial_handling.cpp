#include "serial_handling.h"

extern shared_vars shared;


uint8_t get_waypoints(const lon_lat_32& start, const lon_lat_32& end) {
  // Currently this does not communicate over the serial port.
  // It just stores a path length of 0. You should make it communicate with
  // the server over Serial using the protocol in the assignment description.

  // TODO: implement the communication protocol from the assignment

  // for now, nothing is stored
  //shared.num_waypoints = 0;

  // 1 indicates a successful exchange, of course you should only output 1
  // in your final solution if the exchange was indeed successful
  // (otherwise, return 0 if the communication failed)
  //return 1;


  // loop variable
  int loop;


  // sets a timeout for arduino reading functions
  Serial.setTimeout(50);


  // sends request to server
  Serial.print("R ");
  Serial.print(start.lat);
  Serial.print(" ");
  Serial.print(start.lon);
  Serial.print(" ");
  Serial.print(end.lat);
  Serial.print(" ");
  Serial.print(end.lon);
  Serial.print("\n");


  // wait for response from server. Waits 10 seconds then returns 0
  int currentTime;
  int startTime = millis();

  while (Serial.available()<1) {
    currentTime = millis();

    if (currentTime - startTime > 10000) {
      Serial.flush();
      return 0;
    }
  }


  // reads response
  String ackn = Serial.readStringUntil('\n');


  // makes sure it reads number of waypoints. Finds number of waypoints and returns 1 if over 500
  if (ackn[0] == 'N') {
    loop = (ackn.substring(1, ackn.length())).toInt();

    if(loop > 500){
      Serial.flush();
      return 1;
    }

    shared.num_waypoints = loop;
  }
  else {
    Serial.flush();
    return 0;
  }


  // acknowleges
  Serial.println("A");


  // loop for every waypoint
  for (int i=0; i<loop; i++) {
  	// wait for waypoint to be sent from server. Waits 1 second then returns 0
    startTime = millis();

    while (Serial.available()<1) {
      currentTime = millis();

      if (currentTime - startTime > 1000) {;
        Serial.flush();
        return 0;
      }
    }

    // reads waypoint
    String waypoint = Serial.readStringUntil('\n');

    // makes sure it reads a waypoint. Adds waypoint to waypoints
    if (waypoint[0] == 'W') {
      int firstSpace = 1;
      int secondSpace = waypoint.indexOf(' ', firstSpace+1);

      long long lati = (waypoint.substring(firstSpace+1, secondSpace)).toInt();
      long long longi = (waypoint.substring(secondSpace+1)).toInt();

      shared.waypoints[i].lat = lati;
      shared.waypoints[i].lon = longi;
    }
    else {
      Serial.flush();
      return 0;
    }

    // acknowleges
    Serial.println("A");
  }


  // waits for end character from server. Waits 1 second then returns 0
  startTime = millis();

  while (Serial.available()<1) {
    currentTime = millis();

    if (currentTime - startTime > 1000) {
      Serial.flush();
      return 0;
    }
  }


  // reads end character
  String endAckn = Serial.readStringUntil('\n');


  // makes sure it read end character and returns 1 if so
  if (endAckn[0] == 'E') {
    Serial.flush();
    return 1;
  }
  else {
    Serial.flush();
    return 0;
  }
}
