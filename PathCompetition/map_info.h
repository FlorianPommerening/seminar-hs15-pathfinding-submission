#ifndef MAP_INFO
#define MAP_INFO

#include "locations.h"

#include <iostream>
#include <vector>

typedef std::vector<int> RoomMap;

static const double SQUARE_TWO = 1.4142;
static const double EPSILON = 10e-8;

class ExitSuccesor;

struct Exit {
    int id;
    int room_id;
    xyLoc location;
    std::vector<ExitSuccesor> successors;

    friend std::ostream& operator<<(std::ostream& os, const Exit& dt);
    friend std::istream& operator>>(std::istream& os, Exit& dt);
};

struct ExitSuccesor {
    int id;
    double distance;

    friend std::ostream& operator<<(std::ostream& os, const ExitSuccesor& dt);
    friend std::istream& operator>>(std::istream& os, ExitSuccesor& dt);
};

struct MapInfo {
    std::vector<bool> map;
    int width;
    int height;
    int num_rooms;
    RoomMap rooms;
    std::vector<std::vector<int>> room_exits;
    std::vector<Exit> exits;

    bool get_occupied(int x, int y) const {
        if (x < 0 || y < 0 || x >= width || y >= height)
            return true;
        return !map[x + y * width];
    }

    int get_room(int x, int y) const {
        if (x < 0 || y < 0 || x >= width || y >= height)
            return -2;
        return rooms[x + y * width];
    }

    void set_room(int x, int y, int room_id) {
        rooms[x + y * width] = room_id;
    }

    friend std::ostream& operator<<(std::ostream& os, const MapInfo& dt);
    friend std::istream& operator>>(std::istream& os, MapInfo& dt);
};

#endif

