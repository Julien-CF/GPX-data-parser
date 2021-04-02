#include "GPXParser.h"
char * getGPXInfo(char *filename);

Waypoint *createWaypoint(double latitude, double longitude){
  Waypoint * newWaypoint = malloc(sizeof(Waypoint) * 1);

  newWaypoint->name = "\0";
  newWaypoint->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

  newWaypoint->latitude = latitude;
  newWaypoint->longitude = longitude;

  return(newWaypoint);
}

int addRoutetoGpxDoc(char *filename, char *routeName, char *waypoints){
  GPXdoc *newdoc = createGPXdoc(filename);
  char *newwpts = malloc(sizeof(char) * strlen(waypoints) + 100);
  strcpy(newwpts, waypoints);
  Route * newRoute = malloc(sizeof(Route) * 1);
  newRoute->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  newRoute->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
  int index = 0;
  for(int i = 0; i < strlen(waypoints); i++){
    if(waypoints[i] == '|'){
      index++;
    }
  }
  newRoute->name = malloc(sizeof(char ) * strlen(routeName) + 1);
  // printf("%d\n", strlen(routeName));
  newRoute->name = routeName;
  printf("%s\n", newRoute->name);

if(strlen(waypoints) != 0){
  char *wpt = strtok(newwpts, "|");
  char *lon = strtok(wpt, " ");
  char *lat = strtok(NULL, "\0");

  double longitude = atof(lon);
  double latitude = atof(lat);

  Waypoint *newWpt = createWaypoint(longitude, latitude);
  insertBack(newRoute->waypoints, newWpt);

  for(int i = 0; i < index - 1; i++){
    char *wpt2 = malloc(sizeof(char) * 10000);
    wpt2 = strtok(wpt, "|");
    lon = strtok(wpt2, " ");
    lat = strtok(wpt2, "\0");
    // printf("%s %s\n", lon, lat);
    double longitude2 = atof(lon);
    double latitude2 = atof(lat);
    Waypoint *newWpt2 = createWaypoint(longitude2, latitude2);
    insertBack(newRoute->waypoints, newWpt2);
  }
}
  insertBack(newdoc->routes, newRoute);
  // printf("MADE IT\n");
  // printf("%s\n", GPXdocToString(newdoc));
  // printf("MADE IT2\n");


  if(!writeGPXdoc(newdoc, filename)){
    return 0;
  }

  // printf("%d\n", index);
  return 1;
}

int main(int argc, char * argv[]) {

  int check = addRoutetoGpxDoc("../uploads/txt.gpx", "route 1", "12 14|");
}
