#include <bits/stdc++.h>
#include <cmath>
#include "Strategy.h"
#include "Junction.h"
#include <iostream>
#include <vector>
#include <algorithm>

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define INVALID 5

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1
#define SKIPPATH 2

//CONTENT STATE
#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4
#define INVERTWHITE 5

#define COLS 9
#define ROWS 7

//junctions
#define VISITEDWITHOUTWHITE 2

#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4

using namespace std;

static bool compare_col(const vector<int> &v1, const vector<int> &v2)
{
    return v1[1] < v2[1];
}

int Strategy::get_from_priority(vector<int> juncs, int robot, bool is_visited)
{

    int dir = INVALID;
    vector<int> stack = robot_right_stack;
    if (robot == LEFT)
    {
        stack = robot_left_stack;
    }
    if (is_visited)
    {
        dir = find_visited_next(juncs);
        if (dir == INVALID)
        {
            dir = stack[stack.size() - 1];
            stack.pop_back();
        }
        else
        {
            stack.push_back(get_opposite_dir(dir));
        }
    }
    else
    {
        vector<int> priorityLeft{LEFT, DOWN, UP, RIGHT};
        vector<int> priorityRight{RIGHT, UP, DOWN, LEFT};
        vector<int> priority = priorityRight;
        if (robot == LEFT)
        {
            priority = priorityLeft;
        }

        for (int i = 0; i < 4; i++)
        {
            if (find(juncs.begin(), juncs.end(), priority[i]) != juncs.end())
            {
                dir = priority[i];
                break;
            }
        }

        stack.push_back(get_opposite_dir(dir));
    }

    if (robot == LEFT)
    {
        robot_left_stack = stack;
    }
    else
    {
        robot_right_stack = stack;
    }

    return dir;
}

int Strategy::find_next_direction(int robot_col, int robot_row, Maze maze, int robot, int last_direction, bool has_white)
{
    c_maze = maze;
    vector<int> current_loc{robot_col, robot_row};
    int content_state = maze.junctions[robot_col][robot_row].content_state;

    if (backtracking_white)
    {
        int selected = shortest_path[0];
        shortest_path.erase(shortest_path.begin());
        invert_path.push_back(get_opposite_dir(selected));
        if (shortest_path.size() == 0)
        {
            backtracking_invert = true;
            backtracking_white = false;
        }
        return selected;
    }
    else if (backtracking_invert)
    {
        int selected = invert_path[0];
        invert_path.erase(invert_path.begin());
        if (invert_path.size() == 0)
        {
            backtracking_invert = false;
        }
        return selected;
    }
    else
    {
        if (content_state == WHITE && has_white)
        {
            white_locations.push_back(current_loc);
        }

        if (content_state == INVERTED && !has_white && white_locations.size() > 0)
        {
            backtracking_white = true;
            vector<int> white_coord = white_locations[white_locations.size() - 1];
            white_locations.pop_back();
            find_shortest_path(white_coord[0], white_coord[1]);
            int selected = shortest_path[0];
            shortest_path.erase(shortest_path.begin());
            invert_path.push_back(get_opposite_dir(selected));
            return selected;
        }
        else
        {
            current_col = robot_col;
            current_row = robot_row;
            vector<int> unvisited;
            vector<int> visited;
            int achievables[4][2] = {{robot_col, robot_row + 1},
                                     {robot_col + 1, robot_row},
                                     {robot_col, robot_row - 1},
                                     {robot_col - 1, robot_row}};

            //get the discovered paths from the current junction
            int *paths = maze.junctions[robot_col][robot_row].get_paths();

            //loop for probable locations
            for (int i = 0; i < 4; i++)
            {
                //if the path is discovered and coordinates are acceptable
                if (0 <= achievables[i][0] && achievables[i][0] < COLS and 0 <= achievables[i][1] and achievables[i][1] < ROWS && *(paths + i) == DISCOVERED)
                {
                    //if the location is undiscovered push to unvisited queue its index and distance
                    if (maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == UNDISCOVERED)
                    {
                        unvisited.push_back(i);
                    }
                    //if the location is discovered push to visited queue its index and distance and not over yet
                    else if (maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == DISCOVERED)
                    {
                        visited.push_back(i);
                    }
                }
            }

            int selected = INVALID;

            if (maze.junctions[robot_col][robot_row].content_state == INVERTED && has_white)
            {
                maze.junctions[robot_col][robot_row].content_state = INVERTWHITE;
            }

            selected = find_next_direction_normal(visited, unvisited, robot);

            cout << "Selected: " << selected << "\n";

            return selected;
        }
    }
}

int Strategy::find_next_direction_normal(vector<int> visited, vector<int> unvisited, int robot)
{
    int selected = INVALID;

    if (unvisited.size())
    {
        selected = get_from_priority(unvisited, robot, false);
    }
    else if (visited.size())
    {
        selected = get_from_priority(visited, robot, true);
    }
    return selected;
}

bool Strategy::check_existence(vector<int> arr, int val)
{
    bool found = false;
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i] == val)
        {
            found = true;
            break;
        }
    }
    return found;
}

int Strategy::get_opposite_dir(int direction)
{

    int opposite_dir = direction + 2;
    if (opposite_dir > 3)
    {
        opposite_dir = opposite_dir - 4;
    }

    return opposite_dir;
}

void Strategy::find_shortest_path(int col, int row)
{
    int opposite_dir =  2;
    if (opposite_dir > 3)
    {
        opposite_dir = opposite_dir - 4;
    }
}

int Strategy::find_visited_next(vector<int> juncs)
{
    int dir = INVALID;
    int min_founds = 4;

    for (int i = 0; i < juncs.size(); i++)
    {
        int founds = 4;
        int col = current_col;
        int row = current_row;
        if (juncs[i] == 0)
        {
            row = current_row + 1;
        }
        else if (juncs[i] == 1)
        {
            col = col + 1;
        }
        else if (juncs[i] == 2)
        {
            row = row - 1;
        }
        else
        {
            col = col - 1;
        }
        founds = c_maze.junctions[col][row].found_junctions;
        if (min_founds > founds)
        {
            min_founds = founds;
            dir = juncs[i];
        }
    }

    return dir;
}
