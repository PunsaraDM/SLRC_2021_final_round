#include <bits/stdc++.h>
#include <cmath>
#include "Strategy.h"

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

#define COLS 9
#define ROWS 7

using namespace std;

static bool compare_col(const vector<int> &v1,const vector<int> &v2)
{
    return v1[1] < v2[1];
}

int Strategy::find_next_direction(int target_col, int target_row, int robot_col, int robot_row, Maze maze)
{
    vector<vector<int>> unvisited;
    vector<vector<int>> visited;
    // co-ordinates of 4 probable destinations
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
            int distance = abs(achievables[i][0] - target_col) + abs(achievables[i][1] - target_row);
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
    // cout << "visited" << "\n";
    // for (int i = 0; i < visited.size(); i++)
    // {
    //     cout << visited[i][0] << "," << visited[i][1] << "\n";
    // }
    // cout << "unvisited" << "\n";
    // for (int i = 0; i < unvisited.size(); i++)
    // {
    //     cout << unvisited[i][0] << "," << unvisited[i][1] << "\n";
    // }
    
    if(unvisited.size()){
        selected = unvisited[0][0];
    }
    else if(visited.size()){
        selected = visited[0][0];
    }
    return selected;
}
