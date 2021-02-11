#include "GPXParser.h"
#include "stdio.h"
#include "string.h"
/** Function to create an GPX object based on the contents of an GPX file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the GPX file
**/

GPXData* parseExtraData(xmlNode *data){
  char *content = (char *)data->children->content;
  GPXData *newData = malloc(sizeof(GPXData) + (strlen(content) + 1) * sizeof(char));
  char *name = (char *)data->name;
  strcpy(newData->name, name);
  strcpy(newData->value, content);

  return(newData);
}

Waypoint* parseWaypoint(xmlNode *wpt){
  Waypoint *newWpt = malloc(sizeof(Waypoint));
  newWpt->name = NULL;
  xmlAttr *properties = wpt->properties;
  xmlAttr *curAttr = NULL;
  List *extraDataList = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

  for(curAttr = properties; curAttr != NULL; curAttr = curAttr->next){
    if(strcmp((char *)curAttr->name, "lat") == 0){
      xmlNode *value = curAttr->children;
      double latitude = strtod((char *)value->content, NULL);
      newWpt->latitude = latitude;
    } else if(strcmp((char *)curAttr->name, "lon") == 0){
      xmlNode *value = curAttr->children;
      double longitude = strtod((char *)value->content, NULL);
      newWpt->longitude = longitude;
    }
  }
  for(xmlNode *child = wpt->children; child != NULL; child = child->next){
    if (child->type == XML_ELEMENT_NODE) {
      if(strcmp((char *)child->name, "name") == 0){
        char *text = (char*)child->children->content;
        newWpt->name = (malloc(sizeof(char) * (strlen(text)) + 1));
        strcpy(newWpt->name, text);
      } else {
        GPXData *newData = parseExtraData(child);
        // printf("%s %s\n", newData->value, newData->name);
        insertBack(extraDataList, newData);
      }
    }
  }
  newWpt->otherData = extraDataList;
  if(newWpt->name == NULL){
    newWpt->name = malloc(sizeof(char) + 1);
    strcpy(newWpt->name, "\0");
  }
  return(newWpt);
}

TrackSegment* parseTrackSeg(xmlNode *trkSeg){
  TrackSegment *newTrkSeg = malloc(sizeof(TrackSegment));

  List *waypointList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  for(xmlNode *trkPt = trkSeg->children; trkPt != NULL; trkPt = trkPt->next){
    if (trkPt->type == XML_ELEMENT_NODE) {
      Waypoint *newWaypoint = parseWaypoint(trkPt);
      // printf("%f\n", newWaypoint->longitude);
      insertBack(waypointList, newWaypoint);
    }
  }
  newTrkSeg->waypoints = waypointList;

  return (newTrkSeg);
}

Track* parseTrack(xmlNode *trk){
  Track *newTrk = malloc(sizeof(Track));
  newTrk->name = NULL;
  List *trkSegList = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
  List *extraDataList = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

  for(xmlNode *child = trk->children; child != NULL; child = child->next){
    if (child->type == XML_ELEMENT_NODE) {
      if(strcmp((char *)child->name, "name") == 0){
        char *text = (char*)child->children->content;
        newTrk->name = (malloc(sizeof(char) * (strlen(text)) + 1));
        strcpy(newTrk->name, text);
      } else if(strcmp((char *)child->name, "trkseg") == 0){
        xmlNode *curNode = child;
        TrackSegment *newTrkSeg = parseTrackSeg(curNode);
        insertBack(trkSegList, newTrkSeg);
      }
      else {
        xmlNode *xtraData = child;
        GPXData *newData = parseExtraData(xtraData);
        // printf("%s %s\n", newData->value, newData->name);
        insertBack(extraDataList, newData);
      }
    }
  }
  newTrk->segments = trkSegList;
  newTrk->otherData = extraDataList;
  if(newTrk->name == NULL){
    newTrk->name = malloc(sizeof(char) + 1);
    strcpy(newTrk->name, "\0");
  }

  return (newTrk);
}

