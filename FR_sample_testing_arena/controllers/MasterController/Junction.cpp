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

using namespace std;

void Junction::set_paths(vector<int> foundPaths)
{
    for (int i = 0; i < 4; i++)
    {
        if (foundPaths[i] != NOTACCESIBLE)
        {
            if (paths[i] == UNDISCOVERED)
            {
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
        }
        paths[direction] = current_state;
    }
}

void Junction::increment_found_junc()
{
    found_junctions += 1;
}

void Junction::set_state(int received)
{
    state = received;
}

int Junction::get_state()
{
    return state;
}

int Junction::get_path(int direction)
{
    return paths[direction];
}

vector<int> Junction::get_paths()
{
    return paths;
}

void Junction::set_content(vector<int> recieved)
{
    content = recieved;
}

bool Junction::set_found_by(int dir, int robot)
{

    bool match_found = false;
    vector<int> founded = found_by[dir];
      cout << "found by: " << robot << "," << dir << "\n";
    for (size_t i = 0; i < found_by.size(); i++)
    {
        cout << "dir: " << i << "| "
             << "\n";
        for (size_t j = 0; j < found_by[i].size(); j++)
        {
            cout << found_by[i][j] << "|";
        }
        cout << "\n";
    }
    for (size_t i = 0; i < founded.size(); i++)
    {
        if (robot == founded[i])
        {
            match_found = true;
        }
    }

    if (!match_found)
    {
        found_by[dir].push_back(robot);
    }

    cout << "found by: " << robot << "," << dir << "\n";
    for (size_t i = 0; i < found_by.size(); i++)
    {
        cout << "dir: " << i << "| "
             << "\n";
        for (size_t j = 0; j < found_by[i].size(); j++)
        {
            cout << found_by[i][j] << "|";
        }
        cout << "\n";
    }
    if (found_by[dir].size() == 2)
    {
        return true;
    }
    return false;
}

vector<int> Junction::get_content()
{
    return content;
}
