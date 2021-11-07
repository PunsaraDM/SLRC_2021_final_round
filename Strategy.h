#include "Maze.h"
#include <bits/stdc++.h>
#include <cmath>

using namespace std;

#ifndef STRATEGY_H
#define STRATEGY_H

#ifdef __cplusplus
extern "C"
{
#endif
    class Strategy
    {
    public:
        int find_next_direction(int robot_col, int robot_row, Maze maze, int last_direction, bool has_white);
        // bool compare_col(const vector<int> &v1, const vector<int> &v2);
        int find_next_direction_normal(vector<int> visited, vector<int> unvisited);
        int find_next_direction_colored_with_one(vector<int> visited, vector<int> unvisited);
        int find_next_direction_colored_with_two(vector<int> visited, vector<int> unvisited);
        int find_next_direction_inverted(vector<int> visited, vector<int> unvisited, int last_direction, bool has_white);
        int find_next_direction_white(vector<int> visited, vector<int> unvisited);
        int find_distance_to_target_position(int target_col, int target_row, int pos_col, int pos_row);
        bool check_existence(vector<int> arr, int val);
        int get_from_priority(vector<int> juncs, int robot, bool is_visited);
    private:
        vector<vector<int>> robot_left_stack;
        vector<vector<int>> robot_right_stack;
        int current_col;
        int current_row;
        int invert_found = false;
        int invert_dir;
        int invert_col;
        int invert_row;
    };
#ifdef __cplusplus
}
#endif

#endif