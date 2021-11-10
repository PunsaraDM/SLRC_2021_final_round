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
        vector<vector<int>> white_locations;

        int find_next_direction(int robot_col, int robot_row, Maze maze, int robot, int last_direction, bool has_white);
        int find_next_direction_normal(vector<int> visited, vector<int> unvisited, int robot);
        int find_distance_to_target_position(int target_col, int target_row, int pos_col, int pos_row);
        bool check_existence(vector<int> arr, int val);
        int get_from_priority(vector<int> juncs, int robot, bool is_visited);
        int get_opposite_dir(int direction);
        int find_shortest_path(int col1, int row1, int col2, int row2);
        int find_visited_next(vector<int> juncs);

    private:
        vector<int> shortest_path;
        Maze c_maze;
        vector<int> robot_left_stack;
        vector<int> robot_right_stack;
        int current_col = 0;
        int current_row = 0;
        int invert_row = 0;
        int invert_col = 0;
        bool backtracking_white = false;
        bool backtracking_invert = false;
    };
#ifdef __cplusplus
}
#endif

#endif