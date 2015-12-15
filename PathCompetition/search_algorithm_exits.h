#ifndef SEARCHALGORITHMEXITS_H
#define SEARCHALGORITHMEXITS_H

#include "map_info.h"

#include <vector>

class PairOpenList;
class SearchSpace;

class SearchAlgorithmExits {
    const MapInfo &map_info;
    const xyLoc s_loc;
    const xyLoc g_loc;
    std::vector<xyLoc> &path;

    int goal_room_id;
    std::vector<ExitSuccesor> additional_goal_room_successors;

    void compute_goal_room_paths(int goal_id);
    void add_successors_of_start(SearchSpace &search_space, PairOpenList &q);

    double get_octile_heuristic_value(xyLoc xy_loc) const;
    void add_successors_if_necessary(
        SearchSpace &search_space, PairOpenList &q,
        const std::vector<ExitSuccesor> &successors,
        int parent_id, double parent_g);

    void extend_path(xyLoc next_loc);
    void extract_path(SearchSpace &search_space);

public:
    SearchAlgorithmExits(MapInfo &map_info, xyLoc s_loc, xyLoc g_loc,
                         std::vector<xyLoc> &path)
        : map_info(map_info), s_loc(s_loc), g_loc(g_loc), path(path) {
    }

    bool search();
};


#endif
