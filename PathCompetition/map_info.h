#ifndef MAP_INFO
#define MAP_INFO

#include "locations.h"

#include <vector>

typedef std::vector<int> RoomMap;
typedef std::pair<xyLoc, int> Exit;

struct MapInfo {
    std::vector<bool> map;
    int width;
    int height;
    RoomMap rooms;
    std::vector<std::vector<Exit> > room_exits;

    bool get_occupied(int x, int y) const {
        if (x < 0 || y < 0 || x >= width || y >= height)
            return true;
        return map[x + y * width];
    }

    int get_room(int x, int y) const {
        if (x < 0 || y < 0 || x >= width || y >= height)
            return -2;
        return rooms[x + y * width];
    }

    void set_room(int x, int y, int room_id) {
        rooms[x + y * width] = room_id;
    }
};


#endif

