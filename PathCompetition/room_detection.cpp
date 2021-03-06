#include "room_detection.h"

#include "locations.h"

#include <cassert>
#include <iostream>
#include <functional>
#include <vector>

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

void get_max_free_span(int major_index, int min_minor_index, int max_minor_index,
                       function<bool(int, int)> is_free,
                       int &span_start, int &span_length) {
    span_start = min_minor_index;
    span_length = 0;
    int current_span_start = min_minor_index;
    bool span_running = false;
    for (int minor_index = min_minor_index; minor_index < max_minor_index + 1; ++minor_index) {
        if (span_running &&
                (minor_index == max_minor_index || !is_free(minor_index, major_index))) {
            int current_span_length = minor_index - current_span_start;
            if (current_span_length > span_length) {
                span_start = current_span_start;
                span_length = current_span_length;
            }
            span_running = false;
        } else if (!span_running && is_free(minor_index, major_index)) {
            span_running = true;
            current_span_start = minor_index;
        }
    }
}

int extend_room_in_direction(
        int major_index_start, int direction,
        int room_start, int room_end,
        function<bool(int, int)> is_free,
        function<bool(int, int, int)> stop,
        function<void(int, int)> set_room,
        int &room_size_at_room_start,
        int &room_size_at_room_end) {
    assert(direction == 1 || direction == -1);
    int previous_span_length = room_end - room_start;
    int span_start;
    int span_length;
    int min_span_start = room_start;
    int max_span_end = room_end;

    int major_index = major_index_start;
    while (true) {
        get_max_free_span(
            major_index, min_span_start, max_span_end, is_free,
            span_start, span_length);

        min_span_start = span_start;
        max_span_end = span_start + span_length;

        int num_free = 0;
        for (int minor_index = min_span_start - 1; is_free(minor_index, major_index); --minor_index)
            ++num_free;
        for (int minor_index = max_span_end; is_free(minor_index, major_index); ++minor_index)
            ++num_free;
        if (stop(previous_span_length, span_length, num_free))
            break;
        previous_span_length = span_length;

        if (span_start == room_start) {
            room_size_at_room_start = major_index;
        }
        if (span_start + span_length == room_end) {
            room_size_at_room_end = major_index;
        }

        for (int minor_index = min_span_start; minor_index < max_span_end; ++minor_index) {
            set_room(minor_index, major_index);
        }
        major_index += direction;
    }
    // Undo overcounting in last iteration
    major_index -= direction;
    return major_index;
}

void extend_room_in_all_directions(
        MapInfo &map_info, int room_id,
        int &room_x, int &room_y, int &room_w, int &room_h,
        function<bool(int, int, int)> stop) {
    function<bool(int, int)> is_free =
    [&](int x, int y) {
        return map_info.get_room(x, y) == FREE;
    };

    function<bool(int, int)> is_free_transposed =
    [&](int y, int x) {
        return map_info.get_room(x, y) == FREE;
    };

    function<void(int, int)> set_room =
    [&](int x, int y) {
        map_info.set_room(x, y, room_id);
    };

    function<void(int, int)> set_room_transposed =
    [&](int y, int x) {
        map_info.set_room(x, y, room_id);
    };
    int unused_output;
    int min_y_l = room_y;
    int max_y_l = room_y + room_h - 1;
    int min_y_r = room_y;
    int max_y_r = room_y + room_h - 1;

    // Try to extend room upwards
    int min_y = extend_room_in_direction(
        room_y - 1, -1,
        room_x, room_x + room_w,
        is_free, stop, set_room,
        min_y_l, min_y_r);

    // Try to extend room downwards
    int max_y = extend_room_in_direction(
        room_y + room_h, 1,
        room_x, room_x + room_w,
        is_free, stop, set_room,
        max_y_l, max_y_r);

    // Try to extend room to left
    int min_x = extend_room_in_direction(
        room_x - 1, -1,
        min_y_l, max_y_l + 1,
        is_free_transposed, stop, set_room_transposed,
        unused_output, unused_output);

    // Try to extend room to right
    int max_x = extend_room_in_direction(
        room_x + room_w, 1,
        min_y_r, max_y_r + 1,
        is_free_transposed, stop, set_room_transposed,
        unused_output, unused_output);

    room_x = min_x;
    room_y = min_y;
    room_w = max_x - min_x + 1;
    room_h = max_y - min_y + 1;
}

void build_large_room(MapInfo &map_info, int room_id, int room_x, int room_y,
                      int room_size) {
    int room_w = room_size;
    int room_h = room_size;

    for (int x = room_x; x < room_x + room_w; ++x) {
        for (int y = room_y; y < room_y + room_h; ++y) {
            map_info.set_room(x, y, room_id);
        }
    }

    // Stop if area gets smaller
    function<bool(int, int, int)> stop_on_smaller_span =
    [&](int previous_span_length, int span_length, int /*num_free*/) {
        return (span_length < previous_span_length);
    };
    extend_room_in_all_directions(
        map_info, room_id, room_x, room_y, room_w, room_h, stop_on_smaller_span);

    // Stop if area opens up after a gate
    function<bool(int, int, int)> stop_on_gate =
    [&](int /*previous_span_length*/, int span_length, int num_free) {
        return (span_length <= 2*num_free);
    };

    extend_room_in_all_directions(
        map_info, room_id, room_x, room_y, room_w, room_h, stop_on_gate);
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

    /*
    while (detect_room_bjornsson(map_info, room_id)) {
        ++room_id;
    }
    */

    map_info.num_rooms = room_id;
    cout << "Found " << map_info.num_rooms << " rooms in total" << endl;
}
