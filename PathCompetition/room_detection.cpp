#include "room_detection.h"

#include <iostream>

using namespace std;

int FREE = -1;
int IMPASSABLE = -2;
int UNINITIALIZED = -3;

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

bool detect_room_bjornsson(MapInfo &map_info, int room_id) {
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
        if (map_info.get_room(x, y-1) != room_id) {
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


void get_max_free_row_span(MapInfo &map_info, int row, int min_col, int max_col,
                           int &span_start, int &span_length) {
    span_start = min_col;
    span_length = 0;
    int current_span_start = min_col;
    bool span_running = false;
    for (int x = min_col; x < max_col + 1; ++x) {
        if (span_running && (x == max_col || map_info.get_room(x, row) != FREE)) {
            int current_span_length = x - current_span_start;
            if (current_span_length > span_length) {
                span_start = current_span_start;
                span_length = current_span_length;
                span_running = false;
            }
        } else if (!span_running && map_info.get_room(x, row) == FREE) {
            span_running = true;
            current_span_start = x;
        }
    }
}

void get_max_free_col_span(MapInfo &map_info, int col, int min_row, int max_row,
                           int &span_start, int &span_length) {
    span_start = min_row;
    span_length = 0;
    int current_span_start = min_row;
    bool span_running = false;
    for (int y = min_row; y < max_row + 1; ++y) {
        if (span_running && (y == max_row || map_info.get_room(col, y) != FREE)) {
            int current_span_length = y - current_span_start;
            if (current_span_length > span_length) {
                span_start = current_span_start;
                span_length = current_span_length;
                span_running = false;
            }
        } else if (!span_running && map_info.get_room(col, y) == FREE) {
            span_running = true;
            current_span_start = y;
        }
    }
}
void build_large_room(MapInfo &map_info, int room_id, int room_x, int room_y,
                      int room_size) {
    int room_w = room_size;
    int room_h = room_size;

    // Try to extend room upwards
    while (true) {
        bool blocked = false;
        for (int x = room_x; x < room_x + room_w; ++x) {
            if (map_info.get_room(x, room_y - 1) != FREE) {
                blocked = true;
                break;
            }
        }
        if (blocked)
            break;
        ++room_h;
        --room_y;
    }
    // Try to extend room downwards
    while (true) {
        bool blocked = false;
        for (int x = room_x; x < room_x + room_w; ++x) {
            if (map_info.get_room(x, room_y + room_h) != FREE) {
                blocked = true;
                break;
            }
        }
        if (blocked)
            break;
        ++room_h;
    }
    // Try to extend room to left
    while (true) {
        bool blocked = false;
        for (int y = room_y; y < room_y + room_h; ++y) {
            if (map_info.get_room(room_x - 1, y) != FREE) {
                blocked = true;
                break;
            }
        }
        if (blocked)
            break;
        ++room_w;
        --room_x;
    }
    // Try to extend room to right
    while (true) {
        bool blocked = false;
        for (int y = room_y; y < room_y + room_h; ++y) {
            if (map_info.get_room(room_x + room_w, y) != FREE) {
                blocked = true;
                break;
            }
        }
        if (blocked)
            break;
        ++room_w;
    }



    for (int x = room_x; x < room_x + room_w; ++x) {
        for (int y = room_y; y < room_y + room_h; ++y) {
            map_info.set_room(x, y, room_id);
        }
    }

    int min_x = room_x;
    int max_x = room_x + room_w;
    int min_y_l = room_y;
    int max_y_l = room_y + room_h;
    int min_y_r = room_y;
    int max_y_r = room_y + room_h;

    int span_start;
    int span_length;

    // Try to extend room upwards
    for (int y = room_y-1; y >= 0; --y) {
        get_max_free_row_span(map_info, y, min_x, max_x,
                              span_start, span_length);
        min_x = span_start;
        max_x = span_start + span_length;

        // Stop if area opens up after a gate
        int num_free = 0;
        for (int x = min_x - 1; map_info.get_room(x, y) == FREE; --x)
            ++num_free;
        for (int x = max_x; map_info.get_room(x, y) == FREE; ++x)
            ++num_free;
        if (span_length <= 2*num_free)
            break;

        if (span_start == room_x) {
            --min_y_l;
        }
        if (span_start + span_length == room_x + room_w) {
            --min_y_r;
        }
        for (int x = min_x; x < max_x; ++x) {
            map_info.set_room(x, y, room_id);
        }
    }

    // Try to extend room downwards
    min_x = room_x;
    max_x = room_x + room_w;
    for (int y = room_y + room_h; y < map_info.height; ++y) {
        get_max_free_row_span(map_info, y, min_x, max_x,
                              span_start, span_length);
        min_x = span_start;
        max_x = span_start + span_length;

        // Stop if area opens up after a gate
        int num_free = 0;
        for (int x = min_x - 1; map_info.get_room(x, y) == FREE; --x)
            ++num_free;
        for (int x = max_x; map_info.get_room(x, y) == FREE; ++x)
            ++num_free;
        if (span_length <= 2*num_free)
            break;

        if (span_start == room_x) {
            ++max_y_l;
        }
        if (span_start + span_length == room_x + room_w) {
            ++max_y_r;
        }
        for (int x = min_x; x < max_x; ++x) {
            map_info.set_room(x, y, room_id);
        }
    }


    // Try to extend room to left
    for (int x = room_x-1; x >= 0; --x) {
        get_max_free_col_span(map_info, x, min_y_l, max_y_l,
                              span_start, span_length);
        min_y_l = span_start;
        max_y_l = span_start + span_length;

        // Stop if area opens up after a gate
        int num_free = 0;
        for (int y = min_y_l - 1; map_info.get_room(x, y) == FREE; --y)
            ++num_free;
        for (int y = max_y_l; map_info.get_room(x, y) == FREE; ++y)
            ++num_free;
        if (span_length <= 2*num_free)
            break;

        for (int y = min_y_l; y < max_y_l; ++y) {
            map_info.set_room(x, y, room_id);
        }
    }

    // Try to extend room to right
    for (int x = room_x + room_w; x <= map_info.width; ++x) {
        get_max_free_col_span(map_info, x, min_y_r, max_y_r,
                              span_start, span_length);
        min_y_r = span_start;
        max_y_r = span_start + span_length;

        // Stop if area opens up after a gate
        int num_free = 0;
        for (int y = min_y_r - 1; map_info.get_room(x, y) == FREE; --y)
            ++num_free;
        for (int y = max_y_r; map_info.get_room(x, y) == FREE; ++y)
            ++num_free;
        if (span_length <= 2*num_free)
            break;

        for (int y = min_y_r; y < max_y_r; ++y) {
            map_info.set_room(x, y, room_id);
        }
    }

}

bool detect_room_large(MapInfo &map_info, int room_id, int min_size) {
    vector<vector<int> > wall_distance;
    vector<xyLoc> queue;
    xyLoc best;
    int distance = 0;
    for (int x = 0; x < map_info.width; ++x) {
        wall_distance.push_back(vector<int>(map_info.height, UNINITIALIZED));
        for (int y = 0; y < map_info.height; ++y) {
            if (map_info.get_room(x, y) != FREE) {
                wall_distance[x][y] = distance;
                best = xyLoc(x, y);
                queue.push_back(best);
            }
        }
    }
    for (int x = 0; x < map_info.width; ++x) {
        queue.push_back(xyLoc(x, -1));
        queue.push_back(xyLoc(x, map_info.height));
    }
    for (int y = 0; y < map_info.height; ++y) {
        queue.push_back(xyLoc(-1, y));
        queue.push_back(xyLoc(map_info.width, y));
    }

    vector<xyLoc> next_queue;
    while (!queue.empty()) {
        ++distance;
        while (!queue.empty()) {
            xyLoc loc = queue.back();
            queue.pop_back();
            for (int suc_x = max(0, loc.x - 1); suc_x < min(map_info.width, loc.x + 2); ++suc_x) {
                for (int suc_y = max(0, loc.y - 1); suc_y < min(map_info.height, loc.y + 2); ++suc_y) {
                    if (wall_distance[suc_x][suc_y] == UNINITIALIZED) {
                        wall_distance[suc_x][suc_y] = distance;
                        best = xyLoc(suc_x, suc_y);
                        next_queue.push_back(best);
                    }
                }
            }
        }
        queue.swap(next_queue);
    }
    // There were no nodes with the last distance value.
    --distance;

    if (distance < min_size)
        return false;

    int room_x = best.x - distance + 1;
    int room_y = best.y - distance + 1;
    int room_size = 2*(distance - 1) + 1;
    build_large_room(map_info, room_id, room_x, room_y, room_size);
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

    while (detect_room_large(map_info, room_id, 3)) {
        ++room_id;
    }
    cout << "Found " << room_id << " large rooms" << endl;

    for (int x = 0; x < map_info.width; ++x) {
        for (int y = 0; y < map_info.height; ++y) {
            if (map_info.get_room(x, y) == FREE) {
                build_large_room(map_info, room_id, x, y, 1);
                ++room_id;
            }
        }
    }

    // HACK
    int num_no_room = 0;
    for (int i = 0; i < map_info.map.size(); ++i){
        if (map_info.rooms[i] == FREE) {
            map_info.rooms[i] = room_id + 1;
            ++num_no_room;
        }
    }
    cout << "Found " << num_no_room
         << " out of " << map_info.width * map_info.height
         << " without a room"
         << " (" << (num_no_room * 100) / (map_info.width * map_info.height) << "%)"
         << endl;

    while (detect_room_bjornsson(map_info, room_id)) {
        ++room_id;
    }

    cout << "Found " << room_id << " rooms in total" << endl;


    vector<int> num_gates(room_id, 0);
    for (int x = 0; x < map_info.width; ++x) {
        for (int y = 0; y < map_info.height; ++y) {
            int current_room_id = map_info.get_room(x, y);
            if (current_room_id == IMPASSABLE)
                continue;
            for (int suc_x = max(0, x - 1); suc_x < min(map_info.width, x + 2); ++suc_x) {
                for (int suc_y = max(0, y - 1); suc_y < min(map_info.height, y + 2); ++suc_y) {
                    int succ_room_id = map_info.get_room(suc_x, suc_y);
                    if (succ_room_id != IMPASSABLE && succ_room_id != current_room_id) {
                        ++num_gates[current_room_id];
                    }
                }
            }
        }
    }
    long long num_precomputations = 0;
    for (int i = 0; i < num_gates.size(); ++i) {
        for (int j = i+1; j < num_gates.size(); ++j) {
                num_precomputations += num_gates[i] * num_gates[j];
        }
    }
    cout << "Precomputations " << num_precomputations << endl;
}
