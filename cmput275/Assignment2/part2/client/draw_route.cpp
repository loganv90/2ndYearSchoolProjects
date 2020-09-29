#include "draw_route.h"
#include "map_drawing.h"

extern shared_vars shared;


void draw_route() {
  for (int i=0; i<shared.num_waypoints-1; i++) {
  	// grabs i point in path
  	lon_lat_32 firstPoint;

    // convert coords to pixels
  	firstPoint.lat = latitude_to_y(shared.map_number,shared.waypoints[i].lat) - shared.map_coords.y;
  	firstPoint.lon = longitude_to_x(shared.map_number,shared.waypoints[i].lon) - shared.map_coords.x;

  	//grabs i+1 point in path
  	lon_lat_32 secondPoint;

    // convert coords to pixels
  	secondPoint.lat = latitude_to_y(shared.map_number,shared.waypoints[i+1].lat) - shared.map_coords.y;
  	secondPoint.lon = longitude_to_x(shared.map_number,shared.waypoints[i+1].lon) - shared.map_coords.x;

    // draw a line from the first point to second point
  	shared.tft->drawLine(firstPoint.lon,firstPoint.lat,secondPoint.lon,secondPoint.lat,0x000);
  }
}
