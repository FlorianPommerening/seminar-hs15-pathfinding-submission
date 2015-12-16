#ifndef SEARCHALGORITHM_H
#define SEARCHALGORITHM_H

#include "f_based_open_list.h"
#include "map_info.h"

#include <vector>

class SearchAlgorithm {
    const MapInfo *map_info;
    const xyLoc s_loc;
    const xyLoc g_loc;
    std::vector<xyLoc> &path;
    double &path_cost;
public:
    SearchAlgorithm(MapInfo *map_info, xyLoc s_loc, xyLoc g_loc,
                    std::vector<xyLoc> &path, double &path_cost)
        : map_info(map_info), s_loc(s_loc), g_loc(g_loc), path(path), path_cost(path_cost) {
    }

    bool search();

private:
    int GetIndex(xyLoc s) {
        return s.y * map_info->width + s.x;
    }

    double get_octile_heuristic_value(Node& succ) const;
    void GetAllSuccessors(const Node &node, std::vector<Node> &succ);
    // generate a straight successor (if legal)
    void try_straight_successor(const Node &node, int delta_x, int delta_y, std::vector<Node> &succ);
    // generate a diagonal successor (if legal)
    void try_diagonal_successor(const Node &node, int delta_x, int delta_y, std::vector<Node> &succ);
    void ExtractPath(xyLoc end, const std::vector<double> &visited, std::vector<xyLoc> &finalPath);
};


#endif
