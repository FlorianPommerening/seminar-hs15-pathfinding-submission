#include "room_heuristic_precomputation.h"

#include "pair_open_list.h"

#include <iostream>
#include <limits>
#include <vector>

using namespace std;


void compute_room_distances(MapInfo &map_info, int room_id) {
    vector<bool> seen(map_info.exits.size(), false);
    vector<double> exit_distance(seen.size(), numeric_limits<double>::max());

    PairOpenList q;
    for (Exit &e : map_info.exits) {
        if (e.room_id == room_id) {
            q.insert(0, e.id);
            exit_distance[e.id] = 0;
        }
    }

    vector<double> &shortest_distance = map_info.shortest_distances_to_room[room_id];
    shortest_distance.resize(map_info.num_rooms, numeric_limits<double>::max());
    shortest_distance[room_id] = 0;

    while (!q.empty()) {
        OpenListEntry next = q.pop_min();
        double cost = next.first;
        int id = next.second;
        if (exit_distance[id] != cost)
            continue;

        Exit &e = map_info.exits[id];
        shortest_distance[e.room_id] = min(shortest_distance[e.room_id], cost);
        seen[id] = true;

        for (ExitSuccesor &succ : e.successors) {
            if (seen[succ.id])
                continue;
            double new_exit_distance = cost + succ.distance;
            if (new_exit_distance < exit_distance[succ.id]) {
                exit_distance[succ.id] = new_exit_distance;
                q.insert(exit_distance[succ.id], succ.id);
            }
        }
    }
}


void compute_room_heuristic(MapInfo &map_info) {
    map_info.shortest_distances_to_room.resize(map_info.num_rooms);
    for (int room_id = 0; room_id < map_info.num_rooms; ++room_id) {
        cout << "computing distances of room " << room_id << endl;
        compute_room_distances(map_info, room_id);
    }
}
