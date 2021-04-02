#include "GPXParser.h"
#include "stdio.h"
#include "string.h"
#include <math.h>
#include <libxml/xmlschemastypes.h>
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

/**
  Name : Julien C-Fournier
  Student ID#: 1083079
  email : jcarpe03@uoguelph.ca
**/

char *tracksInPath(double latitudeStart, double longitudeStart, double latitudeEnd, double longitudeEnd, int delta, char *filename){
  GPXdoc *newdoc = createGPXdoc(filename);
  List *inBetween = getTracksBetween(newdoc, latitudeStart, longitudeStart, latitudeEnd, longitudeEnd, delta);

  char *json = trackListToJSON((const List *)inBetween);

  // printf("%s\n", json);

  return json;
}

char *routesInPath(double latitudeStart, double longitudeStart, double latitudeEnd, double longitudeEnd, int delta, char *filename){
  GPXdoc *newdoc = createGPXdoc(filename);
  List *inBetween = getRoutesBetween(newdoc, latitudeStart, longitudeStart, latitudeEnd, longitudeEnd, delta);

  char *json = routeListToJSON((const List *)inBetween);

  // printf("%s\n", json);

  return json;
}

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

int createNewGpxDoc(char *filename, char *creator){

  GPXdoc *newdoc = malloc(sizeof(GPXdoc) * 1);
  newdoc->creator = (malloc(sizeof(char) * (strlen(creator)) + 1));
  strcpy(newdoc->creator, creator);

  newdoc->version = 1.1;
  strcpy(newdoc->namespace, "http://www.topografix.com/GPX/1/1");

  newdoc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  newdoc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
  newdoc->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);

  if(!validateGPXDoc(newdoc, "./parser/gpx.xsd")){
    return 0;
  }

  if(!writeGPXdoc(newdoc, filename)){
    return 0;
  }
  printf("%s\n", newdoc->namespace);


  return 1;
}

void changeRouteName(GPXdoc * doc, char * oldname, char * newname){
  ListIterator iterator = createIterator(doc->routes);
  void *elem;
  if(strcmp(oldname, "None") == 0){
    strcpy(oldname, "");
  }
  while((elem = nextElement(&iterator)) != NULL){
    // printf("%s compared to %s\n", ((Route *)elem)->name, oldname);
    if(strcmp(((Route *)elem)->name, oldname) == 0){
      printf("Found one\n");
      strcpy(((Route *)elem)->name, newname);
      break;
    }
  }
}

void changeTrackName(GPXdoc * doc, char * oldname, char * newname){
  ListIterator iterator = createIterator(doc->tracks);
  void *elem;
  if(strcmp(oldname, "None") == 0){
    strcpy(oldname, "");
  }
  while((elem = nextElement(&iterator)) != NULL){
    printf("%s compared to %s\n", ((Route *)elem)->name, oldname);
    if(strcmp(((Track *)elem)->name, oldname) == 0){
      printf("Found one\n");
      strcpy(((Track *)elem)->name, newname);
      break;
    }
  }
}

int rewriteFile(char *filename, char *type, char *newName, char *oldname){

  // printf("%s %s %s %s\n", filename, type, newName, oldname);
  GPXdoc *doc = createGPXdoc(filename);

  char *tokens = strtok(type, " ");
  if(strcmp(tokens, "track") == 0){
    changeTrackName(doc, oldname, newName);
  } else if(strcmp(tokens, "route") == 0){
    changeRouteName(doc, oldname, newName);
  }

  if(!validateGPXDoc(doc, "./parser/gpx.xsd")){
    printf("Failes\n");
    return 0;
  }
  if(!writeGPXdoc(doc, filename)){
    printf("Failes\n");
    return 0;
  }
  printf("passed\n");
  return 1;
}

char * getGPXInfo(char *filename){
  char *file;
  GPXdoc *doc = createValidGPXdoc(filename, "./parser/gpx.xsd");
  if(doc == NULL){
    char *temp = malloc(sizeof(char) * 10);
    strcpy(temp, "error");
    return(temp);
  } else {
    file = GPXtoJSON(doc);
  }
  deleteGPXdoc(doc);
  return (file);
}

int numWaypoints(Track *trk){
  int total;
  ListIterator iterator = createIterator(trk->segments);
  void *elem;
  while((elem = nextElement(&iterator)) != NULL){
    total += getLength(((TrackSegment *)elem)->waypoints);
  }
  return total;
}

