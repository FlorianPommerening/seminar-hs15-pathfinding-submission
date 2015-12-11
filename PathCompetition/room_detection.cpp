#include "room_detection.h"

int FREE = -1;
int IMPASSABLE = -2;

bool find_unassigned_tile(const MapInfo &map_info,
                          int &unassigned_x, int&unassigned_y) {
    for (int x = 0; x < map_info.width; ++x) {
        for (int y = 0; y < map_info.height; ++y) {
            if (map_info.get_room(x, y) == FREE) {
                unassigned_x = x;
                unassigned_y = y;
                return true;
            }
        }
    }
    return false;
}

bool detect_room(MapInfo &map_info, int room_id) {
    int x_left = 0;
    int y = 0;
    if (!find_unassigned_tile(map_info, x_left, y))
        return false;
    bool shrunk_r = false;
    bool shrunk_l = false;
    while (true) {
        // Mark line until hit wall or area opens upwards
        int x = x_left;
        map_info.set_room(x, y, room_id);
        while (map_info.get_room(x+1, y) == FREE &&
               map_info.get_room(x+1, y-1) != FREE) {
            ++x;
            map_info.set_room(x, y, room_id);
        }
        // Stop filling area if right border regrowing
        if (map_info.get_room(x+1, y-1) == room_id) {
            shrunk_r = true;
        } else if (shrunk_r && map_info.get_room(x, y-1) != room_id) {
            // Undo line markings
            while (map_info.get_room(x, y) == room_id) {
                map_info.set_room(x, y, FREE);
                --x;
            }
            break;
        }
        // Goto same initial x-pos in next line
        x = x_left;
        ++y;
        // If on obstacle, go right in zone until empty
        while (map_info.get_room(x, y) != FREE &&
               map_info.get_room(x, y-1) == room_id) {
            ++x;
        }
        if (map_info.get_room(x, y) != FREE) {
            break;
        }
        // Move further left until wall or opens upwards
        while (map_info.get_room(x-1, y) == FREE &&
               map_info.get_room(x-1, y-1) != FREE) {
            --x;
        }
        // Stop filling area if left border regrowing
        if (map_info.get_room(x-1, y-1) == room_id) {
            shrunk_l = true;
        } else if (shrunk_l && map_info.get_room(x, y-1) != room_id) {
            break;
        }
    }
    return true;
}


void detect_rooms(MapInfo &map_info) {
    map_info.rooms.clear();
    map_info.rooms.reserve(map_info.map.size());
    for (int i = 0; i < map_info.map.size(); ++i){
        if (map_info.map[i]) {
            map_info.rooms.push_back(FREE);
        } else {
            map_info.rooms.push_back(IMPASSABLE);
        }
    }
    int room_id = 0;
    while (detect_room(map_info, room_id)) {
        ++room_id;
    }
}
