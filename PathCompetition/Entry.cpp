#include <deque>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "Entry.h"

std::vector<bool> map;
std::vector<double> visited;
std::vector<Node> succ;
int width, height;

const double SQUARE_TWO = 1.4142;
const double EPSILON = 10e-8;

using namespace std;

void GetSuccessors(xyLoc s, std::vector<Node> &neighbors);
void Get_Diagonal_Successors(xyLoc s, std::vector<Node> &neighbors);
int GetIndex(xyLoc s);
void ExtractPath(xyLoc end, std::vector<xyLoc> &finalPath);

const char *GetName()
{
	return "TestProgram";
}

void PreprocessMap(std::vector<bool> &bits, int width, int height, const char *filename)
{
	printf("Not writing to file '%s'\n", filename);
}

void *PrepareForSearch(std::vector<bool> &bits, int w, int h, const char *filename)
{
	printf("Not reading from file '%s'\n", filename);
	map = bits;
	width = w;
	height = h;
	return (void *)13182;
}


bool GetPath(void *data, xyLoc s_loc, xyLoc g_loc, std::vector<xyLoc> &path)
{
	assert((long)data == 13182);

        
        Node s;
        s.xy_loc = s_loc;
        s.g_value = 0;

	GBasedOpenList q;

	if (path.size() > 0)
	{
                path.push_back(g_loc);
		return true;
	}

	visited.assign(map.size(),-1);
	q.insert(s);
	
	while (!q.empty())
    {
                Node next = q.pop_min();
                xyLoc next_loc = next.xy_loc;

		if (visited[GetIndex(next_loc)] > -1) { 
		    continue;
		}

		visited[GetIndex(next_loc)] = next.g_value; //mark as visited
		
		if (next_loc.x == g_loc.x && next_loc.y == g_loc.y) // goal found
		    {
			ExtractPath(next_loc, path);
			if (path.size() > 0)
			    {
				path.pop_back();
				return false;
			    }
			return true; // empty path
		    }
		

		GetSuccessors(next_loc, succ);
		for (unsigned int x = 0; x < succ.size(); x++)
		{
		    succ[x].g_value = next.g_value + 1;
		    q.insert(succ[x]);
		}

		Get_Diagonal_Successors(next_loc, succ);
		for (unsigned int x = 0; x < succ.size(); x++)
		{
		    succ[x].g_value = next.g_value + SQUARE_TWO;
		    q.insert(succ[x]);
		}

    }
	return true; // no path returned, but we're done
}

int GetIndex(xyLoc s)
{
	return s.y*width+s.x;
}

// generates 4-connected neighbors
// doesn't generate 8-connected neighbors (which ARE allowed)
// a diagonal move must have both cardinal neighbors free to be legal
void GetSuccessors(xyLoc s, std::vector<Node> &neighbors)
{
	neighbors.resize(0);
  
	xyLoc next = s;
	next.x++;
	if (next.x < width && map[GetIndex(next)])
               neighbors.push_back(Node(next));

	next = s;
	next.x--;
	if (next.x >= 0 && map[GetIndex(next)])
                neighbors.push_back(Node(next));

	next = s;
	next.y--;
	if (next.y >= 0 && map[GetIndex(next)])
                neighbors.push_back(Node(next));

	next = s;
	next.y++;
	if (next.y < height && map[GetIndex(next)])
                neighbors.push_back(Node(next));
}

// generates the diagonal successors of s
void Get_Diagonal_Successors(xyLoc s, std::vector<Node> &neighbors)
{
    neighbors.resize(0);
  
	xyLoc next = s;
	next.x++;
	if (next.x < width && map[GetIndex(next)]) {
		next.y++;
		xyLoc temp = next;
		temp.x--;
		
		if (next.y < height && map[GetIndex(next)] && map[GetIndex(temp)]) {
		    neighbors.push_back(Node(next));
		}
		next.y = next.y-2;
		temp.y = temp.y-2;
		if (next.y >= 0 && map[GetIndex(next)] && map[GetIndex(temp)]) {
		    neighbors.push_back(Node(next));
		}
	}

	next = s;
	next.x--;
	if (next.x >= 0 && map[GetIndex(next)]) {
		next.y++;
		xyLoc temp = next;
		temp.x++;

		if (next.y < height && map[GetIndex(next)] && map[GetIndex(temp)]) {
		    neighbors.push_back(Node(next));
		}
		next.y = next.y-2;
		temp.y = temp.y-2;
		if (next.y >= 0 && map[GetIndex(next)] && map[GetIndex(temp)]) {
		    neighbors.push_back(Node(next));
		}
	}
}

void ExtractPath(xyLoc end, std::vector<xyLoc> &finalPath)
{
	double currCost = visited[GetIndex(end)];
	
	// cout << "extracting path... " << endl;

	// cout << "visited-value: " << currCost << endl;
	// cout << "g-value: " << end.g_value << endl;

	finalPath.resize(0);
	finalPath.push_back(end);

	bool found = false;

	// cout << "currCost: " << currCost << endl;
	
	while (abs(currCost - 0.0) > EPSILON) 
    {
	found = false;
	Get_Diagonal_Successors(finalPath.back(), succ);
	
	for (unsigned int x = 0; x < succ.size(); x++)
	    {
		//cout << visited[GetIndex(succ[x])] << endl;

		if (abs(currCost - visited[GetIndex(succ[x].xy_loc)] - SQUARE_TWO) < EPSILON)
		    {
			finalPath.push_back(succ[x].xy_loc);
			currCost = currCost - SQUARE_TWO;
			found = true;
			break;
		    }
	    }
	
	if (!found) {
	    GetSuccessors(finalPath.back(), succ);
	    
	    for (unsigned int x = 0; x < succ.size(); x++)
		{
		    if (abs(currCost - visited[GetIndex(succ[x].xy_loc)] -1) < EPSILON)
			{
			    finalPath.push_back(succ[x].xy_loc);
			    currCost--;
			    break;
			}
		}
	}		
    }
	std::reverse(finalPath.begin(), finalPath.end());
}
