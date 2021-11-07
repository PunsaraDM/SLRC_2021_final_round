#include "Junction.h"
#include <bits/stdc++.h>

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1
#define SKIPPATH 2

#define COLS 9
#define ROWS 7

//state
#define UNDISCOVERED 0
#define DISCOVERED 1
#define VISITEDWITHOUTWHITE 2
#define INVERTNEIGHBOUR 3

using namespace std;

void Junction::set_paths(int foundPaths[])
{
    for (int i = 0; i < 4; i++)
    {
        if (foundPaths[i] != NOTACCESIBLE)
        {
            if(paths[i] == UNDISCOVERED) {
                found_paths += 1;
            }
            paths[i] = foundPaths[i];
        }
    }
}

void Junction::set_path(int direction, int current_state)
{
    if (current_state != NOTACCESIBLE)
    {
        if (paths[direction] == UNDISCOVERED)
        {
            found_paths += 1;
            found_junctions += 1;
        }
        paths[direction] = current_state;
    }
}

void Junction::set_state(int received)
{
    if (state == INVERTNEIGHBOUR && inverted_chain_parent.size() == 0)
    {
        state = DISCOVERED;
    }
    else if (state == INVERTNEIGHBOUR)
    {
        inverted_chain_count -= 1;
    }
    else
    {
        state = received;
    }
}

int Junction::get_state()
{
    return state;
}

int Junction::get_path(int direction)
{
    return paths[direction];
}

int *Junction::get_paths()
{
    return paths;
}

void Junction::set_content(vector<int> recieved)
{
    content = recieved;
}

vector<int> Junction::get_content()
{
    return content;
}

void Junction::update_distances(int white[], int inverted[])
{
    if (paths[white[0]] != NOPATH)
    {
        if (white[1] < white_dis[1] || white_dis[0] == -1)
        {
            white_dis[1] = white[1];
            white_dis[0] = white[0];
        }
    }
    if (paths[inverted[0]] != NOPATH)
    {
        if (inverted[1] < inverted_dis[1])
        {
            inverted_dis[1] = inverted[1];
            inverted_dis[0] = inverted[0];
        }
    }
}