Route* parseRoute(xmlNode *rte){
  Route *newRoute = malloc(sizeof(Route));
  newRoute->name = NULL;
  List *waypointList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  List *extraDataList = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

  for(xmlNode *child = rte->children; child != NULL; child = child->next){
    if (child->type == XML_ELEMENT_NODE) {
      if(strcmp((char *)child->name, "name") == 0){
        char *text = (char*)child->children->content;
        newRoute->name = (malloc(sizeof(char) * (strlen(text)) + 1));
        strcpy(newRoute->name, text);
      } else if(strcmp((char *)child->name, "rtept") == 0){
          xmlNode *rtePt = child;
          Waypoint *newWaypoint = parseWaypoint(rtePt);
          // printf("%f\n", newWaypoint->longitude);
          insertBack(waypointList, newWaypoint);
      }
      else {
        xmlNode *xtraData = child;
        GPXData *newData = parseExtraData(xtraData);
        // printf("%s %s\n", newData->value, newData->name);
        insertBack(extraDataList, newData);
      }
    }
  }
  newRoute->waypoints = waypointList;
  newRoute->otherData = extraDataList;
  if(newRoute->name == NULL){
    newRoute->name = malloc(sizeof(char) + 1);
    strcpy(newRoute->name, "\0");
  }

  return (newRoute);
}


GPXdoc* createGPXdoc(char* fileName){
  GPXdoc *gpxDoc = malloc(sizeof(GPXdoc));
  xmlDoc *doc = NULL;
  xmlNode *rootNode = NULL;
  xmlNode *curNode = NULL;
  List *wptList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  List *trkList = initializeList(&trackToString, &deleteTrack, &compareTracks);
  List *rteList = initializeList(&routeToString, &deleteRoute, &compareRoutes);

  doc = xmlReadFile(fileName, NULL, 0);

  if (doc == NULL) {
    free(gpxDoc);
    return NULL;
  }

  rootNode = xmlDocGetRootElement(doc);

  for (curNode = rootNode; curNode != NULL; curNode = curNode->next) {
    if (curNode->type == XML_ELEMENT_NODE) {
      if(strcmp((char *)curNode->name, "gpx") == 0){
        xmlAttr *properties = curNode->properties;
        xmlAttr *curAttr = NULL;
        for(curAttr = properties; curAttr !=NULL; curAttr = curAttr->next){
          if(strcmp((char *)curAttr->name, "version") == 0){
            xmlNode *value = curAttr->children;
            double version = strtod((char *)value->content, NULL);
            gpxDoc->version = version;
          } else if(strcmp((char *)curAttr->name, "creator") == 0) {
            xmlNode *value = curAttr->children;
            gpxDoc->creator = (malloc(sizeof(char) * (strlen((char *)value->content)) + 1));
            char *creator = (char *)value->content;
            strcpy(gpxDoc->creator, creator);
          } else if(strcmp((char *)curAttr->name, "schemaLocation") == 0){
            // printf("%s\n", (char *)curAttr->ns->href);
            strcpy(gpxDoc->namespace, (char *)curAttr->ns->href);
          }
        }
        for (xmlNode *childNode = curNode->children; childNode != NULL; childNode = childNode->next) {
          if (childNode->type == XML_ELEMENT_NODE) {
            if(strcmp((char *)childNode->name, "wpt") == 0){\
              xmlNode *wpt = childNode;
              Waypoint *newWaypoint = parseWaypoint(wpt);
              // printf("%f %f %s\n", newWaypoint->longitude, newWaypoint->latitude, newWaypoint->name);
              insertBack(wptList, newWaypoint);
            } else if (strcmp((char *)childNode->name, "trk") == 0){
              xmlNode *trk = childNode;
              Track *newTrack = parseTrack(trk);
              // printf("%s\n", newTrack->name);
              insertBack(trkList, newTrack);
            } else if (strcmp((char *)childNode->name, "rte") == 0){
              xmlNode *rte = childNode;
              Route *newRoute = parseRoute(rte);

              insertBack(rteList, newRoute);
            }
          }
        }
      }
    }
  }
  gpxDoc->waypoints = wptList;
  gpxDoc->tracks = trkList;
  gpxDoc->routes = rteList;
  xmlFreeDoc(doc);
  xmlCleanupParser();
  return (gpxDoc);
}

/** Function to create a string representation of an GPX object.
 *@pre GPX object exists, is not null, and is valid
 *@post GPX has not been modified in any way, and a string representing the GPX contents has been created
 *@return a string contaning a humanly readable representation of an GPX object
 *@param obj - a pointer to an GPX struct
**/
char* GPXdocToString(GPXdoc* doc){
  char *str = malloc(sizeof(char) * 10000);

  char *waypoints = toString(doc->waypoints);
  char *routes = toString(doc->routes);
  char *tracks = toString(doc->tracks);

  snprintf(str, 10000, "namespace : %s\nversion : %f\ncreator : %s\n\n WAYPOINTS : %s\n\n ROUTES : %s\n\n TRACKS : %s", doc->namespace, doc->version, doc->creator, waypoints, routes, tracks);

  free(waypoints);
  free(routes);
  free(tracks);
  return str;
}

