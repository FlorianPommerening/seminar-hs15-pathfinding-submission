#include <deque>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "Entry.h"

using namespace std;


struct MapInfo {
    vector<bool> map;
    int width;
    int height;
};


const char *GetName() {
    return "TestProgram";
}


void PreprocessMap(vector<bool> &bits, int width, int height, const char *filename) {
    printf("Not writing to file '%s'\n", filename);
}


void *PrepareForSearch(vector<bool> &bits, int w, int h, const char *filename) {
    printf("Not reading from file '%s'\n", filename);

    // Note: this is a memory leak, but the infrastructure code gives us no choice.
    MapInfo *map_info = new MapInfo;
    map_info->map = bits;
    map_info->width = w;
    map_info->height = h;
    return map_info;
}


static const double SQUARE_TWO = 1.4142;
static const double EPSILON = 10e-8;


class SearchAlgorithm {
    const MapInfo *map_info;
    const xyLoc s_loc;
    const xyLoc g_loc;
    vector<xyLoc> &path;
public:
    SearchAlgorithm(MapInfo *map_info, xyLoc s_loc, xyLoc g_loc,
                    vector<xyLoc> &path)
        : map_info(map_info), s_loc(s_loc), g_loc(g_loc), path(path) {
    }

    bool search() {
        Node s;
        s.xy_loc = s_loc;
        s.g_value = 0;

        vector<double> visited;

        GBasedOpenList q;

        if (path.size() > 0) {
            path.push_back(g_loc);
            return true;
        }

        visited.assign(map_info->map.size(),-1);
        q.insert(s);

        while (!q.empty()) {
            Node next = q.pop_min();
            xyLoc next_loc = next.xy_loc;

            if (visited[GetIndex(next_loc)] > -1) {
                continue;
            }

            visited[GetIndex(next_loc)] = next.g_value; //mark as visited

            if (next_loc.x == g_loc.x && next_loc.y == g_loc.y) { // goal found
                ExtractPath(next_loc, visited, path);
                if (path.size() > 0) {
                    path.pop_back();
                    return false;
                }
                return true; // empty path
            }

            vector<Node> succ;
            succ.reserve(8);

            GetSuccessors(next_loc, succ);
            for (unsigned int x = 0; x < succ.size(); x++) {
                succ[x].g_value = next.g_value + 1;
                q.insert(succ[x]);
            }

            Get_Diagonal_Successors(next_loc, succ);
            for (unsigned int x = 0; x < succ.size(); x++) {
                succ[x].g_value = next.g_value + SQUARE_TWO;
                q.insert(succ[x]);
            }
        }
        return true; // no path returned, but we're done
    }

private:
    int GetIndex(xyLoc s) {
        return s.y*map_info->width+s.x;
    }


    // generates 4-connected neighbors
    // doesn't generate 8-connected neighbors (which ARE allowed)
    // a diagonal move must have both cardinal neighbors free to be legal

    void GetSuccessors(xyLoc s, vector<Node> &neighbors) {
        neighbors.resize(0);

        xyLoc next = s;
        next.x++;
        if (next.x < map_info->width && map_info->map[GetIndex(next)])
            neighbors.push_back(Node(next));

        next = s;
        next.x--;
        if (next.x >= 0 && map_info->map[GetIndex(next)])
            neighbors.push_back(Node(next));

        next = s;
        next.y--;
        if (next.y >= 0 && map_info->map[GetIndex(next)])
            neighbors.push_back(Node(next));

        next = s;
        next.y++;
        if (next.y < map_info->height && map_info->map[GetIndex(next)])
            neighbors.push_back(Node(next));
    }


    // generates the diagonal successors of s
    void Get_Diagonal_Successors(xyLoc s, vector<Node> &neighbors) {
        neighbors.resize(0);

        xyLoc next = s;
        next.x++;
        if (next.x < map_info->width && map_info->map[GetIndex(next)]) {
            next.y++;
            xyLoc temp = next;
            temp.x--;

            if (next.y < map_info->height && map_info->map[GetIndex(next)] && map_info->map[GetIndex(temp)]) {
                neighbors.push_back(Node(next));
            }
            next.y = next.y-2;
            temp.y = temp.y-2;
            if (next.y >= 0 && map_info->map[GetIndex(next)] && map_info->map[GetIndex(temp)]) {
                neighbors.push_back(Node(next));
            }
        }

        next = s;
        next.x--;
        if (next.x >= 0 && map_info->map[GetIndex(next)]) {
            next.y++;
            xyLoc temp = next;
            temp.x++;

            if (next.y < map_info->height && map_info->map[GetIndex(next)] && map_info->map[GetIndex(temp)]) {
                neighbors.push_back(Node(next));
            }
            next.y = next.y-2;
            temp.y = temp.y-2;
            if (next.y >= 0 && map_info->map[GetIndex(next)] && map_info->map[GetIndex(temp)]) {
                neighbors.push_back(Node(next));
            }
        }
    }


    void ExtractPath(xyLoc end, const vector<double> &visited, vector<xyLoc> &finalPath) {
        double currCost = visited[GetIndex(end)];

        // cout << "extracting path... " << endl;

        // cout << "visited-value: " << currCost << endl;
        // cout << "g-value: " << end.g_value << endl;

        finalPath.resize(0);
        finalPath.push_back(end);

        bool found = false;

        // cout << "currCost: " << currCost << endl;

        while (abs(currCost) > EPSILON) {
            found = false;
            vector<Node> succ;
            succ.reserve(8);

            Get_Diagonal_Successors(finalPath.back(), succ);

            for (unsigned int x = 0; x < succ.size(); x++) {
                //cout << visited[GetIndex(succ[x])] << endl;

                if (abs(currCost - visited[GetIndex(succ[x].xy_loc)] - SQUARE_TWO) < EPSILON) {
                    finalPath.push_back(succ[x].xy_loc);
                    currCost = currCost - SQUARE_TWO;
                    found = true;
                    break;
                }
            }

            if (!found) {
                GetSuccessors(finalPath.back(), succ);

                for (unsigned int x = 0; x < succ.size(); x++) {
                    if (abs(currCost - visited[GetIndex(succ[x].xy_loc)] -1) < EPSILON) {
                        finalPath.push_back(succ[x].xy_loc);
                        currCost--;
                        break;
                    }
                }
            }
        }
        reverse(finalPath.begin(), finalPath.end());
    }

};


bool GetPath(void *data, xyLoc s_loc, xyLoc g_loc, vector<xyLoc> &path) {
    MapInfo *map_info = static_cast<MapInfo *>(data);
    SearchAlgorithm algo(map_info, s_loc, g_loc, path);
    return algo.search();
}
