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

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4

#define COLS 9
#define ROWS 7

using namespace std;

static bool compare_col(const vector<int> &v1, const vector<int> &v2)
{
    return v1[1] < v2[1];
}

int Strategy::find_next_direction(int target_col, int target_row, int robot_col, int robot_row, Maze maze, int last_direction)
{

    vector<vector<int>> unvisited;
    vector<vector<int>> visited;
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
            // get distance to target position
            cout << "i:"  << i <<"," <<  *(paths + i) << "\n";
            int distance = find_distance_to_target_position(target_col, target_row, achievables[i][0], achievables[i][1]);
            vector<int> vect{i, distance};
            //if the location is undiscovered push to unvisited queue its index and distance
            if (maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == UNDISCOVERED)
            {
                unvisited.push_back(vect);
            }

            //if the location is discovered push to visited queue its index and distance and not over yet
            else if (maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == DISCOVERED)
            {
                visited.push_back(vect);
            }
        }
    }

    int n = unvisited.size();

    //sort visited and unvisited queues on distance
    sort(unvisited.begin(), unvisited.end(), compare_col);
    sort(visited.begin(), visited.end(), compare_col);

    int selected = 4;

    switch (maze.junctions[robot_col][robot_row].content_state)
    {
    case COLORED:
        if (maze.junctions[robot_col][robot_row].get_content().size() > 1)
        {
            selected = find_next_direction_colored_with_two(visited);
        }
        else
        {
            selected = find_next_direction_colored_with_one(visited, unvisited);
        }
        break;
    case WHITE:
        /* code */
        selected = find_next_direction_white(visited, unvisited);
        break;
    case INVERTED:
        /* code */
        selected = find_next_direction_inverted(visited, unvisited, last_direction);
        break;
    case NORMAL:
        /* code */
        selected = find_next_direction_normal(visited, unvisited);
        break;

    default:
        cout << "Invalid content state"
             << "\n";
        break;
    }
    cout << "selected:" << selected << "\n";
    cout << "visited"
         << "\n";
    for (int i = 0; i < visited.size(); i++)
    {
        cout << visited[i][0] << "," << visited[i][1] << "\n";
    }
    cout << "unvisited"
         << "\n";
    for (int i = 0; i < unvisited.size(); i++)
    {
        cout << unvisited[i][0] << "," << unvisited[i][1] << "\n";
    }

    return selected;
}

int Strategy::find_next_direction_normal(vector<vector<int>> visited, vector<vector<int>> unvisited)
{
    cout << "Normal Junction"
         << "\n";
    int selected = 4;

    if (unvisited.size())
    {
        selected = unvisited[0][0];
    }
    else if (visited.size())
    {
        selected = visited[0][0];
    }
    return selected;
}

int Strategy::find_next_direction_colored_with_one(vector<vector<int>> visited, vector<vector<int>> unvisited)
{
    cout << "Colored One Junction"
         << "\n";

    int selected = 4;

    if (unvisited.size())
    {
        selected = unvisited[0][0];
    }
    else if (visited.size())
    {
        selected = visited[0][0];
    }
    return selected;
}

int Strategy::find_next_direction_colored_with_two(vector<vector<int>> visited)
{
    cout << "Colored Two Junction"
         << "\n";
    int selected = 4;
    if (visited.size() > 0)
        selected = visited[0][0];
    return selected;
}

int Strategy::find_next_direction_inverted(vector<vector<int>> visited, vector<vector<int>> unvisited, int last_direction)
{
    cout << "Inverted Junction"
         << "\n";

    vector<int> directions_to_check;

    if (last_direction == LEFT || last_direction == RIGHT)
    {
        directions_to_check.push_back(UP);
        directions_to_check.push_back(DOWN);
    }
    else
    {
        directions_to_check.push_back(RIGHT);
        directions_to_check.push_back(LEFT);
    }

    int selected = 4;
    int vertical = 4;
    for (int i = 0; i < unvisited.size(); i++)
    {
        int dir = unvisited[i][0];
        if (find(directions_to_check.begin(), directions_to_check.end(), dir) != directions_to_check.end())
        // if (dir == LEFT || dir == RIGHT)
        {
            selected = dir;
            break;
        }
        else
        {
            vertical = dir;
        }
    }
    //no left and right in visited but has an vertical position in visited; go to vertical position
    if (selected == 4 && vertical != 4)
    {
        selected = vertical;
    }
    else
    {
        for (int i = 0; i < visited.size(); i++)
        {
            int dir = visited[i][0];
            if (find(directions_to_check.begin(), directions_to_check.end(), dir) != directions_to_check.end())
            {
                selected = dir;
                break;
            }
        }
    }

    return selected;
}
int Strategy::find_next_direction_white(vector<vector<int>> visited, vector<vector<int>> unvisited)
{
    cout << "White Junction"
         << "\n";

    int selected = 4;

    if (unvisited.size())
    {
        selected = unvisited[0][0];
    }
    else if (visited.size())
    {
        selected = visited[0][0];
    }
    return selected;
}



int Strategy::find_distance_to_target_position(int target_col, int target_row, int pos_col, int pos_row)
{
    return abs(pos_col - target_col) + abs(pos_row - target_row);
}