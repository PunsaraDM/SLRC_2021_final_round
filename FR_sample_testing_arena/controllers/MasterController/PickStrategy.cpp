#include <bits/stdc++.h>
#include <cmath>
#include "PickStrategy.h"
#include "Junction.h"
#include "Maze.h"
#include <iostream>
#include <vector>
#include <algorithm>

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define INVALID 5
#define ANY 6

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

#define RED 1
#define GREEN 2
#define BLUE 3

#define DOUBLETOP 2
#define DOUBLEBOTTOM 1
#define TOP 3
using namespace std;

void PickStrategy::find_combinations(Maze *m)
{
    //asumming that the location vector is 3d {{{red_col1,red_row1},{red_col2,red_row2}} }
    vector<vector<vector<int>>> locations = m->colored_junctions;
    vector<vector<int>> red_locations = locations[RED - 1];
    vector<vector<int>> green_locations = locations[GREEN - 1];
    vector<vector<int>> blue_locations = locations[BLUE - 1];

    int min_dist = 10000;

    //find the shortest path distances

    discover_shortest_paths(locations, m);
    for (int red = 0; red < 2; red++)
    {
        for (int green = 0; green < 2; green++)
        {
            for (int blue = 0; blue < 2; blue++)
            {
                vector<int> pos{red, green, blue};
                vector<int> pos_opposite{!red, !green, !blue};
                bool pos_valid = check_for_top_boxes(pos[0], pos[1], pos[2], locations);
                bool pos_opposite_valid = check_for_top_boxes(pos_opposite[0], pos_opposite[1], pos_opposite[2], locations);

                int valid = INVALID;
                if (colors_matched)
                {
                    valid = check_combination(pos, pos_opposite, m);
                }
                else
                {
                    valid = ANY;
                }
                //at least one box is in top
                if (pos_valid && pos_opposite_valid && valid != INVALID)
                {
                    int left_dist = red_distances[2 * red] + green_distances[2 * green] + blue_distances[2 * blue];
                    int right_dist = red_distances[2 * red + 1] + green_distances[2 * green + 1] + blue_distances[2 * blue + 1];

                    int rest_left_dist = red_distances[2 * (!red)] + green_distances[2 * (!green)] + blue_distances[2 * (!blue)];
                    int rest_right_dist = red_distances[2 * (!red) + 1] + green_distances[2 * (!green) + 1] + blue_distances[2 * (!blue) + 1];

                    int first_left = left_dist + rest_right_dist;
                    int first_right = right_dist + rest_left_dist;

                    if (valid == ANY)
                    {
                        if (first_left < min_dist || first_right < min_dist)
                        {

                            if (first_left < first_right)
                            {
                                selected_left = pos;
                                selected_right = pos_opposite;
                            }
                            else
                            {
                                selected_left = pos_opposite;
                                selected_right = pos;
                            }
                        }
                    }
                    else if (valid == LEFT)
                    {
                        if (first_left < min_dist)
                        {
                            selected_left = pos;
                            selected_right = pos_opposite;
                        }
                    }
                    else
                    {
                        if (first_right < min_dist)
                        {
                            selected_left = pos_opposite;
                            selected_right = pos;
                        }
                    }
                }
            }
        }
    }

    order_left = find_order(selected_left, locations);

    order_right = find_order(selected_right, locations);

    cout << "order right"
         << "\n";
    for (size_t i = 0; i < order_right.size(); i++)
    {
        cout << order_right[i][0] << "," << order_right[i][1] << "| ";
    }
    cout << "\n";
    cout << "order left"
         << "\n";
    for (size_t i = 0; i < order_left.size(); i++)
    {
        cout << order_left[i][0] << "," << order_left[i][1] << "| ";
    }
    cout << "\n";
}

void PickStrategy::discover_shortest_paths(vector<vector<vector<int>>> locations, Maze *maze)
{
    for (size_t i = 0; i < locations.size(); i++)
    {
        for (size_t j = 0; j < locations[i].size(); j++)
        {
            //left even right odd
            if (i == RED - 1)
            {
                red_distances.push_back(find_shortest_path(0, 0, locations[i][j][0], locations[i][j][1], RED, 0, maze));
                red_distances.push_back(find_shortest_path(8, 6, locations[i][j][0], locations[i][j][1], RED, 0, maze));
            }
            else if (i == GREEN - 1)
            {
                green_distances.push_back(find_shortest_path(0, 0, locations[i][j][0], locations[i][j][1], GREEN, 0, maze));
                green_distances.push_back(find_shortest_path(8, 6, locations[i][j][0], locations[i][j][1], GREEN, 0, maze));
            }
            else
            {
                blue_distances.push_back(find_shortest_path(0, 0, locations[i][j][0], locations[i][j][1], BLUE, 0, maze));
                blue_distances.push_back(find_shortest_path(8, 6, locations[i][j][0], locations[i][j][1], BLUE, 0, maze));
            }
        }
    }
}