char *getTrackInfo(char *fileName){

  GPXdoc *doc = createGPXdoc(fileName);

  // ListIterator trackList = createIterator(doc->tracks);
  // void *trkElem;
  //
  // for(int i = 0; i < index; i++){
  //   trkElem = nextElement(&trackList);
  // }
  char *string = trackListToJSON((const List *)doc->tracks);
  printf("%s\n", string);
  return(string);
}

char *getRouteInfo(char *fileName){

    GPXdoc *doc = createGPXdoc(fileName);

    // ListIterator trackList = createIterator(doc->tracks);
    // void *trkElem;
    //
    // for(int i = 0; i < index; i++){
    //   trkElem = nextElement(&trackList);
    // }
    char *string = routeListToJSON((const List *)doc->routes);
    // printf("%s\n", string);
    return(string);
}

#define R 6371000
#define TO_RAD (M_PI / 180)

GPXData* parseExtraData(xmlNode *data){
  GPXData *newData;
  if(data->children == NULL){
     newData = malloc(sizeof(GPXData) + (strlen("") + 1) * sizeof(char));
    strcpy(newData->value, "");
  } else {
    char *content = (char *)data->children->content;
    newData = malloc(sizeof(GPXData) + (strlen(content) + 1) * sizeof(char));
    strcpy(newData->value, content);
  }
  char *name = (char *)data->name;
  strcpy(newData->name, name);

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
      if(strcmp((char *)child->name, "name") == 0 && child->children != NULL){
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
      if(strcmp((char *)child->name, "name") == 0 && child->children != NULL){
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

  doc = xmlReadFile(fileName, NULL, 0);

  if (doc == NULL) {
    free(gpxDoc);
    xmlCleanupParser();
    return NULL;
  }

  List *wptList = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
  List *trkList = initializeList(&trackToString, &deleteTrack, &compareTracks);
  List *rteList = initializeList(&routeToString, &deleteRoute, &compareRoutes);

  rootNode = xmlDocGetRootElement(doc);
  strcpy(gpxDoc->namespace, (char *)rootNode->ns->href);
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
          } //else if(strcmp((char *)curAttr->name, "schemaLocation") == 0){
          //   // printf("%s\n", (char *)curAttr->ns->href);
          //   strcpy(gpxDoc->namespace, (char *)curAttr->ns->href);
          // }
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
  return (gpxDoc);
}

// xmlNodePtr xtrDataToNode(GPXData *elem){
//   xmlNodePtr newNode = NULL;
//   newNode = xmlNewNode()
// }

xmlNodePtr waypointToNode(Waypoint *elem, xmlNsPtr ptr){
  xmlNodePtr newNode = xmlNewNode(ptr, (unsigned char *)"wpt");

  char temp[1000];
  char temp2[1000];

  sprintf(temp, "%f", elem->latitude);
  sprintf(temp2, "%f", elem->longitude);

  xmlNewProp(newNode, BAD_CAST "lat", BAD_CAST temp);
  xmlNewProp(newNode, BAD_CAST "lon", BAD_CAST temp2);
  if(strlen(elem->name) > 0){
    xmlNewChild(newNode, NULL, BAD_CAST "name", BAD_CAST elem->name);
  }

  ListIterator dataList = createIterator(elem->otherData);
  void *dataElem;
  while((dataElem = nextElement(&dataList)) != NULL){
    GPXData *newData = (GPXData *)dataElem;
    xmlNewChild(newNode, NULL, BAD_CAST newData->name, BAD_CAST newData->value);
  }

  return newNode;
}

xmlNodePtr routesToNode(Route *elem, xmlNsPtr ptr){
  xmlNodePtr newNode = xmlNewNode(ptr, (unsigned char *)"rte");
  if(strlen(elem->name) != 0){
    xmlNewChild(newNode, NULL, BAD_CAST "name", BAD_CAST elem->name);
  }

  ListIterator dataList = createIterator(elem->otherData);
  void *dataElem;
  while((dataElem = nextElement(&dataList)) != NULL){
    GPXData *newData = (GPXData *)dataElem;
    xmlNewChild(newNode, NULL, BAD_CAST newData->name, BAD_CAST newData->value);
  }

  ListIterator waypointList = createIterator(elem->waypoints);
  void * wptElem;
  while((wptElem = nextElement(&waypointList)) != NULL){
    xmlNodePtr wptNode = waypointToNode((Waypoint *)wptElem, ptr);
    xmlNodeSetName(wptNode, (unsigned char *)"rtept");
    // wptNode = xmlNewChild(newNode, NULL, BAD_CAST "rtept", NULL);
    xmlAddChild(newNode, wptNode);

  }
  return newNode;
}

xmlNodePtr trkSegToNode(TrackSegment *elem, xmlNsPtr ptr){
  xmlNodePtr newNode = xmlNewNode(ptr, (unsigned char *)"trkseg");

  ListIterator waypointList = createIterator(elem->waypoints);
  void * trkSegElem;
  while((trkSegElem = nextElement(&waypointList)) != NULL){
    xmlNodePtr wptNode = waypointToNode((Waypoint *)trkSegElem, ptr);
    // wptNode = xmlNewChild(newNode, NULL, BAD_CAST "trkpt", NULL);
    xmlNodeSetName(wptNode, (unsigned char *)"trkpt");
    xmlAddChild(newNode, wptNode);
  }
  return(newNode);
}

xmlNodePtr trackToNode(Track *elem, xmlNsPtr ptr){
  xmlNodePtr newNode = xmlNewNode(ptr, (unsigned char *)"trk");

  xmlNewChild(newNode, NULL, BAD_CAST "name", BAD_CAST elem->name);

  ListIterator dataList = createIterator(elem->otherData);
  void *dataElem;
  while((dataElem = nextElement(&dataList)) != NULL){
    GPXData *newData = (GPXData *)dataElem;
    xmlNewChild(newNode, NULL, BAD_CAST newData->name, BAD_CAST newData->value);
  }

  ListIterator segmentList = createIterator(elem->segments);

  while((elem = nextElement(&segmentList)) != NULL){
    xmlNodePtr trkSegNode = trkSegToNode((TrackSegment *) elem, ptr);
    // trkSegNode = xmlNewChild(newNode, NULL, BAD_CAST "trkseg", NULL);
    xmlAddChild(newNode, trkSegNode);
  }
  return newNode;
}

xmlDocPtr createTree(GPXdoc* doc){
  xmlDocPtr docPtr = NULL;
  xmlNodePtr rootNode = NULL;
  xmlNsPtr ptr = NULL;
  char temp[1000];

  docPtr = xmlNewDoc(BAD_CAST "1.0");
  rootNode = xmlNewNode(NULL, BAD_CAST "gpx");
  xmlDocSetRootElement(docPtr, rootNode);

  sprintf(temp, "%0.1f", doc->version);

  xmlNewProp(rootNode, BAD_CAST "version", BAD_CAST temp);
  xmlNewProp(rootNode, BAD_CAST "creator", BAD_CAST doc->creator);
  ptr = xmlNewNs(rootNode, BAD_CAST doc->namespace, NULL);
  xmlSetNs(rootNode, ptr);

  ListIterator waypointList = createIterator(doc->waypoints);
  void * elem;
  while((elem = nextElement(&waypointList)) != NULL){
    xmlNodePtr newNode = waypointToNode((Waypoint *)elem, ptr);
    // printf("%p\n", newNode);
    xmlAddChild(rootNode, newNode);
    // newNode = xmlNewChild(rootNode, NULL, BAD_CAST "wpt", NULL);
  }

  ListIterator routesList = createIterator(doc->routes);
  void * rteElem;
  while((rteElem = nextElement(&routesList)) != NULL){
    xmlNodePtr newNode = routesToNode((Route *)rteElem, ptr);
    xmlAddChild(rootNode, newNode);
    // newNode = xmlNewChild(rootNode, NULL, BAD_CAST "rte", NULL);
  }

  ListIterator trackList = createIterator(doc->tracks);
  void * trkElem;
  while((trkElem = nextElement(&trackList)) != NULL){
    xmlNodePtr newNode = trackToNode((Track *)trkElem, ptr);
    // newNode = xmlNewChild(rootNode, NULL, BAD_CAST "trk", NULL);
    xmlAddChild(rootNode, newNode);
  }

  return docPtr;
}

bool writeGPXdoc(GPXdoc* doc, char* fileName){
  if(doc == NULL){
    return false;
  }
  if(fileName == NULL){
    return false;
  }
  xmlDocPtr docPtr = createTree(doc);


  xmlSaveFormatFile(fileName, docPtr, 1);

  xmlFreeDoc(docPtr);
  xmlCleanupParser();
  xmlMemoryDump();

  return true;
}

int isGpxDocValid(xmlDocPtr doc, xmlSchemaPtr schema){
  int val;
  xmlSchemaValidCtxtPtr ctxt;

  ctxt = xmlSchemaNewValidCtxt(schema);
  // xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
  val = xmlSchemaValidateDoc(ctxt, doc);
  xmlSchemaFreeValidCtxt(ctxt);

  return val;
}

bool manualVerifyXtraData(GPXData *elem){
  if(strlen(elem->name) == 0){
    return false;
  }
  if(strlen(elem->value) == 0){
    return false;
  }
  return true;
}

bool manualVerifyWaypoint(Waypoint *elem){
  if(elem->name == NULL){
    return false;
  }
  if(elem->otherData == NULL){
    return false;
  }
  ListIterator dataList = createIterator(elem->otherData);
  void * dataElem;
  while((dataElem = nextElement(&dataList)) != NULL){
    if(!manualVerifyXtraData((GPXData *)dataElem)){
      return false;
    }
  }
  return true;
}

bool manualVerifyRoute(Route *elem){
  if(elem->name == NULL){
    return false;
  }

  if(elem->waypoints == NULL){
    return false;
  }
  ListIterator waypointList = createIterator(elem->waypoints);
  void * wptElem;
  while((wptElem = nextElement(&waypointList)) != NULL){
    if(!manualVerifyWaypoint((Waypoint *)wptElem)){
      return false;
    }
  }

  if(elem->otherData == NULL){
    return false;
  }
  ListIterator dataList = createIterator(elem->otherData);
  void * dataElem;
  while((dataElem = nextElement(&dataList)) != NULL){
    if(!manualVerifyXtraData((GPXData *)dataElem)){
      return false;
    }
  }

  return true;
}

bool manualVerifyTrkSegment(TrackSegment *elem){
  if(elem->waypoints == NULL){
    return false;
  }
  ListIterator waypointList = createIterator(elem->waypoints);
  void * wptElem;
  while((wptElem = nextElement(&waypointList)) != NULL){
    if(!manualVerifyWaypoint((Waypoint *)wptElem)){
      return false;
    }
  }

  return true;
}

bool manualVerifyTrack(Track *elem){
  if(elem->name == NULL){
    return false;
  }

  if(elem->segments == NULL){
    return false;
  }
  ListIterator segmentList = createIterator(elem->segments);
  void * segElem;
  while((segElem = nextElement(&segmentList)) != NULL){
    if(!manualVerifyTrkSegment((TrackSegment *)segElem)){
      return false;
    }
  }

  if(elem->otherData == NULL){
    return false;
  }
  ListIterator dataList = createIterator(elem->otherData);
  void * dataElem;
  while((dataElem = nextElement(&dataList)) != NULL){
    if(!manualVerifyXtraData((GPXData *)dataElem)){
      return false;
    }
  }

  return true;
}

bool manualVerify(GPXdoc *doc){
  if(strlen(doc->namespace) == 0){
    return false;
  }
  if(strlen(doc->creator) == 0 || doc->creator == NULL){
    return false;
  }

  if(doc->waypoints == NULL){
    return false;
  }
  ListIterator waypointList = createIterator(doc->waypoints);
  void * wptElem;
  while((wptElem = nextElement(&waypointList)) != NULL){
    if(!manualVerifyWaypoint((Waypoint *)wptElem)){
      return false;
    }
  }

  if(doc->routes == NULL){
    return false;
  }
  ListIterator routeList = createIterator(doc->routes);
  void * rteElem;
  while((rteElem = nextElement(&routeList)) != NULL){
    if(!manualVerifyRoute((Route *)rteElem)){
      return false;
    }
  }

  if(doc->tracks == NULL){
    return false;
  }
  ListIterator trackList = createIterator(doc->tracks);
  void * trkElem;
  while((trkElem = nextElement(&trackList)) != NULL){
    if(!manualVerifyTrack((Track *)trkElem)){
      return false;
    }
  }

  return true;
}

bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile){
  xmlSchemaPtr schema = NULL;
  xmlSchemaParserCtxtPtr ctxt;

  xmlLineNumbersDefault(1);

  ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

  // xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
  schema = xmlSchemaParse(ctxt);
  xmlSchemaFreeParserCtxt(ctxt);

  if(doc == NULL){
    return NULL;
  }
  else {
    xmlDocPtr ptr = createTree(doc);
    int val = isGpxDocValid(ptr, schema);

    if(val == 0){

    } else if(val > 0){
        if(schema != NULL){
          xmlSchemaFree(schema);
          xmlSchemaCleanupTypes();
          xmlCleanupParser();
          xmlMemoryDump();
        }
        xmlFreeDoc(ptr);
        return false;
    } else{
      if(schema != NULL){
        xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
      }
      xmlFreeDoc(ptr);
      return false;
    }
    xmlFreeDoc(ptr);
  }
  if(schema != NULL){
    xmlSchemaFree(schema);
    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
  }

  if(!manualVerify(doc)){
    return false;
  }

  return true;
}


GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){

  xmlDocPtr doc;
  GPXdoc *newDoc;
  xmlSchemaPtr schema = NULL;
  xmlSchemaParserCtxtPtr ctxt;

  xmlLineNumbersDefault(1);

  ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

  // xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
  schema = xmlSchemaParse(ctxt);
  xmlSchemaFreeParserCtxt(ctxt);

  doc = xmlReadFile(fileName, NULL, 0);

  if(doc == NULL){
    return NULL;
  }
  else {

    int val = isGpxDocValid(doc, schema);

    if(val == 0){
      newDoc = createGPXdoc(fileName);
    } else if(val > 0){
        if(schema != NULL){
          xmlSchemaFree(schema);
          xmlSchemaCleanupTypes();
          xmlCleanupParser();
          xmlMemoryDump();
        }
        xmlFreeDoc(doc);
        return NULL;
    } else{
      if(schema != NULL){
        xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
      }
      xmlFreeDoc(doc);
      return NULL;
    }
    xmlFreeDoc(doc);
  }

  if(schema != NULL){
    xmlSchemaFree(schema);
    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
  }

  return(newDoc);
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
  if(doc != NULL){
    free(doc->creator);
    freeList(doc->waypoints);
    freeList(doc->routes);
    freeList(doc->tracks);
    free(doc);
  }
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
  if(doc == 0){
    return 0;
  }
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

double dist(double lat1, double lon1, double lat2, double lon2){
  double dx, dy, dz;

  lon1 -= lon2;

  lon1 *= TO_RAD;
  lat1 *= TO_RAD;
  lat2 *= TO_RAD;

  dz = sin(lat1) - sin(lat2);
  dx = cos(lon1) * cos(lat1) - cos(lat2);
  dy = sin(lon1) * cos(lat1);

  double final = asin(sqrt(dx * dx + dy * dy + dz * dz) / 2) * 2 * R;
  printf("%f\n", final);
  return final;
}

float getRouteLen(const Route *rt){
  if(rt == NULL){
    return 0.0;
  }
  double total = 0;

  ListIterator waypointListFront = createIterator(rt->waypoints);
  ListIterator waypointListBack = createIterator(rt->waypoints);

  void * rteElemFront;
  void * rteElemBack;
  rteElemFront = nextElement(&waypointListFront);
  while((rteElemFront = nextElement(&waypointListFront)) != NULL){
    rteElemBack = nextElement(&waypointListBack);
    total += dist( ((Waypoint *)rteElemBack)->latitude, ((Waypoint *)rteElemBack)->longitude, ((Waypoint *)rteElemFront)->latitude, ((Waypoint *)rteElemFront)->longitude );
  }
  return total;
}


float getTrkSegLen(TrackSegment *trkSeg){
  if(trkSeg == NULL){
    return 0.0;
  }
  double total = 0;

  ListIterator waypointListFront = createIterator(trkSeg->waypoints);
  ListIterator waypointListBack = createIterator(trkSeg->waypoints);

  void * trkSegElemFront;
  void * trkSegElemBack;
  trkSegElemFront = nextElement(&waypointListFront);
  while((trkSegElemFront = nextElement(&waypointListFront)) != NULL){
    trkSegElemBack = nextElement(&waypointListBack);
    total += dist( ((Waypoint *)trkSegElemBack)->latitude, ((Waypoint *)trkSegElemBack)->longitude, ((Waypoint *)trkSegElemFront)->latitude, ((Waypoint *)trkSegElemFront)->longitude );
  }
  return total;
}

float getTrackLen(const Track *tr){
  if(tr == NULL){
    return 0.0;
  }

  double total = 0.0;

  ListIterator trackSegmentList = createIterator(tr->segments);
  void * trkElem = NULL;
  void * checkElem = nextElement(&trackSegmentList);
  while(checkElem != NULL){
    // trkElem = checkElem;
    if(getLength(((TrackSegment*)checkElem)->waypoints) != 0){
        total += getTrkSegLen((TrackSegment *)checkElem);
        if(trkElem != NULL){
          ListIterator currentSegment = createIterator(((TrackSegment *)checkElem)->waypoints);
          ListIterator lastSegment = createIterator(((TrackSegment *)trkElem)->waypoints);
          void * last;
          void *first = nextElement(&currentSegment);
          for(int i = 0; i < getLength(((TrackSegment *)trkElem)->waypoints); i++){
            last = nextElement(&lastSegment);
          }

          total += dist( ((Waypoint *)last)->latitude, ((Waypoint *)last)->longitude, ((Waypoint *)first)->latitude, ((Waypoint *)first)->longitude );
        }
        trkElem = checkElem;
    }
    checkElem = nextElement(&trackSegmentList);
  }
  return total;
}


float round10(float len){

  float temp = (5 +len) / 10;

  int temp2 = temp;
  temp2 *= 10;

  return (float)temp2;
}

int numRoutesWithLength(const GPXdoc* doc, float len, float delta){
  if(doc == NULL || len < 0 || delta < 0){
    return 0.0;
  }
  ListIterator routesList = createIterator(doc->routes);
  int num = 0;
  void * rteElem;
  while((rteElem = nextElement(&routesList)) != NULL){
    double temp = getRouteLen((Route *)rteElem);
    if(temp >= (len - delta) && temp <= (len + delta)){
      num++;
    }
  }
  return num;
}



int numTracksWithLength(const GPXdoc* doc, float len, float delta){
  if(doc == NULL || len < 0 || delta < 0){
    return 0.0;
  }
  ListIterator trackList = createIterator(doc->tracks);
  int num = 0;
  void * trkElem;
  while((trkElem = nextElement(&trackList)) != NULL){
    double temp = getTrackLen((Track *)trkElem);
    if(temp >= (len - delta) && temp <= (len + delta)){
      num++;
    }
  }
  return num;
}


bool isLoopRoute(const Route* route, float delta){
  if(route == NULL || delta < 0){
    return false;
  }
  if(getLength(route->waypoints) < 4){
    return false;
  }
  void *first;
  void *last;
  ListIterator waypointListFirst = createIterator(route->waypoints);
  ListIterator waypointListLast = createIterator(route->waypoints);

  first = nextElement(&waypointListFirst);
  for(int i = 0; i < getLength(route->waypoints); i++){
    last = nextElement(&waypointListLast);
  }
  double distance = dist( ((Waypoint *)last)->latitude, ((Waypoint *)last)->longitude, ((Waypoint *)first)->latitude, ((Waypoint *)first)->longitude );
  // printf("%lf %f\n", distance, delta);
  if(distance <= delta && distance >= (0 - delta)){
    return true;
  }
  return false;
}

int getNumWaypointTrk(const Track *tr){
  ListIterator segmentList = createIterator(tr->segments);
  void *elem;
  int total = 0;
  while((elem = nextElement(&segmentList)) != NULL){
    total += getLength(((TrackSegment *)elem)->waypoints);
  }
  return total;
}

Waypoint *getFirstWptTrk(const Track *tr){
  ListIterator segmentList = createIterator(tr->segments);
  void *elem = nextElement(&segmentList);
  while(getLength(((TrackSegment *)elem)->waypoints) == 0 && elem != NULL){
    elem = nextElement(&segmentList);
  }
  if(elem == NULL){
    return NULL;
  }
  ListIterator waypointList = createIterator(((TrackSegment *)elem)->waypoints);
  void *wptElem = nextElement(&waypointList);
  return((Waypoint *)wptElem);
}

Waypoint *getLastWptTrk(const Track *tr){
  ListIterator trackSegmentList = createIterator(tr->segments);
  void * trkElem = NULL;
  void * checkElem = nextElement(&trackSegmentList);
  while(checkElem != NULL){
    if(getLength(((TrackSegment*)checkElem)->waypoints) != 0){
        trkElem = checkElem;
    }
    checkElem = nextElement(&trackSegmentList);
  }
  void *wptElem;
  ListIterator waypointList = createIterator(((TrackSegment *)trkElem)->waypoints);
  for(int i = 0; i < getLength(((TrackSegment*)trkElem)->waypoints); i++){
    wptElem = nextElement(&waypointList);
  }

  return((Waypoint *)wptElem);
}

bool isLoopTrack(const Track *tr, float delta){
  if(tr == NULL || delta < 0){
    return false;
  }
  if(getNumWaypointTrk(tr) < 4){
    return false;
  }
  Waypoint *first = getFirstWptTrk(tr);
  // printf("%lf %lf\n", first->latitude, first->longitude);
  Waypoint *last = getLastWptTrk(tr);
  // printf("%lf %lf\n", last->latitude, last->longitude);
  double distance = dist( ((Waypoint *)last)->latitude, ((Waypoint *)last)->longitude, ((Waypoint *)first)->latitude, ((Waypoint *)first)->longitude );
  // printf("%lf\n", distance);
  if(distance <= delta && distance >= (0 - delta)){
    return true;
  }
  return false;
}

void dummyDelete(){

}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
  if(doc == NULL){
    return NULL;
  }

  List *routeList = initializeList(&routeToString, &dummyDelete, &compareRoutes);

  ListIterator routesList = createIterator(doc->routes);
  void * rteElem;
  while((rteElem = nextElement(&routesList)) != NULL){
    void *first;
    void *last;
    ListIterator waypointListFirst = createIterator(((Route *)rteElem)->waypoints);
    ListIterator waypointListLast = createIterator(((Route *)rteElem)->waypoints);

    first = nextElement(&waypointListFirst);
    for(int i = 0; i < getLength(((Route *)rteElem)->waypoints); i++){
      last = nextElement(&waypointListLast);
    }
    double firstDist = dist(sourceLat, sourceLong, ((Waypoint *)first)->latitude, ((Waypoint *)first)->longitude);
    double lastDist = dist(destLat, destLong, ((Waypoint *)last)->latitude, ((Waypoint *)last)->longitude);

    printf("%lf %lf\n", firstDist, lastDist);

    if(firstDist <= delta && lastDist <= delta){
      insertBack(routeList, (Route *)rteElem);
    }
  }
  if(getLength(routeList) == 0){
    freeList(routeList);
    return NULL;
  }
  return routeList;
}


