#include "g_based_open_list.h"

using namespace std;


GBasedOpenList::GBasedOpenList() {
}


GBasedOpenList::~GBasedOpenList() {
}


bool GBasedOpenList::empty() const {
    return heap.empty();
}


xyLoc GBasedOpenList::pop_min() {
    xyLoc result = heap.top();
    heap.pop();
    return result;
}


void GBasedOpenList::insert(xyLoc s) {
    heap.push(s);
}
