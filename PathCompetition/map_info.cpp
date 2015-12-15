#include "map_info.h"

using namespace std;

template<typename T>
ostream &operator<<(ostream &os, const vector<vector<T>> &lists) {
    os << lists.size() << endl;
    for (const auto &list : lists) {
        os << list.size() << " ";
        for (const auto &value : list) {
            os << value << " ";
        }
        os << endl;
    }
    os << endl;
}

template<typename T>
istream &operator>>(istream &is, vector<vector<T>> &lists) {
    int num_lists;
    is >> num_lists;
    lists.resize(num_lists);
    for (int i = 0; i < num_lists; ++i) {
        int num_list_i;
        is >> num_list_i;
        lists[i].resize(num_list_i);
        for (int j = 0; j < num_list_i; ++j) {
            is >> lists[i][j];
        }
    }
}

ostream &operator<<(ostream &os, const Exit &e) {
    os << e.id << " "
       << e.room_id << " "
       << e.location.x << " "
       << e.location.y << " "
       << e.successors.size() << endl;
    for (int i = 0; i < e.successors.size(); ++i) {
        os << e.successors[i];
    }
}

istream &operator>>(istream &is, Exit &e) {
    int num_succ;
    is >> e.id >> e.room_id >> e.location.x >> e.location.y >> num_succ;
    e.successors.resize(num_succ);
    for (int i = 0; i < num_succ; ++i) {
        is >> e.successors[i];
    }
}

ostream &operator<<(ostream &os, const ExitSuccesor &s) {
    os << s.id << " " << s.distance << " " << s.dead_end_unless_goal_room_id << endl;
}

istream &operator>>(istream &is, ExitSuccesor &s) {
    is >> s.id >> s.distance >> s.dead_end_unless_goal_room_id;
}

ostream &operator<<(ostream& os, const MapInfo &m) {
    os << m.width << " " << m.height;
    for (int i = 0; i < m.map.size(); ++i) {
        if (i % m.width == 0)
            os << endl;
        os << (m.map[i]? ".":"@");
    }
    os << endl;

    os << m.num_rooms;
    for (int i = 0; i < m.rooms.size(); ++i) {
        if (i % m.width == 0)
            os << endl;
        os << m.rooms[i] << " ";
    }
    os << endl;

    os << m.room_exits;

    os << m.exits.size() << endl;
    for (const auto &e : m.exits) {
        os << e;
    }
    os << endl;

    os << m.room_successors;
    os << m.shortest_distances_to_room;
    return os;
}

istream &operator>>(istream& is, MapInfo &m) {
    is >> m.width >> m.height;
    m.map.resize(m.width * m.height);
    for (int i = 0; i < m.map.size(); ++i) {
        char map_elem;
        is >> map_elem;
        m.map[i] = (map_elem == '.');
    }

    is >> m.num_rooms;
    m.rooms.resize(m.width * m.height);
    for (int i = 0; i < m.rooms.size(); ++i) {
        is >> m.rooms[i];
    }

    is >> m.room_exits;

    int num_exits;
    is >> num_exits;
    m.exits.resize(num_exits);
    for (int i = 0; i < num_exits; ++i) {
        is >> m.exits[i];
    }

    is >> m.room_successors;
    is >> m.shortest_distances_to_room;

    return is;
}