vector<vector<int>> PickStrategy::order_lower(vector<vector<vector<int>>> locations, vector<vector<int>> top, vector<vector<int>> lower)
{
    vector<vector<int>> temp_lower_priority;
    vector<vector<int>> temp_lower_least;
    int lower_col;
    int lower_row;
    int top_col;
    int top_row;
    bool found = false;

    for (size_t i = 0; i < lower.size(); i++)
    {
        lower_col = locations[lower[i][0]][lower[i][1]][0];
        lower_row = locations[lower[i][0]][lower[i][1]][1];
        cout << "lower: " << lower_col << "," << lower_row << "\n";
        found = false;
        for (size_t j = 0; j < top.size(); j++)
        {
            top_col = locations[top[j][0]][top[j][1]][0];
            top_row = locations[top[j][0]][top[j][1]][1];
            cout << "top: " << top_col << "," << top_row << "\n";

            if (top_col == lower_col && lower_row == top_row)
            {
                cout << "found"
                     << "\n";
                found = true;
                break;
            }
        }
        if (found)
        {
            temp_lower_priority.push_back(lower[i]);
        }
        else
        {
            temp_lower_least.push_back(lower[i]);
        }
    }
    temp_lower_priority.reserve(temp_lower_priority.size() + distance(temp_lower_least.begin(), temp_lower_least.end()));
    temp_lower_priority.insert(temp_lower_priority.end(), temp_lower_least.begin(), temp_lower_least.end());

    return temp_lower_priority;
}
vector<vector<int>> PickStrategy::find_order(vector<int> selected, vector<vector<vector<int>>> locations)
{
    vector<vector<int>> lower;
    vector<vector<int>> middle;
    vector<vector<int>> top;
    vector<vector<int>> order;

    for (int i = 0; i < 3; i++)
    {
        vector<int> val{i, selected[i]};

        if (locations[i][selected[i]][2] == TOP)
        {
            top.push_back(val);
        }
        if (locations[i][selected[i]][2] == DOUBLETOP)
        {
            middle.push_back(val);
        }
        if (locations[i][selected[i]][2] == DOUBLEBOTTOM)
        {
            lower.push_back(val);
        }
    }

    lower = order_lower(locations, middle, lower);
    order.reserve(order.size() + distance(middle.begin(), middle.end()));
    order.insert(order.end(), middle.begin(), middle.end());

    order.reserve(order.size() + distance(top.begin(), top.end()));
    order.insert(order.end(), top.begin(), top.end());

    order.reserve(order.size() + distance(lower.begin(), lower.end()));
    order.insert(order.end(), lower.begin(), lower.end());
    return order;
}

bool PickStrategy::check_for_top_boxes(int red, int green, int blue, vector<vector<vector<int>>> locations)
{
    if (locations[RED - 1][red][2] == TOP || locations[RED - 1][red][2] == DOUBLETOP)
    {
        return true;
    }

    if (locations[GREEN - 1][green][2] == TOP || locations[GREEN - 1][green][2] == DOUBLETOP)
    {
        return true;
    }

    if (locations[BLUE - 1][blue][2] == TOP || locations[BLUE - 1][blue][2] == DOUBLETOP)
    {
        return true;
    }
    return false;
}

