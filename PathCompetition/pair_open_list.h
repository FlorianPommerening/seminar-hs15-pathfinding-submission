#ifndef PAIR_OPEN_LIST_H
#define PAIR_OPEN_LIST_H

#include <queue>
#include <utility>
#include <vector>

using OpenListEntry = std::pair<double, int>;

class PairOpenList {
    std::priority_queue<OpenListEntry,
                        std::vector<OpenListEntry>,
                        std::greater<OpenListEntry>> heap;
public:
    inline bool empty() const {
        return heap.empty();
    }
	
    inline void insert(double f, int id) {
        heap.push(std::make_pair(f, id));
    }
    
    inline OpenListEntry pop_min() {
        OpenListEntry result = heap.top();
        heap.pop();
        return result;
    }
};

#endif
