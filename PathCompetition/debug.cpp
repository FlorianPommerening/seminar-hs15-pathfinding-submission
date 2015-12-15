#include "debug.h"

#include <png++/png.hpp>

using namespace std;

// From http://godsnotwheregodsnot.blogspot.ru/2012/09/color-distribution-methodology.html
png::rgb_pixel DISSIMILAR_COLORS[63] = {
    png::rgb_pixel(1, 0, 103),
    png::rgb_pixel(213, 255, 0),
    png::rgb_pixel(255, 0, 86),
    png::rgb_pixel(158, 0, 142),
    png::rgb_pixel(14, 76, 161),
    png::rgb_pixel(255, 229, 2),
    png::rgb_pixel(0, 95, 57),
    png::rgb_pixel(0, 255, 0),
    png::rgb_pixel(149, 0, 58),
    png::rgb_pixel(255, 147, 126),
    png::rgb_pixel(164, 36, 0),
    png::rgb_pixel(0, 21, 68),
    png::rgb_pixel(145, 208, 203),
    png::rgb_pixel(98, 14, 0),
    png::rgb_pixel(107, 104, 130),
    png::rgb_pixel(0, 0, 255),
    png::rgb_pixel(0, 125, 181),
    png::rgb_pixel(106, 130, 108),
    png::rgb_pixel(0, 174, 126),
    png::rgb_pixel(194, 140, 159),
    png::rgb_pixel(190, 153, 112),
    png::rgb_pixel(0, 143, 156),
    png::rgb_pixel(95, 173, 78),
    png::rgb_pixel(255, 0, 0),
    png::rgb_pixel(255, 0, 246),
    png::rgb_pixel(255, 2, 157),
    png::rgb_pixel(104, 61, 59),
    png::rgb_pixel(255, 116, 163),
    png::rgb_pixel(150, 138, 232),
    png::rgb_pixel(152, 255, 82),
    png::rgb_pixel(167, 87, 64),
    png::rgb_pixel(1, 255, 254),
    png::rgb_pixel(255, 238, 232),
    png::rgb_pixel(254, 137, 0),
    png::rgb_pixel(189, 198, 255),
    png::rgb_pixel(1, 208, 255),
    png::rgb_pixel(187, 136, 0),
    png::rgb_pixel(117, 68, 177),
    png::rgb_pixel(165, 255, 210),
    png::rgb_pixel(255, 166, 254),
    png::rgb_pixel(119, 77, 0),
    png::rgb_pixel(122, 71, 130),
    png::rgb_pixel(38, 52, 0),
    png::rgb_pixel(0, 71, 84),
    png::rgb_pixel(67, 0, 44),
    png::rgb_pixel(181, 0, 255),
    png::rgb_pixel(255, 177, 103),
    png::rgb_pixel(255, 219, 102),
    png::rgb_pixel(144, 251, 146),
    png::rgb_pixel(126, 45, 210),
    png::rgb_pixel(189, 211, 147),
    png::rgb_pixel(229, 111, 254),
    png::rgb_pixel(222, 255, 116),
    png::rgb_pixel(0, 255, 120),
    png::rgb_pixel(0, 155, 255),
    png::rgb_pixel(0, 100, 1),
    png::rgb_pixel(0, 118, 255),
    png::rgb_pixel(133, 169, 0),
    png::rgb_pixel(0, 185, 23),
    png::rgb_pixel(120, 130, 49),
    png::rgb_pixel(0, 255, 198),
    png::rgb_pixel(255, 110, 65),
    png::rgb_pixel(232, 94, 190)
};

void draw_rooms(string filename, const MapInfo &map_info) {
    png::image<png::rgb_pixel> image(map_info.width, map_info.height);

    for (int x = 0; x < map_info.width; ++x) {
        for (int y = 0; y < map_info.height; ++y) {
            int room_id = map_info.get_room(x, y);
            image.set_pixel(x, y, DISSIMILAR_COLORS[room_id % 63]);
        }
    }
    image.write(filename);
}

void draw_path(string filename, const MapInfo &map_info, SearchSpace &search_space, vector<xyLoc> &path) {
    png::image<png::rgb_pixel> image(map_info.width, map_info.height);

    for (int x = 0; x < map_info.width; ++x) {
        for (int y = 0; y < map_info.height; ++y) {
            if (map_info.get_occupied(x, y)) {
                image.set_pixel(x, y, png::rgb_pixel(0,0,0));
            } else {
                image.set_pixel(x, y, png::rgb_pixel(255,255,255));
            }
        }
    }

    int num_uninitialized = 0;
    int num_open = 0;
    int num_closed = 0;
    for (const Exit &e : map_info.exits) {
        SearchNode &node = search_space.get_node(e.id);
        if (node.status == NodeStatus::UNINITIALIZED) {
            ++num_uninitialized;
            image.set_pixel(e.location.x, e.location.y, png::rgb_pixel(0,255,0));
        } else if (node.status == NodeStatus::OPEN) {
            ++num_open;
            image.set_pixel(e.location.x, e.location.y, png::rgb_pixel(255,0,0));
        } else {
            ++num_closed;
            image.set_pixel(e.location.x, e.location.y, png::rgb_pixel(0,0,255));
        }
    }
    cout << "Uninitilized " << num_uninitialized << endl;
    cout << "Open         " << num_open << endl;
    cout << "Closed       " << num_closed << endl;

    for (xyLoc l : path) {
        image.set_pixel(l.x, l.y, png::rgb_pixel(255,0,255));
    }

    image.write(filename);
}
