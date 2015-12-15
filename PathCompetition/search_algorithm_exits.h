#ifndef SEARCHALGORITHMEXITS_H
#define SEARCHALGORITHMEXITS_H

#include "map_info.h"

#include <unordered_map>
#include <vector>

class PairOpenList;
class SearchSpace;

class SearchAlgorithmExits {
    const MapInfo &map_info;
    const xyLoc s_loc;
    const xyLoc g_loc;
    std::vector<xyLoc> &path;

    int goal_room_id;
    std::unordered_map<int, ExitSuccesor> additional_goal_room_successors;

    std::vector<double> &room_heuristic;

    void add_successors_of_start(SearchSpace &search_space, PairOpenList &q);

    double get_heuristic_value(const Exit &e) const;

    void extend_path(xyLoc next_loc);
    void extract_path(SearchSpace &search_space);

public:
    SearchAlgorithmExits(MapInfo &map_info, xyLoc s_loc, xyLoc g_loc,
                         std::vector<xyLoc> &path)
        : map_info(map_info),
          s_loc(s_loc),
          g_loc(g_loc),
          path(path),
          room_heuristic(map_info.shortest_distances_to_room[map_info.get_room(g_loc.x, g_loc.y)]) {
    }

    bool search();
};


#endif
