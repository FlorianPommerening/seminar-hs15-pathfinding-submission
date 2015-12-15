#include "map_info.h"

using namespace std;

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
    os << s.id << " " << s.distance << endl;
}

istream &operator>>(istream &is, ExitSuccesor &s) {
    is >> s.id >> s.distance;
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

    os << m.room_exits.size() << endl;
    for (const auto &r_exits : m.room_exits) {
        os << r_exits.size() << " ";
        for (int exit_id : r_exits) {
            os << exit_id << " ";
        }
        os << endl;
    }
    os << endl;

    os << m.exits.size() << endl;
    for (const auto &e : m.exits) {
        os << e;
    }
    os << endl;

    os << m.room_successors.size() << endl;
    for (const auto &r_successors : m.room_successors) {
        os << r_successors.size() << " ";
        for (int succ_id : r_successors) {
            os << succ_id << " ";
        }
        os << endl;
    }
    os << endl;
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

    int num_room_exits;
    is >> num_room_exits;
    m.room_exits.resize(num_room_exits);
    for (int i = 0; i < num_room_exits; ++i) {
        int num_room_exits_i;
        is >> num_room_exits_i;
        m.room_exits[i].resize(num_room_exits_i);
        for (int j = 0; j < num_room_exits_i; ++j) {
            is >> m.room_exits[i][j];
        }
    }

    int num_exits;
    is >> num_exits;
    m.exits.resize(num_exits);
    for (int i = 0; i < num_exits; ++i) {
        is >> m.exits[i];
    }

    int num_room_successors;
    is >> num_room_successors;
    m.room_successors.resize(num_room_successors);
    for (int i = 0; i < num_room_successors; ++i) {
        int num_room_successors_i;
        is >> num_room_successors_i;
        m.room_successors[i].resize(num_room_successors_i);
        for (int j = 0; j < num_room_successors_i; ++j) {
            is >> m.room_successors[i][j];
        }
    }

    return is;
}
