#include "search_algorithm_exits.h"

#include "f_based_open_list.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

using namespace std;

bool SearchAlgorithmExits::search() {
    Node s;
    s.xy_loc = s_loc;
    s.g_value = 0;

    vector<double> visited;

    FBasedOpenList q;

    visited.assign(map_info->map.size(),-1);
    q.insert(s);

    vector<Node> succ;
    succ.reserve(8);

    while (!q.empty()) {
        Node next = q.pop_min();
        xyLoc next_loc = next.xy_loc;

        if (visited[GetIndex(next_loc)] > -1) {
            continue;
        }

        visited[GetIndex(next_loc)] = next.g_value; //mark as visited

        if (next_loc.x == g_loc.x && next_loc.y == g_loc.y) { // goal found
            ExtractPath(next_loc, visited, path);
            return true;
        }

        succ.clear();
        GetAllSuccessors(next, succ);

        for (int i = 0 ; i < succ.size(); ++i) {
            succ[i].f_value = succ[i].g_value + get_octile_heuristic_value(succ[i]);
            if (visited[GetIndex(succ[i].xy_loc)] == -1) {
                q.insert(succ[i]);
            }
        }

    }
    return true; // no path returned, but we're done
}

double SearchAlgorithmExits::get_octile_heuristic_value(Node& succ) const {
    xyLoc xy_loc = succ.xy_loc;
    int dx = abs(xy_loc.x - g_loc.x);
    int dy = abs(xy_loc.y - g_loc.y);

    return SQUARE_TWO * (min(dx,dy)) + abs(dx - dy);
}

void SearchAlgorithmExits::GetAllSuccessors(const Node &node, vector<Node> &succ) {
    try_straight_successor(node, +1, 0, succ);
    try_straight_successor(node, -1, 0, succ);
    try_straight_successor(node, 0, +1, succ);
    try_straight_successor(node, 0, -1, succ);

    try_diagonal_successor(node, +1, +1, succ);
    try_diagonal_successor(node, +1, -1, succ);
    try_diagonal_successor(node, -1, +1, succ);
    try_diagonal_successor(node, -1, -1, succ);
}

// generate a straight successor (if legal)
void SearchAlgorithmExits::try_straight_successor(const Node &node, int delta_x, int delta_y, vector<Node> &succ) {
    xyLoc new_loc;
    new_loc.x = node.xy_loc.x + delta_x;
    new_loc.y = node.xy_loc.y + delta_y;

    if (new_loc.x >= 0 && new_loc.x < map_info->width &&
        new_loc.y >= 0 && new_loc.y < map_info->height &&
        map_info->map[GetIndex(new_loc)]) {
        Node new_node;
        new_node.xy_loc = new_loc;
        new_node.g_value = node.g_value + 1;
        succ.push_back(new_node);
    }
}

// generate a diagonal successor (if legal)
void SearchAlgorithmExits::try_diagonal_successor(const Node &node, int delta_x, int delta_y, vector<Node> &succ) {
    xyLoc new_loc;
    new_loc.x = node.xy_loc.x + delta_x;
    new_loc.y = node.xy_loc.y + delta_y;

    xyLoc straight_loc1;
    straight_loc1.x = node.xy_loc.x + delta_x;
    straight_loc1.y = node.xy_loc.y;

    xyLoc straight_loc2;
    straight_loc2.x = node.xy_loc.x;
    straight_loc2.y = node.xy_loc.y + delta_y;

    if (new_loc.x >= 0 && new_loc.x < map_info->width &&
        new_loc.y >= 0 && new_loc.y < map_info->height &&
        map_info->map[GetIndex(new_loc)] &&
        map_info->map[GetIndex(straight_loc1)] &&
        map_info->map[GetIndex(straight_loc2)]) {
        Node new_node;
        new_node.xy_loc = new_loc;
        new_node.g_value = node.g_value + SQUARE_TWO;
        succ.push_back(new_node);
    }
}

void SearchAlgorithmExits::ExtractPath(xyLoc end, const vector<double> &visited, vector<xyLoc> &finalPath) {
    double currCost = visited[GetIndex(end)];

    finalPath.resize(0);
    finalPath.push_back(end);

    while (abs(currCost) > EPSILON) {
        vector<Node> succ;
        succ.reserve(8);

        GetAllSuccessors(finalPath.back(), succ);

        for (unsigned int x = 0; x < succ.size(); x++) {
            if (abs(currCost - visited[GetIndex(succ[x].xy_loc)] - SQUARE_TWO) < EPSILON) {
                finalPath.push_back(succ[x].xy_loc);
                currCost = currCost - SQUARE_TWO;
        break;
            }

    if (abs(currCost - visited[GetIndex(succ[x].xy_loc)] -1) < EPSILON) {
        finalPath.push_back(succ[x].xy_loc);
        currCost--;
        break;
    }
        }
    }
    reverse(finalPath.begin(), finalPath.end());
}

