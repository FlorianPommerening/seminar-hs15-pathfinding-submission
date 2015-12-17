#include "gate_detection.h"
#include "room_paths.h"
#include "search_algorithm.h"

#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

class ExitPathComputer {
    MapInfo &map_info;
    vector<vector<int>> position_to_exit_id;
public:
    ExitPathComputer(MapInfo &map_info);
    int get_exit(int x, int y, int room_id);
    void create_transition(int from_exit_id, int to_exit_id, double cost);
    void detect_gates();
};


ExitPathComputer::ExitPathComputer(MapInfo &map_info)
    : map_info(map_info),
      position_to_exit_id(map_info.width, vector<int>(map_info.height, -1)) {
}

int ExitPathComputer::get_exit(int x, int y, int room_id) {
    int exit_id = position_to_exit_id[x][y];
    if (exit_id == -1) {
        exit_id = map_info.exits.size();
        position_to_exit_id[x][y] = exit_id;
        Exit new_exit{exit_id, room_id, xyLoc(x, y)};
        map_info.room_exits[room_id].push_back(exit_id);
        map_info.exits.push_back(new_exit);
    }
    return exit_id;
}

void ExitPathComputer::create_transition(int from_exit_id, int to_exit_id, double cost) {
    Exit &from_exit = map_info.exits[from_exit_id];
    from_exit.successors.push_back({to_exit_id, cost, -1});
}

void ExitPathComputer::detect_gates() {
    map_info.exits.clear();
    map_info.room_exits.clear();
    map_info.room_exits.resize(map_info.num_rooms);

    vector<unordered_set<int>> room_successors(map_info.num_rooms);
    vector<vector<bool>> has_connection(map_info.num_rooms, vector<bool>(map_info.num_rooms, false));

    for (int x = 0; x < map_info.width; ++x) {
        for (int y = 0; y < map_info.height; ++y) {
            if (map_info.get_occupied(x, y))
                continue;
            int current_room_id = map_info.get_room(x, y);
            for (int suc_x = max(0, x - 1); suc_x < min(map_info.width, x + 2); ++suc_x) {
                for (int suc_y = max(0, y - 1); suc_y < min(map_info.height, y + 2); ++suc_y) {
                    if (suc_x == x && suc_y == y)
                        continue;
                    int succ_room_id = map_info.get_room(suc_x, suc_y);
                    if (!map_info.get_occupied(suc_x, suc_y) &&
                            !map_info.get_occupied(x, suc_y) &&
                            !map_info.get_occupied(suc_x, y) &&
                            succ_room_id != current_room_id) {
                        if (!has_connection[current_room_id][succ_room_id]) {
                            has_connection[current_room_id][succ_room_id] = true;
                            int current_exit_id = get_exit(x, y, current_room_id);
                            int succ_exit_id = get_exit(suc_x, suc_y, succ_room_id);
                            double cost = (suc_x == x || suc_y == y)? 1 : SQUARE_TWO;
                            create_transition(current_exit_id, succ_exit_id, cost);
                            room_successors[current_room_id].insert(succ_room_id);
                        }
                    }
                }
            }
        }
    }
    for (int room_id = 0; room_id < map_info.num_rooms; ++room_id) {
        const unordered_set<int> &successors = room_successors[room_id];
        map_info.room_successors.emplace_back(successors.begin(), successors.end());
        if (successors.size() == 1) {
            for (int e_id : map_info.room_exits[room_id]) {
                // Mark transitions coming into this room as potential dead ends
                for (const ExitSuccesor &succ : map_info.exits[e_id].successors) {
                    for (ExitSuccesor &succ_succ : map_info.exits[succ.id].successors) {
                        if (map_info.exits[succ_succ.id].room_id == room_id) {
                            succ_succ.dead_end_unless_goal_room_id = room_id;
                        }
                    }
                }
            }
        }
    }

    cout << "computing room paths for " << map_info.num_rooms << " rooms" << endl;
    // Transitions within room;
    int r_id = 0;
    for (const auto &exit_ids : map_info.room_exits) {
        cout << "computing room paths for room " << r_id++ << "/" << map_info.num_rooms << endl;
        for (int exit_id_1 : exit_ids) {
            for (int exit_id_2 : exit_ids) {
                if (exit_id_1 >= exit_id_2) continue;
                Exit &from_exit = map_info.exits[exit_id_1];
                Exit &to_exit = map_info.exits[exit_id_2];

                double cost_shortest_path_in_room =
                    compute_room_path_cost(map_info, from_exit.location, to_exit.location);

                create_transition(exit_id_1, exit_id_2, cost_shortest_path_in_room);
                create_transition(exit_id_2, exit_id_1, cost_shortest_path_in_room);
            }
        }
    }
}


void detect_gates(MapInfo &map_info) {
    ExitPathComputer(map_info).detect_gates();
}
