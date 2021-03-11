#include "GPXParser.h"

int main(int argc, char * argv[]) {
    GPXdoc * g = createValidGPXdoc("testA2.gpx", "gpx.xsd");
    printf("isValidGPXdoc: %d\n\n", validateGPXDoc(g, "gpx.xsd"));
    writeGPXdoc(g, "testA2writeGPXdoc.gpx");

    printf("Route distance: %f\n", getRouteLen(getRoute(g, "Some route")));
    printf("isLoopRoute: %d\n\n", isLoopRoute(getRoute(g, "Some route"), 10));
    printf("Track distance: %f\n", getTrackLen(getTrack(g, "Mount Steele Trail")));
    printf("isLoopTrack: %d\n\n", isLoopTrack(getTrack(g, "Mount Steele Trail"), 1000));

    printf("Num routes: %d %d %d %d %d %d\n",
            numRoutesWithLength(g, 315, 0.5),
            numRoutesWithLength(g, 313, 1),
            numRoutesWithLength(g, 312, 1),
            numRoutesWithLength(g, 312, 2),
            numRoutesWithLength(g, 312, 20),
            numRoutesWithLength(g, 323, 10));

    printf("Num tracks: %d %d %d %d\n",
            numTracksWithLength(g, 337, .1),
            numTracksWithLength(g, 337, .5),
            numTracksWithLength(g, 338, .5),
            numTracksWithLength(g, 338, 2));

    printf("Is route loop: %d %d %d\n",
            isLoopRoute(getRoute(g, "Some route"), 0),
            isLoopRoute(getRoute(g, "Some route"), 10),
            isLoopRoute(getRoute(g, "Some other route"), 1000));

    List * l = getRoutesBetween(g, 44.595860, -123.620950, 44.595860, -123.620955, 2);
    printf("Num routes between: %d\n", l == NULL ? 0 : getLength(l));
    freeList(l);

    l = getTracksBetween(g, 49.595850, -123.620950, 49.598350, -123.619520, 2);
    printf("Num tracks between: %d\n", l == NULL ? 0 : getLength(l));
    freeList(l);

    // char * gpxJson = GPXtoJSON(g);
    // printf("\nGPX JSON: '%s'\n", gpxJson);
    // free(gpxJson);
    //
    char * gpxToJSON = GPXtoJSON(g);
    printf("GPX JSON: '%s'\n", gpxToJSON);
    free(gpxToJSON);


    char * trkJson = trackToJSON(getFromFront(g->tracks));
    printf("\nTrack JSON: '%s'\n", trkJson);
    free(trkJson);

    char * rteJson = routeToJSON(getFromFront(g->routes));
    printf("\nRoute JSON:'%s'\n", rteJson);
    free(rteJson);

    char * rteListJson = routeListToJSON(g->routes);
    printf("\nRoute List JSON: '%s'\n", rteListJson);
    free(rteListJson);

    char * trkListJson = trackListToJSON(g->tracks);
    printf("\nTrack List JSON: '%s'\n", trkListJson);
    free(trkListJson);


    deleteGPXdoc(g);

    // g = JSONtoGPX("{\"version\":1.1,\"creator\":\"WebTool\"}");
    // Route * r = JSONtoRoute("{\"name\":\"Reynolds walk\"}");
    // Waypoint * p = JSONtoWaypoint("{\"lat\":43.537299,\"lon\":-80.218267}\"}");
    // Waypoint * p2 = JSONtoWaypoint("{\"lat\":44.537299,\"lon\":-81.218267}\"}");
    // addWaypoint(r, p);
    // addWaypoint(r, p2);
    // addRoute(g, r); // swg
    // char * str = GPXdocToString(g);
    //
    // printf("\nCheck \"fromJSONs.gpx\" and make sure it matches (output will vary here based on GPXdocToString()):\n%s\n\n", str);
    // writeGPXdoc(g, "fromJSONs.gpx");
    // free(str);
    // deleteGPXdoc(g);
    //
    // if (JSONtoRoute(NULL) != NULL) printf("Bad return with NULL argument\n");
    // if (JSONtoWaypoint(NULL) != NULL) printf("Bad return with NULL argument\n");
    // if (JSONtoGPX(NULL) != NULL) printf("Bad return with NULL argument\n");
}
