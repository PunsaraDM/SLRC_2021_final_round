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

#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4

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
    int dir = 4;
    if (invert_found)
    {
        invert_found = false;
        dir = invert_dir;
        int opposite_dir = dir + 2;
        if (opposite_dir >= 4)
        {
            opposite_dir = opposite_dir - 4;
        }
        vector<int> newTravel{opposite_dir, invert_col, invert_row};
        if (robot == LEFT)
        {
            robot_left_stack.push_back(newTravel);
        }
        else
        {
            robot_right_stack.push_back(newTravel);
        }
    }
    else if (is_visited)
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
        int opposite_dir = dir + 2;
        if (opposite_dir >= 4)
        {
            opposite_dir = opposite_dir - 4;
        }

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

int Strategy::find_next_direction(int robot_col, int robot_row, Maze maze, int last_direction, bool has_white)
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
        bool invert = false;
        //handling inverted path
        if ((maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == VISITEDWITHOUTWHITE && has_white) || (maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == INVERTNEIGHBOUR && has_white))
        {
            cout << maze.junctions[achievables[i][0]][achievables[i][1]].get_state() << "\n";
            invert = true;
            invert_found = true;
            invert_col = achievables[i][0];
            invert_row = achievables[i][1];
            invert_dir = i;
            maze.junctions[achievables[i][0]][achievables[i][1]].set_state(DISCOVERED);
        }

        //setting invert neigbour
        if (maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == VISITEDWITHOUTWHITE && !has_white && maze.junctions[robot_col][robot_row].found_junctions == 4)
        {
            maze.junctions[robot_col][robot_row].set_state(INVERTNEIGHBOUR);
            vector<int> coord_parent{achievables[i][0], achievables[i][1]};
            vector<int> coord_child{achievables[i][0], achievables[i][1]};
            maze.junctions[robot_col][robot_row].inverted_chain_parent.push_back(coord_parent);
            maze.junctions[achievables[i][0]][achievables[i][1]].inverted_chain_children.push_back(coord_child);
        }
        else if (maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == INVERTNEIGHBOUR && !has_white && maze.junctions[robot_col][robot_row].found_junctions == 4)
        {
            maze.junctions[robot_col][robot_row].set_state(INVERTNEIGHBOUR);
            maze.update_inverted_chain(achievables[i][0], achievables[i][1], robot_col, robot_row);
        }

        //if the path is discovered and coordinates are acceptable
        if (0 <= achievables[i][0] && achievables[i][0] < COLS and 0 <= achievables[i][1] and achievables[i][1] < ROWS && *(paths + i) == DISCOVERED)
        {
            //if the location is undiscovered push to unvisited queue its index and distance
            if (maze.junctions[achievables[i][0]][achievables[i][1]].get_state() == UNDISCOVERED || invert)
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
        selected = find_next_direction_white(visited, unvisited);
        break;
    case INVERTED:
        selected = find_next_direction_inverted(visited, unvisited, last_direction, has_white);
        break;
    case NORMAL:
        selected = find_next_direction_normal(visited, unvisited);
        break;

    default:
        cout << "Invalid content state"
             << "\n";
        break;
    }

    cout << "Selected: " << selected << "\n";

    return selected;
}

int Strategy::find_next_direction_normal(vector<int> visited, vector<int> unvisited)
{
    cout << "Normal Junction"
         << "\n";
    int selected = INVALID;

    if (unvisited.size())
    {
        selected = get_from_priority(unvisited, LEFT, false);
    }
    else if (visited.size())
    {
        selected = get_from_priority(visited, LEFT, true);
    }
    return selected;
}

int Strategy::find_next_direction_colored_with_one(vector<int> visited, vector<int> unvisited)
{
    cout << "Colored One Junction"
         << "\n";

    int selected = INVALID;

    if (unvisited.size())
    {
        selected = get_from_priority(unvisited, LEFT, false);
    }
    else if (visited.size())
    {
        selected = get_from_priority(visited, LEFT, true);
    }
    return selected;
}

