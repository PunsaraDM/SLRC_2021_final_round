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
#define SKIPPATH 2

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
            selected = find_next_direction_colored_with_two(visited, unvisited);
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
        selected = find_next_direction_inverted(visited, unvisited, last_direction, maze, robot_col, robot_row);
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
    // cout << "selected:" << selected << "\n";
    // cout << "visited"
    //      << "\n";
    // for (int i = 0; i < visited.size(); i++)
    // {
    //     cout << visited[i][0] << "," << visited[i][1] << "\n";
    // }
    // cout << "unvisited"
    //      << "\n";
    // for (int i = 0; i < unvisited.size(); i++)
    // {
    //     cout << unvisited[i][0] << "," << unvisited[i][1] << "\n";
    // }

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

//scan paths as normal
//calculate the distances
//place the white box either on left or right side
// pick the colored box
//scan path
//if the path next is to the up then pick box by going to that side place it on other side drop the colored box and then go in the up direction
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

//change to next
int Strategy::find_next_direction_colored_with_two(vector<vector<int>> visited, vector<vector<int>> unvisited)
{
    cout << "Colored Two Junction"
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

int Strategy::find_next_direction_inverted(vector<vector<int>> visited, vector<vector<int>> unvisited, int last_direction, Maze maze, int robot_col, int robot_row)
{
    cout << "Inverted Junction"
         << "\n";
    int selected = 4;
    int straight_dir = last_direction;
    int back_dir = last_direction + 2;
    vector<int> perpendicular{last_direction + 1, last_direction - 1};
    bool existence = false;

    if (back_dir > 3)
    {
        back_dir = back_dir - 4;
    }

    for (int i = 0; i < 2; i++)
    {
        if (perpendicular[i] > 3)
        {
            perpendicular[i] = 0;
        }
        else if (perpendicular[i] < 0)
        {
            perpendicular[i] = 3;
        }
        if (check_existence(unvisited, perpendicular[i]) || check_existence(visited, perpendicular[i]))
        {
            existence = true;
        }
    }

    if (unvisited.size() > 0)
    {
        if (unvisited[0][0] == straight_dir)
        {
            if (existence)
            {
                selected = straight_dir;
            }
            else
            {
                // UP - 0 COL,ROW+1  |  RIGHT 1 COL+1,ROW | DOWN 2 COL, ROW-1, | LEFT 3 COL-1, ROW
                int neighbour_col = robot_col;
                int neighbour_row = robot_row;
                if (straight_dir % 2 == 0)
                {
                    neighbour_row = neighbour_row + 1 - straight_dir;
                }
                else
                {
                    neighbour_col = neighbour_col + 2 - straight_dir;
                }
                maze.junctions[robot_col][robot_row].set_path(straight_dir, SKIPPATH);
                maze.junctions[neighbour_col][neighbour_row].set_path(back_dir, SKIPPATH);
                selected = back_dir;
            }
        }
        else
        {
            selected = unvisited[0][0];
        }
    }
    else
    {
        if (visited.size() > 0)
        {
            if (visited[0][0] == straight_dir)
            {
                if (existence)
                {
                    selected = straight_dir;
                }
                else
                {
                    selected = back_dir;
                }
            }
            else
            {
                selected = visited[0][0];
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

bool Strategy::check_existence(vector<vector<int>> arr, int val)
{
    bool found = false;
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i][0] == val)
        {
            found = true;
            break;
        }
    }
    return found;
}