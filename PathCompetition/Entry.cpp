#include "Entry.h"

#include "debug.h"
#include "room_detection.h"
#include "search_algorithm.h"

#include <deque>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <cmath>


using namespace std;

const char *GetName() {
    return "Florian Pommerening";
}


void PreprocessMap(vector<bool> &bits, int width, int height, const char *filename) {
    printf("Not writing to file '%s'\n", filename);
}


void *PrepareForSearch(vector<bool> &bits, int w, int h, const char *filename) {
    printf("Not reading from file '%s'\n", filename);

    // Note: this is a memory leak, but the infrastructure code gives us no choice.
    MapInfo *map_info = new MapInfo;
    map_info->map = bits;
    map_info->width = w;
    map_info->height = h;
    detect_rooms(*map_info);
    draw_rooms("rooms.png", *map_info);
    return map_info;
}


bool GetPath(void *data, xyLoc s_loc, xyLoc g_loc, vector<xyLoc> &path) {
    MapInfo *map_info = static_cast<MapInfo *>(data);
    SearchAlgorithm algo(map_info, s_loc, g_loc, path);
    return algo.search();
}
