/*
  Class for an open list which orders nodes based on their f values (=
  path costs + heuristic value.)

  We implement this as a regular heap (priority queue), which means
  that all relevant operations take O(log N) time were N is the number
  of elements currently in the heap.
*/

#ifndef F_BASED_OPEN_LIST_H
#define F_BASED_OPEN_LIST_H

#include <queue>
#include <vector>
#include <stdint.h>


struct xyLoc {
    int16_t x;
    int16_t y;
};


struct Node {
    xyLoc xy_loc;
    double g_value;
    double f_value;

    Node() {}

    Node(xyLoc xy_loc) : xy_loc(xy_loc) {}

    bool operator<(const Node &other) const {
	return f_value > other.f_value;
    }
};


class FBasedOpenList {
    std::priority_queue<Node, std::vector<Node> > heap;
public:
    FBasedOpenList();
    ~FBasedOpenList();

    inline bool empty() const {
	return heap.empty();
    };
	
    inline void insert(const Node &n) {
	heap.push(n);
    };
    
    inline Node pop_min() {
	Node result = heap.top();
	heap.pop();
	return result;
    };
};

#endif
