#ifndef PAIR_OPEN_LIST_H
#define PAIR_OPEN_LIST_H

#include <cassert>
#include <queue>
#include <utility>
#include <map>
#include <unordered_set>
#include <vector>

using OpenListEntry = std::pair<double, int>;

class PairOpenList {
    std::priority_queue<OpenListEntry,
                        std::vector<OpenListEntry>,
                        std::greater<OpenListEntry>> heap;
    std::map<int, std::unordered_set<int>> levels;
    std::vector<double> values;
    int current_level;
public:
    PairOpenList(int num_elems)
        : values(num_elems, 0),
          current_level(0) {
    }

    inline bool empty() const {
        return heap.empty() && levels.empty();
    }
	
    inline void insert(double f, int id) {
        int level = static_cast<int>(f);
        if (level <= current_level) {
            heap.push(std::make_pair(f, id));
        } else {
            levels[level].insert(id);
            values[id] = f;
        }
    }
    
    inline void decrease_key(int id, double new_f) {
        int new_level = static_cast<int>(new_f);
        if (new_level <= current_level) {
            heap.push(std::make_pair(new_f, id));
        } else {
            int old_level = static_cast<int>(values[id]);
            if (new_level != old_level) {
                levels[old_level].erase(id);
                levels[new_level].insert(id);
            }
            values[id] = new_f;
        }
    }

    inline OpenListEntry pop_min() {
        if (heap.empty()) {
            const auto &level = *levels.begin();
            current_level = level.first;
            for (int i : level.second) {
                heap.push(std::make_pair(values[i], i));
            }
            levels.erase(levels.begin());
        }
        OpenListEntry result = heap.top();
        heap.pop();
        return result;
    }
};

#endif