//give the destination as the start
//col1 -> 0,0 || 8,6
//col2-> colored box
int PickStrategy::find_shortest_path(int col1, int row1, int col2, int row2, int color, int dir, Maze *c_maze)
{
    cout << "start: " << col1 << "," << row1 << "\n";
    cout << "end: " << col2 << "," << row2 << "\n";
    string key = to_string(col1) + to_string(row1);

    vector<int> start{col1, row1};
    vector<int> destination{col2, row2};
    vector<vector<int>> queue{start};
    map<string, int> distance = {
        {key, 0}};
    map<string, vector<string>> parent = {};

    bool break_both = false;
    while (queue.size() > 0 && !break_both)
    {
        vector<int> current_junc = queue[0];
        string current_key = to_string(current_junc[0]) + to_string(current_junc[1]);
        // cout << "current key:" << current_key << "\n";
        queue.erase(queue.begin());
        int current_dist = distance.at(current_key);

        vector<vector<int>> achievables{{current_junc[0], current_junc[1] + 1},
                                        {current_junc[0] + 1, current_junc[1]},
                                        {current_junc[0], current_junc[1] - 1},
                                        {current_junc[0] - 1, current_junc[1]}};
        vector<int> paths = c_maze->junctions[current_junc[0]][current_junc[1]].get_paths();

        for (int i = 0; i < 4; i++)
        {

            // if the path is discovered and coordinates are acceptable
            if (0 <= achievables[i][0] && achievables[i][0] < COLS and 0 <= achievables[i][1] and achievables[i][1] < ROWS && paths[i] == DISCOVERED && (c_maze->junctions[achievables[i][0]][achievables[i][1]].get_state() == DISCOVERED || (achievables[i][0] == col1 && achievables[i][1] == row1)))
            {

                vector<int> coord{achievables[i][0], achievables[i][1]};
                string neighbour_key = to_string(coord[0]) + to_string(coord[1]);
                if (!distance.count(neighbour_key))
                {
                    distance.insert(std::pair<string, int>(neighbour_key, current_dist + 1));
                    vector<string> parent_vec{current_key, to_string(get_opposite_dir(i))};
                    // cout << "parent:" << current_key << "| neighbour: " << neighbour_key << " dir : " << get_opposite_dir(i) << "\n";
                    parent[neighbour_key] = parent_vec;
                    queue.push_back(coord);
                }

                if (col2 == coord[0] && row2 == coord[1])
                {

                    break_both = true;
                    break;
                }
            }
        }
    }

    string destination_key = to_string(col2) + to_string(row2);
    string operation_key = destination_key;
    vector<int> sequence;

    while (operation_key != key)
    {
        vector<string> current = parent[operation_key];
        operation_key = current[0];
        sequence.push_back(stoi(current[1]));
    }
    cout << "sequence: ";
    for (size_t i = 0; i < sequence.size(); i++)
    {
        cout << sequence[i] << " |";
    }
    cout << "\n";
    if (dir != 0)
    {
        if (dir == LEFT)
        {
            // left_stack.reserve(left_stack.size() + distance(sequence.begin(), sequence.end()));
            left_stack.insert(left_stack.end(), sequence.begin(), sequence.end());
        }
        else
        {
            // right_stack.reserve(right_stack.size() + distance(sequence.begin(), sequence.end()));
            right_stack.insert(right_stack.end(), sequence.begin(), sequence.end());
        }
    }
    else
    {
        if (shortest_path_seq.size() < color)
        {
            vector<vector<int>> init_vector;
            init_vector.push_back(sequence);
            shortest_path_seq.push_back(init_vector);
        }
        else
        {
            shortest_path_seq[color - 1].push_back(sequence);
        }
    }
    cout << "next"
         << "\n";
    return distance[destination_key];
}

int PickStrategy::get_opposite_dir(int direction)
{

    int opposite_dir = direction + 2;
    if (opposite_dir > 3)
    {
        opposite_dir = opposite_dir - 4;
    }

    return opposite_dir;
}

int PickStrategy::find_next_direction_pick(int robot, Maze *maze)
{
    int direction = INVALID;
    if (robot == LEFT)
    {
        cout << "left:";
        for (size_t i = 0; i < left_stack.size(); i++)
        {
            cout << left_stack[i] << "| ";
        }
        cout << "\n";
        direction = left_stack[0];
        left_stack.erase(left_stack.begin());
    }
    if (robot == RIGHT)
    {
        cout << "right:";
        for (size_t i = 0; i < right_stack.size(); i++)
        {
            cout << right_stack[i] << "| ";
        }
        cout << "\n";
        direction = right_stack[0];
        right_stack.erase(right_stack.begin());
    }

    return direction;
}

void PickStrategy::add_to_stack(int robot, vector<int> seq)
{
    vector<int> reverse_seq = get_reverse_path(seq);
    if (robot == LEFT)
    {
        left_stack.reserve(left_stack.size() + distance(seq.begin(), seq.end()));
        left_stack.insert(left_stack.begin(), seq.begin(), seq.end());
        left_stack.reserve(left_stack.size() + distance(seq.begin(), seq.end()));
        left_stack.insert(left_stack.begin(), reverse_seq.begin(), reverse_seq.end());
    }
    else
    {
        right_stack.reserve(right_stack.size() + distance(seq.begin(), seq.end()));
        right_stack.insert(right_stack.begin(), seq.begin(), seq.end());
        right_stack.reserve(right_stack.size() + distance(seq.begin(), seq.end()));
        right_stack.insert(right_stack.begin(), reverse_seq.begin(), reverse_seq.end());
    }
}

