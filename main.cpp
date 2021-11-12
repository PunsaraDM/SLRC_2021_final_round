#include "PathFinder.h"
#include "Generator.h"
#include <bits/stdc++.h>

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define INVALID 5

#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE_COL 4
#define NOCOLOR 5

#define COLORED 1
#define WHITE_PATCH 2
#define INVERTED 3
#define NORMAL 4
#define INVERTWHITE 5
#define PATCHNOBOX 6

//size
#define COLS 9
#define ROWS 7

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1
#define SKIPPATH 2

//junction
#define VISITEDWITHOUTWHITE 2

//data packet structure
#define NAVIGATE_STATE 0
#define NAVIGATE_STATE_SEARCH 0
#define NAVIGATE_STATE_VISITED 1
#define DIRECTION 1
#define INV_PATCH 2
#define JUNC_TYPE 3
#define BOX_GRAB 4
#define NEGLECT 0
#define LOWER 1
#define MIDDLE 2

using namespace std;

vector<int> get_col_row(int col, int row, int direction)
{

    switch (direction)
    {
    case UP:
        row += 1;
        break;

    case RIGHT:
        col += 1;
        break;

    case DOWN:
        row -= 1;
        break;

    case LEFT:
        col -= 1;
        break;

    default:
        cout << "Invalid direction"
             << "\n";
        break;
    }
    return vector<int>{col, row};
}

//LAST_DIRECTION = ACTUAL LAST_DIRECTION
vector<int> convert_to_local(vector<int> paths, int last_direction)
{
    // cout << "last" << last_direction <<"\n";
    vector<int> temp_paths;
    for (int i = 0; i < 4; i++)
    {
        int local_dir = last_direction + i;
        if (local_dir >= 4)
        {
            local_dir = local_dir - 4;
        }
        // cout<< "path: "<< i << "," <<paths[local_dir] << "\n";
        if (paths[local_dir] == UNDISCOVERED)
        {
            temp_paths.push_back(DISCOVERED);
        }
        else
        {
            temp_paths.push_back(paths[local_dir]);
        }
    }

    return temp_paths;
}

vector<int> convert_content(vector<int> content)
{
    if (content.size() == 1)
    {
        content.push_back(NOCOLOR);
    }
    if (content.size() == 0)
    {
        content.push_back(NOCOLOR);
        content.push_back(NOCOLOR);
    }
    return content;
}

int main(int argc, char **argv)
{
    int row = 0;
    int col = 0;
    bool scan_over = false;
    int last_direction = RIGHT;
    Generator generator;
    cout << "simulation started"
         << "\n";
    PathFinder robot(0, 0);
    int count = 0;
    while (count < 20000)
    {
        vector<vector<int>> result;
        int current_dir = 0;
        if (!scan_over)
        {
            current_dir= 0;
            count += 1;
            int juncType = generator.maze.junctions[col][row].content_state;
            vector<int> path_state = convert_to_local(generator.maze.junctions[col][row].get_paths(), last_direction);
            vector<int> box_type = convert_content(generator.maze.junctions[col][row].get_content());
            result = robot.travel_maze(juncType, path_state, box_type);
        }
        else{
            result = robot.travel_maze(1, vector<int>(), vector<int>());
        }

        current_dir = result[1][0];
        last_direction = current_dir + last_direction;
        if (last_direction >= 4)
        {
            last_direction = last_direction - 4;
        }
        vector<int> pos = get_col_row(col, row, last_direction);
        scan_over = result[5][0];
        col = pos[0];
        row = pos[1];
    }

    return 0;
};