List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){
 if(doc == NULL){
   return NULL;
 }
 List *ListTracks = initializeList(&trackToString, &dummyDelete, compareTracks);
 ListIterator trackList = createIterator(doc->tracks);
 void *trkElem;
 while((trkElem = nextElement(&trackList)) != NULL){
   Waypoint *first = getFirstWptTrk((Track *)trkElem);
   Waypoint *last = getLastWptTrk((Track *)trkElem);

   double firstDist = dist(sourceLat, sourceLong, ((Waypoint *)first)->latitude, ((Waypoint *)first)->longitude);
   double lastDist = dist(destLat, destLong, ((Waypoint *)last)->latitude, ((Waypoint *)last)->longitude);

   if(firstDist <= delta && lastDist <= delta){
     insertBack(ListTracks, (Track *)trkElem);
   }
 }
 if(getLength(ListTracks) == 0){
   freeList(ListTracks);
   return NULL;
 }
 return ListTracks;
}

/** Function to converting a Track into a JSON string
 *@pre Track is not NULL
 *@post Track has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Track struct
 **/
char* trackToJSON(const Track *tr){
  if(tr == NULL){
    return "{}";
  }
  char *final = malloc(sizeof(char) * 100000);
  char *name, *loop;
  if(strlen(tr->name) == 0){
    name = "None";
  } else {
    name = tr->name;
  }
  float len;
  len = round10(getTrackLen(tr));
  if(isLoopTrack(tr, 10)){
    loop = "true";
  } else{
    loop = "false";
  }
  int num = numWaypoints((Track *)tr);
  sprintf(final, "{\"name\":\"%s\",\"len\":%.1f,\"loop\":%s,\"numPoints\":%d}", name, len, loop, num);
  return final;
}