void PickStrategy::initialize(Maze *m, int left_col, int left_row, int right_col, int right_row, bool matched)
{
    colors_matched = matched;
    left_start_col = left_col;
    left_start_row = left_row;
    right_start_col = right_col;
    right_start_row = right_row;
    cout << "initializing"
         << "\n";
    vector<vector<vector<int>>> locations = m->colored_junctions;
    find_combinations(m);

    // for (size_t i = 0; i < order_left.size(); i++)
    // {
    //     cout << "order_left: " << order_left[i][0] << "," << order_left[i][1] << '\n';

    find_shortest_path(locations[order_left[0][0]][order_left[0][1]][0], locations[order_left[0][0]][order_left[0][1]][1], left_start_col, left_start_row, 0, LEFT, m);
    find_shortest_path(locations[order_right[0][0]][order_right[0][1]][0], locations[order_right[0][0]][order_right[0][1]][1], right_start_col, right_start_row, 0, RIGHT, m);

    for (size_t i = 0; i < 3; i++)
    {
        vector<int> path = shortest_path_seq[order_left[i][0]][order_left[i][1] * 2];
        if (i == 0)
        {
            left_stack.reserve(left_stack.size() + distance(path.begin(), path.end()));
            left_stack.insert(left_stack.end(), path.begin(), path.end());
        }
        else
        {
            vector<int> reverse_path = get_reverse_path(path);
            left_stack.reserve(left_stack.size() + distance(reverse_path.begin(), reverse_path.end()));
            left_stack.insert(left_stack.end(), reverse_path.begin(), reverse_path.end());

            left_stack.reserve(left_stack.size() + distance(path.begin(), path.end()));
            left_stack.insert(left_stack.end(), path.begin(), path.end());
        }
    }

    cout << "left_Stack:"
         << "\n";
    for (size_t i = 0; i < left_stack.size(); i++)
    {
        cout << left_stack[i] << "|";
    }
    cout << "\n";
    for (size_t i = 0; i < 3; i++)
    {
        vector<int> path = shortest_path_seq[order_right[i][0]][order_right[i][1] * 2 + 1];
        if (i == 0)
        {
            right_stack.reserve(right_stack.size() + distance(path.begin(), path.end()));
            right_stack.insert(right_stack.end(), path.begin(), path.end());
        }
        else
        {
            vector<int> reverse_path = get_reverse_path(path);
            right_stack.reserve(right_stack.size() + distance(reverse_path.begin(), reverse_path.end()));
            right_stack.insert(right_stack.end(), reverse_path.begin(), reverse_path.end());

            right_stack.reserve(right_stack.size() + distance(path.begin(), path.end()));
            right_stack.insert(right_stack.end(), path.begin(), path.end());
        }
    }

    cout << "right_Stack:"
         << "\n";
    for (size_t i = 0; i < right_stack.size(); i++)
    {
        cout << right_stack[i] << "|";
    }
    cout << "\n";
}

vector<vector<int>> PickStrategy::get_pick_order(int dir)
{
    if (dir == LEFT)
    {
        return order_left;
    }
    return order_right;
}

vector<int> PickStrategy::get_reverse_path(vector<int> path)
{
    vector<int> reverse;
    for (int i = path.size() - 1; i > -1; i--)
    {
        reverse.push_back(get_opposite_dir(path[i]));
    }
    return reverse;
}

int PickStrategy::check_combination(vector<int> pos, vector<int> pos_opposite, Maze *m)
{
    bool left_pos = check_one_combination(pos, m->left_colors, m);
    bool right_pos_opposite = check_one_combination(pos_opposite, m->right_colors, m);
    bool left_pos_opposite = check_one_combination(pos_opposite, m->left_colors, m);
    bool right_pos = check_one_combination(pos, m->right_colors, m);

    if (right_pos && left_pos_opposite && left_pos && right_pos_opposite)
    {
        return ANY;
    }
    else if (right_pos && left_pos_opposite && !left_pos && !right_pos_opposite)
    {
        return RIGHT;
    }
    else if (!right_pos && !left_pos_opposite && left_pos && right_pos_opposite)
    {
        return LEFT;
    }
    else
    {
        return INVALID;
    }
}
bool PickStrategy::check_one_combination(vector<int> pos, vector<vector<vector<int>>> robot_found_colors, Maze *m)
{
    vector<bool> founded{false, false, false};

    for (size_t j = 0; j < robot_found_colors.size(); j++)
    {
        for (size_t i = 0; i < robot_found_colors[j].size(); i++)
        {
            if (robot_found_colors[j][i][0] == m->colored_junctions[j][pos[j]][0] && robot_found_colors[j][i][1] == m->colored_junctions[j][pos[j]][1]) /* code */
            {
                founded[j] = true;
            }
        }
    }

    bool valid = true;
    for (size_t i = 0; i < founded.size(); i++)
    {
        if (!founded[i])
        {
            valid = false;
        }
    }

    return valid;
}