/** Function to delete doc content and free all the memory.
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object had been freed
 *@return none
 *@param obj - a pointer to an GPX struct
**/

void deleteGPXdoc(GPXdoc* doc){
  free(doc->creator);
  freeList(doc->waypoints);
  freeList(doc->routes);
  freeList(doc->tracks);
  free(doc);
}

/* For the five "get..." functions below, return the count of specified entities from the file.
They all share the same format and only differ in what they have to count.

 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object has not been modified in any way
 *@return the number of entities in the GPXdoc object
 *@param obj - a pointer to an GPXdoc struct
 */


//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc){
  if(doc == NULL){
    return 0;
  }
  int total = getLength(doc->waypoints);
  return (total);
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc){
  int total = getLength(doc->routes);
  return(total);
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc){
  if(doc == NULL){
    return 0;
  }
  int total = getLength(doc->tracks);
  return(total);
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc){
  if(doc == NULL){
    return 0;
  }
  ListIterator trackList = createIterator(doc->tracks);
  void * elem;
  int total = 0;

  while ((elem = nextElement(&trackList)) != NULL){
    total += getLength(((Track *)elem)->segments);
  }
  // printf("%d\n", total);

  return total;
}

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc){
  if(doc == NULL){
    return 0;
  }
  ListIterator trackList = createIterator(doc->tracks);
  ListIterator waypointList = createIterator(doc->waypoints);
  ListIterator routeList = createIterator(doc->routes);
  int total = 0;
  void *elem;

  while ((elem = nextElement(&trackList)) != NULL){
    ListIterator trkSegList = createIterator(((Track *)elem)->segments);
    void * segElem;
    while ((segElem = nextElement(&trkSegList)) != NULL){
      ListIterator trkWptList = createIterator(((TrackSegment *)segElem)->waypoints);
      void * segWptElem;
      while((segWptElem = nextElement(&trkWptList)) != NULL){
        if( strcmp(((Waypoint *)segWptElem)->name, "\0") != 0){
          // printf("%s\n", ((Waypoint *)segWptElem)->name);
          total++;
        }

        total += getLength(((Waypoint *)segWptElem)->otherData);
      }
    }
    if( strcmp(((Track *)elem)->name, "\0") != 0){
      total++;
    }
    total += getLength(((Track *)elem)->otherData);
  }


  while ((elem = nextElement(&waypointList)) != NULL){
    total += getLength(((Waypoint *)elem)->otherData);
    if( strcmp(((Waypoint *)elem)->name, "\0") != 0){
      total++;
    }
  }

  while ((elem = nextElement(&routeList)) != NULL){
    ListIterator rteWptList = createIterator(((Route *)elem)->waypoints);
    void * rteWptElem;
    while((rteWptElem = nextElement(&rteWptList)) != NULL){
      if( strcmp(((Waypoint *)rteWptElem)->name, "\0") != 0){
        // printf("%s\n", ((Waypoint *)segWptElem)->name);
        total++;
      }

      total += getLength(((Waypoint *)rteWptElem)->otherData);
    }
    total += getLength(((Route *)elem)->otherData);
    if( strcmp(((Route *)elem)->name, "\0") != 0){
      total++;
    }
  }

  // printf("%d\n", total);

  return(total);
}

// Function that returns a waypoint with the given name.  If more than one exists, return the first one.
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name){
  if(doc == NULL || name == NULL){
    return NULL;
  }
  ListIterator waypointList = createIterator(doc->waypoints);
  void *elem;
  while ((elem = nextElement(&waypointList)) != NULL){
    if(strcmp(name, ((Waypoint *)elem)->name) == 0){
      // printf("Found\n");
      return (((Waypoint *)elem));
    }
  }
  return(NULL);
}
// Function that returns a track with the given name.  If more than one exists, return the first one.
// Return NULL if the track does not exist
Track* getTrack(const GPXdoc* doc, char* name){
  if(doc == NULL || name == NULL){
    return NULL;
  }
  ListIterator trackList = createIterator(doc->tracks);
  void *elem;
  while ((elem = nextElement(&trackList)) != NULL){
    if(strcmp(name, ((Track *)elem)->name) == 0){
      // printf("Found\n");
      return (((Track *)elem));
    }
  }
  return(NULL);
}
// Function that returns a route with the given name.  If more than one exists, return the first one.
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name){

  if(doc == NULL || name == NULL){
    return NULL;
  }
  ListIterator routeList = createIterator(doc->routes);
  void *elem;
  while ((elem = nextElement(&routeList)) != NULL){
    if(strcmp(name, ((Route *)elem)->name) == 0){
      // printf("Found\n");
      return (((Route *)elem));
    }
  }
  return(NULL);
}


