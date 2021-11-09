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
#define UPWITHOUTWHITE 4
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
#define INVERTNEIGHBOUR 3

using namespace std;

static bool compare_col(const vector<int> &v1, const vector<int> &v2)
{
    return v1[1] < v2[1];
}

int Strategy::get_from_priority(vector<int> juncs, int robot, bool is_visited)
{

    int dir = INVALID;
    if (is_visited)
    {
        vector<vector<int>> stack = robot_right_stack;
        if (robot == LEFT)
        {
            stack = robot_left_stack;
        }
        dir = stack[stack.size() - 1][0];
        stack.pop_back();
        if (robot == LEFT)
        {
            robot_left_stack = stack;
        }
        else
        {
            robot_right_stack = stack;
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
        int opposite_dir = get_opposite_dir(dir);
        vector<int> newTravel{opposite_dir, current_col, current_row};
        if (robot == LEFT)
        {
            robot_left_stack.push_back(newTravel);
        }
        else
        {
            robot_right_stack.push_back(newTravel);
        }
    }

    return dir;
}

int Strategy::get_backtrack_white(int robot)
{
    bool found = false;
    int index = 1;
    int selected = robot_left_missed_inverted[0][2];
    if (robot_left_missed_inverted.size() > 1)
    {
        for (int i = 1; i < robot_left_missed_inverted.size(); i++)
        {
            if (robot_left_missed_inverted[i].size() > 2)
            {
                robot_left_missed_inverted[i].pop_back();
            }
            else
            {
                robot_left_missed_inverted[i].push_back(get_opposite_dir(selected));
            }
        }
    }

    while (!found)
    {
        vector<int> path = robot_left_missed_inverted[index];
        if (path.size() > 2)
        {
            found = true;
            selected = path[path.size() - 1];
            path.pop_back();
            robot_left_missed_inverted[index] = path;
            if (path.size() == 2)
            {
                robot_left_missed_inverted.erase(robot_left_missed_inverted.begin() + index);
            }
        }
        index += 1;
    }

    return selected;
}

int Strategy::find_next_direction(int robot_col, int robot_row, Maze maze, int robot, int last_direction, bool has_white)
{
    if (has_white && ((robot == LEFT && robot_left_missed_inverted.size() > 0) || (robot == RIGHT && robot_right_missed_inverted.size() > 0)))
    {
        backtracking_white = true;
        return get_backtrack_white(robot);
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

        cout << "visited"
             << "\n";
        for (int i = 0; i < visited.size(); i++)
        {
            cout << visited[i] << "\n";
        }
        cout << "unvisited"
             << "\n";
        for (int i = 0; i < unvisited.size(); i++)
        {
            cout << unvisited[i] << "\n";
        }

        if (maze.junctions[robot_col][robot_row].content_state == INVERTED)
        {
            if (has_white)
            {
                maze.junctions[robot_col][robot_row].content_state = INVERTWHITE;
            }
            selected = find_next_direction_inverted(visited, unvisited, robot, has_white);
        }
        else
        {
            selected = find_next_direction_normal(visited, unvisited, robot);
        }

        cout << "Selected: " << selected << "\n";

        return selected;
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

int Strategy::find_next_direction_inverted(vector<int> visited, vector<int> unvisited, int robot, bool has_white)
{
    int selected = INVALID;

    if (!has_white)
    {
        selected = robot_left_stack[robot_left_stack.size() - 1][0];
        vector<int> missed{current_col, current_row};
        robot_left_missed_inverted.push_back(missed);
        robot_left_stack.pop_back();
    }
    else
    {
        if (unvisited.size() > 0)
        {
            selected = get_from_priority(unvisited, robot, false);
        }
        else
        {
            selected = get_from_priority(visited, robot, true);
        }
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

void Strategy::update_missed(int direction, int robot)
{
    int opposite_dir = get_opposite_dir(direction);
    if (robot == LEFT)
    {
        for (int i = 0; i < robot_left_missed_inverted.size(); i++)
        {
            robot_left_missed_inverted.push_back(opposite_dir);
        }
    }
    else
    {
        for (int i = 0; i < robot_right_missed_inverted.size(); i++)
        {
            robot_right_missed_inverted.push_back(opposite_dir);
        }
    }
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
