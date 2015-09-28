#include "g_based_open_list.h"

using namespace std;


GBasedOpenList::GBasedOpenList() {
}


GBasedOpenList::~GBasedOpenList() {
}


bool GBasedOpenList::empty() const {
    return heap.empty();
}


Node GBasedOpenList::pop_min() {
    Node result = heap.top();
    heap.pop();
    return result;
}


void GBasedOpenList::insert(const Node &n) {
    heap.push(n);
}
