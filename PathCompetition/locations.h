#ifndef LOCATIONS
#define LOCATIONS

#include <stdint.h>

struct xyLoc {
    int16_t x;
    int16_t y;

    xyLoc() {
    }
    xyLoc(int16_t x, int16_t y)
        : x(x), y(y) {
    }
};

#endif
