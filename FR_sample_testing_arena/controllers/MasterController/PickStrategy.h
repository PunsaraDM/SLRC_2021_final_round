#include "Maze.h"
#include <bits/stdc++.h>
#include <cmath>

using namespace std;

#ifndef PICKSTRATEGY_H
#define PICKSTRATEGY_H

#ifdef __cplusplus
extern "C"
{
#endif
    class PickStrategy
    {
    public:
        int left_start_col;
        int left_start_row;
        int right_start_col;
        int right_start_row;
        vector<vector<int>> order_left;
        vector<vector<int>> order_right;
        vector<vector<int>> white_locations;
        vector<int> left_stack;
        vector<int> right_stack;
        void find_combinations(Maze m);
        int find_shortest_path(int col1, int row1, int col2, int row2, int color, int dir, Maze c_maze);
        void discover_shortest_paths(vector<vector<vector<int>>> locations, Maze m);
        vector<vector<int>> find_order(vector<int> selected, vector<vector<vector<int>>> locations);
        vector<int> get_reverse_path(vector<int> path);
        bool check_for_top_boxes(int red, int green, int blue, vector<vector<vector<int>>> locations);
        int get_opposite_dir(int direction);
        int find_next_direction_pick(int robot, Maze maze);
        void initialize(Maze m, int left_col, int left_row, int right_col, int right_row);
        vector<vector<int>> order_lower(vector<vector<vector<int>>> locations, vector<vector<int>> top, vector<vector<int>> lower);
        vector<vector<int>> get_pick_order(int dir);
        bool check_one_combination(vector<int> pos, vector<vector<vector<int>>> robot_found_colors, Maze m);
        int check_combination(vector<int> pos, vector<int> pos_opposite, Maze m);

    private:
        vector<int> shortest_path;
        Maze c_maze;
        vector<vector<vector<int>>> shortest_path_seq;
        vector<int> red_distances;
        vector<int> blue_distances;
        vector<int> green_distances;
        int placement_back = false;
        vector<int> selected_left;
        vector<int> selected_right;
        bool left_initialized = false;
        bool right_initialized = false;
    };
#ifdef __cplusplus
}
#endif

#endif