/** Function to converting a Route into a JSON string
 *@pre Route is not NULL
 *@post Route has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Route struct
 **/
char* routeToJSON(const Route *rt){
  if(rt == NULL){
    return "{}";
  }
    char *final = malloc(sizeof(char) * 100000);
    char *name, *loop;
    int numpoints;
    float len;


    if(strlen(rt->name) == 0){
      name = "None";
    } else {
      name = rt->name;
    }
    numpoints = getLength(rt->waypoints);
    len = round10(getRouteLen(rt));
    if(isLoopRoute(rt, 10)){
      loop = "true";
    } else{
      loop = "false";
    }

    sprintf(final, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", name, numpoints, len, loop);

    return final;
}

/** Function to converting a list of Route structs into a JSON string
 *@pre Route list is not NULL
 *@post Route list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* routeListToJSON(const List *list){
  if(list == NULL){
    return "[]";
  }
  char *final = malloc(sizeof(char)* 1000000);
  final[0] = '\0';
  strcat(final, "[");
  ListIterator iterator = createIterator((List *)list);
  void *elem;
  int i = 0;
  while((elem = nextElement(&iterator)) != NULL){
    char *temp = routeToJSON((Route *)elem);
    strcat(final, temp);
    if(i < getLength((List *)list) - 1){
      strcat(final, ",");
    }
    i++;
    free(temp);
  }
  strcat(final, "]");
  return final;
}

/** Function to converting a list of Track structs into a JSON string
 *@pre Track list is not NULL
 *@post Track list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* trackListToJSON(const List *list){
  if(list == NULL){
    return "[]";
  }
  char *final = malloc(sizeof(char)* 1000000);
  final[0] = '\0';
  strcat(final, "[");
  ListIterator iterator = createIterator((List *)list);
  void *elem;
  int i = 0;
  while((elem = nextElement(&iterator)) != NULL){
    char *temp = trackToJSON((Track *)elem);
    strcat(final, temp);
    if(i < getLength((List *)list) - 1){
      strcat(final, ",");
    }
    free(temp);
    i++;
  }
  strcat(final, "]");
  return final;
}

char *otherDataToJSON(GPXData *elem){
  char *final = malloc(sizeof(char) * 100000);

  if(elem == NULL){
    return "{}";
  }
  char *value = elem->value;
  char *name = elem->name;

  for(int i = 0; i < strlen(value); i++){
    if(value[i] == '\n'){
      value[i] = '\0';
    }
  }

  sprintf(final, "{\"name\":\"%s\",\"value\":\"%s\"}", name, value);

  return final;
}

char *otherDataListToJSON(List* list){
  if(list == NULL){
    return "[]";
  }
  char *final = malloc(sizeof(char)* 1000000);
  final[0] = '\0';
  strcat(final, "[");
  ListIterator iterator = createIterator((List *)list);
  void *elem;
  int i = 0;
  while((elem = nextElement(&iterator)) != NULL){
    char *temp = otherDataToJSON((GPXData *)elem);
    strcat(final, temp);
    if(i < getLength((List *)list) - 1){
      strcat(final, ",");
    }
    free(temp);
    i++;
  }
  strcat(final, "]");
  return final;
}

char *getRouteOtherData(char *fileName, char *routeName){
  GPXdoc *doc = createGPXdoc(fileName);
  ListIterator iterator = createIterator(doc->routes);
  void *elem;
  char *string;

  while((elem = nextElement(&iterator)) != NULL){
    if(strcmp(((Route *)elem)->name, routeName) == 0){
      // printf("%s compared to %s\n", ((Route *)elem)->name, routeName);
      string = otherDataListToJSON(((Route *)elem)->otherData);
      // printf("%s\n", string);
      break;
    }
  }

  return(string);
}

char *getTrackOtherData(char *fileName, char *routeName){
  GPXdoc *doc = createGPXdoc(fileName);
  ListIterator iterator = createIterator(doc->tracks);
  void *elem;
  char *string;

  while((elem = nextElement(&iterator)) != NULL){
    if(strcmp(((Track *)elem)->name, routeName) == 0){
      // printf("%s compared to %s\n", ((Route *)elem)->name, routeName);
      string = otherDataListToJSON(((Track *)elem)->otherData);
      printf("%s\n", string);
      break;
    }
  }

  return(string);
}
/** Function to converting a GPXdoc into a JSON string
 *@pre GPXdoc is not NULL
 *@post GPXdoc has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a GPXdoc struct
 **/
char* GPXtoJSON(const GPXdoc* gpx){
  if(gpx == NULL){
    return "{}";
  }
  char *final = malloc(sizeof(char) * 100000);
  char *creator;
  int numWaypoints, numRoutes, numTracks;
  float version;


  creator = gpx->creator;
  numWaypoints = getNumWaypoints((GPXdoc *)gpx);
  version = gpx->version;
  numRoutes = getNumRoutes((GPXdoc *)gpx);
  numTracks = getNumTracks((GPXdoc *)gpx);

  sprintf(final, "{\"version\":%.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", version, creator, numWaypoints, numRoutes, numTracks);

  return final;
}

void addWaypoint(Route *rt, Waypoint *pt){

}

/** Function to adding an Route struct to an existing GPXdoc struct
 *@pre arguments are not NULL
 *@post The new route has been added to the GPXdoc's routes list
 *@return N/A
 *@param doc - a GPXdoc struct
 *@param rt - a Route struct
 **/
void addRoute(GPXdoc* doc, Route* rt){

}

/** Function to converting a JSON string into an GPXdoc struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized GPXdoc struct
 *@param str - a pointer to a string
 **/
GPXdoc* JSONtoGPX(const char* gpxString){

  return NULL;
}

/** Function to converting a JSON string into an Waypoint struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Waypoint struct
 *@param str - a pointer to a string
 **/
Waypoint* JSONtoWaypoint(const char* gpxString){
  return NULL;
}

/** Function to converting a JSON string into an Route struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Route struct
 *@param str - a pointer to a string
 **/
Route* JSONtoRoute(const char* gpxString){
  return NULL;
}
