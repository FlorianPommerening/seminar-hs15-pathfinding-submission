#include "search_algorithm_exits.h"

#include "pair_open_list.h"
#include "room_paths.h"
#include "search_space.h"
#include "debug.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <sstream>

using namespace std;

bool SearchAlgorithmExits::search() {
    SearchSpace search_space(map_info);
    const int GOAL_ID = search_space.get_goal_node_id();
    PairOpenList q;
    goal_room_id = map_info.get_room(g_loc.x, g_loc.y);
    add_successors_of_start(search_space, q);

    while (!q.empty()) {
        pair<double,int> next = q.pop_min();
        int node_id = next.second;

        if (node_id == GOAL_ID) {
            extract_path(search_space);
            return true;
        }

        SearchNode &node = search_space.get_node(node_id);

        if (node.status == NodeStatus::CLOSED) {
            continue;
        }
        if (node.f_value != next.first) {
            continue;
        }
        node.close();

        Exit e = map_info.exits[node_id];
        for (ExitSuccesor succ : e.successors) {
            if (succ.id == node_id) {
                continue;
            }

            /*
            // Not needed if room heuristic is used
            if (succ.dead_end_unless_goal_room_id >= 0 &&
                succ.dead_end_unless_goal_room_id != goal_room_id) {
                continue;
            }
            */

            double succ_g = node.g_value + succ.distance;
            SearchNode &succ_node = search_space.get_node(succ.id);
            if (succ_node.status == NodeStatus::UNINITIALIZED ||
                succ_g + EPSILON < succ_node.g_value) {
                double succ_h = 0;
                succ_h = get_heuristic_value(map_info.exits[succ.id]);
                succ_node.open(node_id, succ_g, succ_h);
                q.insert(succ_node.f_value, succ.id);
            }
        }

        if (e.room_id == goal_room_id) {
            double succ_g = node.g_value + compute_room_path_cost(map_info, e.location, g_loc);
            SearchNode &succ_node = search_space.get_node(GOAL_ID);
            if (succ_node.status == NodeStatus::UNINITIALIZED ||
                succ_g < succ_node.g_value) {
                succ_node.open(node_id, succ_g, 0);
                q.insert(succ_node.f_value, GOAL_ID);
            }
        }
    }
    return true; // no path returned, but we're done
}

void SearchAlgorithmExits::add_successors_of_start(SearchSpace &search_space, PairOpenList &q) {
    int initial_node_id = search_space.get_initial_node_id();
    int goal_node_id = search_space.get_goal_node_id();
    int initial_room_id = map_info.get_room(s_loc.x, s_loc.y);
    for (int exit_id : map_info.room_exits[initial_room_id]) {
        const Exit &e = map_info.exits[exit_id];
        double g = compute_room_path_cost(map_info, s_loc, e.location);
        SearchNode &node = search_space.get_node(exit_id);
        int h = get_heuristic_value(e);
        node.open(initial_node_id, g, h);
        q.insert(node.f_value, exit_id);
    }

    if (initial_room_id == goal_room_id) {
        double g = compute_room_path_cost(map_info, s_loc, g_loc);
        SearchNode &node = search_space.get_node(goal_node_id);
        node.open(initial_node_id, g, 0);
        q.insert(node.f_value, goal_node_id);
    }
}

double SearchAlgorithmExits::get_heuristic_value(const Exit &e) const {
    int dx = abs(e.location.x - g_loc.x);
    int dy = abs(e.location.y - g_loc.y);
    double octile = SQUARE_TWO * (min(dx,dy)) + abs(dx - dy);

    return max(room_heuristic[e.room_id], octile);
}

void SearchAlgorithmExits::extend_path(xyLoc next_loc) {
    xyLoc last_loc = path.back();
    if (map_info.get_room(last_loc.x, last_loc.y) == map_info.get_room(next_loc.x, next_loc.y)) {
        vector<xyLoc> room_path = compute_room_path(map_info, last_loc, next_loc);
        path.insert(path.end(), room_path.begin() + 1, room_path.end());
    } else {
        path.push_back(next_loc);
    }
}

void SearchAlgorithmExits::extract_path(SearchSpace &search_space) {
    int current_id = search_space.get_goal_node_id();
    path.push_back(g_loc);
    SearchNode &current_node = search_space.get_node(current_id);
    while (current_node.parent_id != search_space.get_initial_node_id()) {
        xyLoc next_loc = map_info.exits[current_node.parent_id].location;
        extend_path(next_loc);

        current_id = current_node.parent_id;
        current_node = search_space.get_node(current_id);
    }
    extend_path(s_loc);

    reverse(path.begin(), path.end());

    /*
    // HACK
    static int num_path = 0;
    ostringstream oss;
    oss << "expanded_" << num_path++ << ".png";
    draw_path(oss.str(), map_info, search_space, path);
    */
}

