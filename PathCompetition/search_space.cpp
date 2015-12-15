#include "search_space.h"

#include <algorithm>

using namespace std;

SearchSpace::SearchSpace(const MapInfo &map_info)
    : nodes(map_info.exits.size() + 2),
      initial_node_id(map_info.exits.size()),
      goal_node_id(map_info.exits.size() + 1) {
}

