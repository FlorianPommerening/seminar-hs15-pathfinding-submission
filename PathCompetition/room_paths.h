#ifndef ROOM_PATHS_H
#define ROOM_PATHS_H

#include "locations.h"
#include "map_info.h"

#include <vector>

std::vector<xyLoc> compute_room_path(const MapInfo &map_info, xyLoc start, xyLoc goal);
double compute_room_path_cost(const MapInfo &map_info, xyLoc start, xyLoc goal);

#endif
