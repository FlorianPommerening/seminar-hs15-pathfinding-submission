#ifndef ROOM_DETECTION_H
#define ROOM_DETECTION_H

#include "map_info.h"
#include "locations.h"

#include <vector>

// Flood filling algorithm from Bjornsson and Halldorsson, 2006
void detect_rooms(MapInfo &map_info);

#endif
