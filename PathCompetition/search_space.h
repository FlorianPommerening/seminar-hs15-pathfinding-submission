#ifndef SEARCH_SPACE_H
#define SEARCH_SPACE_H

#include "locations.h"
#include "map_info.h"

#include <vector>

enum class NodeStatus {
    UNINITIALIZED = 0,
    OPEN,
    CLOSED,
};

struct SearchNode {
    NodeStatus status;
    int parent_id;
    double g_value;
    double f_value;

    SearchNode()
        : status(NodeStatus::UNINITIALIZED) {
    }

    void open_initial(int initial_id) {
        status = NodeStatus::OPEN;
        parent_id = initial_id;
    }

    void open(int parent_id_, double g, double h) {
        status = NodeStatus::OPEN;
        parent_id = parent_id_;
        g_value = g;
        f_value = g + h;
    }

    void reopen(int parent_id_, double g) {
        parent_id = parent_id_;
        f_value = f_value - g_value + g;
        g_value = g;
    }

    void close() {
        status = NodeStatus::CLOSED;
    }
};

class SearchSpace {
    const int initial_node_id;
    const int goal_node_id;
    std::vector<SearchNode> nodes;
public:
    SearchSpace(const MapInfo &MapInfo);

    SearchNode &get_node(int id) {
        return nodes[id];
    }

    int get_initial_node_id() {
        return initial_node_id;
    }

    int get_goal_node_id() {
        return goal_node_id;
    }
};

#endif
