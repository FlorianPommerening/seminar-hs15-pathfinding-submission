/*
  Class for an open list which orders nodes based on their g values (=
  path costs.)

  We implement this as a regular heap (priority queue), which means
  that all relevant operations take O(log N) time were N is the number
  of elements currently in the heap.
*/

#ifndef G_BASED_OPEN_LIST_H
#define G_BASED_OPEN_LIST_H

#include <queue>
#include <vector>
#include <stdint.h>
#include <cmath>

struct xyLoc {
    int16_t x;
    int16_t y;
    double g_value;
};

// extern std::vector<double> visited;
// extern int GetIndex(xyLoc s);

class GValueComparator {
public:
    bool operator()(const xyLoc s1, const xyLoc s2) const {
	//return visited[GetIndex(s1)] > visited[GetIndex(s2)];
	return s1.g_value > s2.g_value;
	//return std::abs(s1.g_value - s2.g_value) > 0.000001;
    }
};

class GBasedOpenList {
    std::priority_queue<xyLoc, std::vector<xyLoc>, GValueComparator> heap;
public:
    GBasedOpenList();
    ~GBasedOpenList();

    bool empty() const;
    void insert(xyLoc s);
    xyLoc pop_min();
};

#endif
