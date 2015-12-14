#include "room_paths.h"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>

using namespace std;

bool try_straight_successor(const MapInfo &map_info, xyLoc xy_loc, int delta_x, int delta_y, int room_id,
                            double &distance, xyLoc &next_location) {
    distance = 1;
    next_location.x = xy_loc.x + delta_x;
    next_location.y = xy_loc.y + delta_y;
    return map_info.get_room(next_location.x, next_location.y) == room_id;
}

bool try_diagonal_successor(const MapInfo &map_info, xyLoc xy_loc, int delta_x, int delta_y, int room_id,
                            double &distance, xyLoc &next_location) {
    distance = SQUARE_TWO;
    next_location.x = xy_loc.x + delta_x;
    next_location.y = xy_loc.y + delta_y;

    xyLoc straight_loc1;
    straight_loc1.x = xy_loc.x + delta_x;
    straight_loc1.y = xy_loc.y;

    xyLoc straight_loc2;
    straight_loc2.x = xy_loc.x;
    straight_loc2.y = xy_loc.y + delta_y;

    return (map_info.get_room(next_location.x, next_location.y) == room_id &&
            !map_info.get_occupied(straight_loc1.x, straight_loc1.y) &&
            !map_info.get_occupied(straight_loc2.x, straight_loc2.y));
}

void compute_greedy_room_path(const MapInfo &map_info, xyLoc start, xyLoc goal,
                              function<void(xyLoc, int)> handle_successor) {
    int room_id = map_info.get_room(start.x, start.y);
    assert(map_info.get_room(goal.x, goal.y) == room_id);
    xyLoc current = start;
    while (true) {
        double distance;
        xyLoc next_location;
        int dx = (current.x < goal.x)? 1 : (current.x > goal.x)? -1 : 0;
        int dy = (current.y < goal.y)? 1 : (current.y > goal.y)? -1 : 0;
        if (dx == 0 && dy == 0)
            return;

        bool found = false;
        if (dx != 0 && dy != 0)
            found = try_diagonal_successor(map_info, current, dx, dy, room_id, distance, next_location);
        if (!found && dx != 0)
            found = try_straight_successor(map_info, current, dx,  0, room_id, distance, next_location);
        if (!found && dy != 0)
            found = try_straight_successor(map_info, current, 0,  dy, room_id, distance, next_location);
        // With the way we construct rooms, one such successor should always exist.
        assert(found);
        handle_successor(next_location, distance);
        current = next_location;
    }
}

vector<xyLoc> compute_room_path(const MapInfo &map_info, xyLoc start, xyLoc goal) {
    vector<xyLoc> path(1, start);
    compute_greedy_room_path(
        map_info, start, goal,
        [&](xyLoc next_location, double /*distance*/) {
            path.push_back(next_location);
        });
    return path;
}

double compute_room_path_cost(const MapInfo &map_info, xyLoc start, xyLoc goal) {
    double cost = 0;
    compute_greedy_room_path(
        map_info, start, goal,
        [&](xyLoc /*next_location*/, double distance) {
            cost += distance;
        });
    return cost;
}