//change to next
int Strategy::find_next_direction_colored_with_two(vector<int> visited, vector<int> unvisited)
{
    cout << "Colored Two Junction"
         << "\n";

    int selected = INVALID;

    if (unvisited.size())
    {
        selected = get_from_priority(unvisited, LEFT, false);
    }
    else if (visited.size())
    {
        selected = get_from_priority(visited, LEFT, true);
    }
    return selected;
}

int Strategy::find_next_direction_inverted(vector<int> visited, vector<int> unvisited, int last_direction, bool has_white)
{
    cout << "Inverted Junction"
         << "\n";
    int selected = INVALID;

    if (!has_white)
    {
        selected = robot_left_stack[robot_left_stack.size() - 1][0];
        robot_left_stack.pop_back();
    }
    else
    {
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
            selected = get_from_priority(unvisited, LEFT, false);
            if (selected == straight_dir)
            {
                if (existence)
                {
                    selected = straight_dir;
                }
                else
                {
                    selected = UPWITHOUTWHITE;
                }
            }
        }
        else
        {
            selected = get_from_priority(visited, LEFT, true);
            if (visited.size() > 0)
            {
                if (selected == straight_dir)
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
            }
        }
    }

    return selected;
}

int Strategy::find_next_direction_white(vector<int> visited, vector<int> unvisited)
{
    cout << "White Junction"
         << "\n";

    int selected = INVALID;

    if (unvisited.size())
    {
        selected = get_from_priority(unvisited, LEFT, false);
    }
    else if (visited.size())
    {
        selected = get_from_priority(visited, LEFT, true);
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

int find_shortest_path(int col1, int row1, int col2, int row2, Maze maze)
{
    string key = to_string(col1) + to_string(row1);
    if (distance_store.count(key))
    {
        return distance_store.find(key)->first;
    }

    int *paths = maze.junctions[col1][row1].get_paths();

    vector<int> start{col1, row1};
    vector<int> destination{col2, row2};
    vector<vector<int>> queue{start};
    map<string, int> distance = {
        {key, 0}};
    map<string, vector<int>> parent = {};
    //vector<vector<int>> reverse_queue;
    while (queue.size() > 0)
    {
        vector<int> current_junc = queue[0];
        string current_key = to_string(current_junc[0]) + to_string(current_junc[1]);
        //reverse_queue.push_back(current_junc);
        queue.erase(queue.begin());
        int current_dist = distance.find(current_key)->first;

        int achievables[4][2] = {{current_junc[0], current_junc[1] + 1},
                                 {current_junc[0] + 1, current_junc[1]},
                                 {current_junc[0], current_junc[1] - 1},
                                 {current_junc[0] - 1, current_junc[1]}};

        for (int i = 0; i < 4; i++)
        {

            //if the path is discovered and coordinates are acceptable
            if (0 <= achievables[i][0] && achievables[i][0] < COLS and 0 <= achievables[i][1] and achievables[i][1] < ROWS && *(paths + i) == DISCOVERED)
            {
                vector<int> coord{achievables[i][0], achievables[i][1]};
                int queue.push_back(coord);
                int neighbour_key = to_string(coord[0]) + to_string(coord[1]);
                if ( !(distance.count(neighbour_key)) )
                {
                    distance.insert(std::pair<string, int>(neighbour_key, current_dist + 1));
                    children.insert(std::pair<string, int>(neighbour_key, current_key));
                }

                if (coord == destination)
                {
                    break
                }
                    /*if (distance.find(neighbour_key)->first > current_dist + 1)
                    {
                        distance.insert(std::pair<string, int>(neighbour_key, current_dist + 1)); //update distance
                        parent.insert(std::pair<string, int>(neighbour_key, current_key));
                    }
                    */
                
            }
        }


    }


    string destination_key = to_string(destination[0]) + to_string(destination[1]);
    vector<vector<int>> queue{start};

    while()


    return distance.find(destination_key)->first;;
}