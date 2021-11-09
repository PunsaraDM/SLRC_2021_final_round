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
        int find_next_direction(int robot_col, int robot_row, Maze maze, int robot, int last_direction, bool has_white);
        int find_next_direction_normal(vector<int> visited, vector<int> unvisited, int robot);
        int find_distance_to_target_position(int target_col, int target_row, int pos_col, int pos_row);
        bool check_existence(vector<int> arr, int val);
        int get_from_priority(vector<int> juncs, int robot, bool is_visited);
        int get_opposite_dir(int direction);
        void find_shortest_path(int col, int row);
        int find_visited_next(vector<int> juncs);
    private:
        vector<int> shortest_path;
        Maze c_maze;
        vector<int> invert_path;
        vector<int> robot_left_stack;
        vector<int> robot_right_stack;
        vector<vector<int>> white_locations;
        int current_col;
        int current_row;
        bool backtracking_white = false; 
        bool backtracking_invert = false; 
    };
#ifdef __cplusplus
}
#endif

#endif