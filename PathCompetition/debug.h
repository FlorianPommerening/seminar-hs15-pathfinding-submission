#ifndef DEBUG_H
#define DEBUG_H

#include "map_info.h"
#include "search_space.h"

#include <string>

void draw_rooms(std::string filename, const MapInfo &map_info);
void draw_path(std::string filename, const MapInfo &map_info,
               SearchSpace &search_space, std::vector<xyLoc> &path);

#endif