/* ******************************* List helper functions  - MUST be implemented *************************** */

void deleteGpxData( void* data){
  GPXData *newData = (GPXData *)data;
  free(newData);
}

char* gpxDataToString( void* data){
  if(data == NULL){
    return NULL;
  }
  char *str = malloc(sizeof(char) * 10000);
  GPXData *newData = (GPXData *)data;

  snprintf(str, 10000, "name : %s\nvalue : %s", (char *)newData->name, (char *)newData->value);
  // printf("%s %s\n", (char *)newData->name, (char *)newData->value);
  // printf("%s\n", str);
  return str;
}

int compareGpxData(const void *first, const void *second){
  return -1;
}

void deleteWaypoint(void* data){
  Waypoint *newWpt = (Waypoint *)data;
  free(newWpt->name);
  freeList(newWpt->otherData);
  free(newWpt);
}

char* waypointToString( void* data){
  if(data == NULL){
    return NULL;
  }
  char *str = malloc(sizeof(char) * 10000);
  Waypoint *newWpt = (Waypoint *)data;
  // printf("%s\n", (char *)newWpt->otherData);
  char * xtra = (char *)toString(newWpt->otherData);

  snprintf(str, 10000, "name : %s\nLon : %f Lat : %f\nExtra waypoint data: %s\n",(char *)newWpt->name, newWpt->longitude, newWpt->latitude, xtra);
  free(xtra);
  return(str);
}

int compareWaypoints(const void *first, const void *second){
  return -1;
}

void deleteRoute(void* data){
  Route *newRte = (Route *)data;
  free(newRte->name);
  freeList(newRte->waypoints);
  freeList(newRte->otherData);
  free(newRte);
}

char* routeToString(void* data){
  if(data == NULL){
    return NULL;
  }
  char *str = malloc(sizeof(char) * 10000);
  Route *newRte = (Route *)data;

  char *waypoints = toString(newRte->waypoints);
  char *xtra = toString(newRte->otherData);


  snprintf(str, 10000, "name : %s\n\n Waypoints : %s\n\n Extra route Data : %s\n", (char *)newRte->name, waypoints, xtra);

  // printf("%s\n", str);

  free(waypoints);
  free(xtra);
  return str;
}

int compareRoutes(const void *first, const void *second){
  return -1;
}

void deleteTrackSegment(void* data){
  TrackSegment *newTrkSeg = (TrackSegment *)data;
  freeList(newTrkSeg->waypoints);
  free(newTrkSeg);
}

char* trackSegmentToString(void* data){
  if(data == NULL){
    return NULL;
  }
  char *str = malloc(sizeof(char) * 10000);
  TrackSegment *newTrkSeg = (TrackSegment *)data;

  char *waypoints = toString(newTrkSeg->waypoints);

  snprintf(str, 10000, "%s\n", waypoints);

  free(waypoints);
  return str;
}

int compareTrackSegments(const void *first, const void *second){
  return(-1);
}

void deleteTrack(void* data){
  Track *newTrk = (Track *)data;
  free(newTrk->name);
  freeList(newTrk->segments);
  freeList(newTrk->otherData);
  free(newTrk);
}

char* trackToString(void* data){
  if(data == NULL){
    return NULL;
  }

  char *str = malloc(sizeof(char) * 10000);
  Track *newTrk = (Track *)data;

  char * segs = toString(newTrk->segments);
  char * xtra = toString(newTrk->otherData);

  snprintf(str, 10000, "name : %s\n\n Segments : %s\n Extra Track Data : %s\n", (char *)newTrk->name, segs, xtra);
  // printf("%s\n", str);

  free(segs);
  free(xtra);
  return(str);
}

int compareTracks(const void *first, const void *second){
  return -1;
}
