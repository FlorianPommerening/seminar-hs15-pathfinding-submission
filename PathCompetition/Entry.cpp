#include "Entry.h"

#include "debug.h"
#include "gate_detection.h"
#include "room_detection.h"
#include "search_algorithm.h"

#include <deque>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <fstream>


using namespace std;

const char *GetName() {
    return "Florian Pommerening";
}


void PreprocessMap(vector<bool> &bits, int width, int height, const char *filename) {
    // Note: this is a memory leak, but the infrastructure code gives us no choice.
    MapInfo map_info;
    map_info.map = bits;
    map_info.width = width;
    map_info.height = height;
    detect_rooms(map_info);
    detect_gates(map_info);
    draw_rooms("rooms.png", map_info);

    printf("Writing to file '%s'\n", filename);
    ofstream map_file;
    map_file.open (filename);
    map_file << map_info;
    map_file.close();
}


void *PrepareForSearch(vector<bool> &bits, int w, int h, const char *filename) {
    // Note: this is a memory leak, but the infrastructure code gives us no choice.
    MapInfo *map_info = new MapInfo;

    printf("Reading from file '%s'\n", filename);
    ifstream map_file(filename);
    map_file >> (*map_info);
    map_file.close();

    return map_info;
}


bool GetPath(void *data, xyLoc s_loc, xyLoc g_loc, vector<xyLoc> &path) {
    MapInfo *map_info = static_cast<MapInfo *>(data);
    SearchAlgorithm algo(map_info, s_loc, g_loc, path);
    return algo.search();
}
