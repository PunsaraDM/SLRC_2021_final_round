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
        Strategy(Maze maze, vector<int> priority_order);
        int find_next_direction(int robot_col, int robot_row, int last_direction, bool has_white);
        int find_next_direction_normal(vector<int> visited, vector<int> unvisited);
        int get_from_priority(vector<int> juncs, bool is_visited);
        int get_opposite_dir(int direction);
        int find_shortest_path(int col1, int row1, int col2, int row2);

    private:
        vector<int> shortest_path;
        Maze c_maze;
        vector<int> robot_stack;
        int current_col = 0;
        int current_row = 0;
        int invert_row = 0;
        int invert_col = 0;
        bool backtracking_white = false;
        bool backtracking_invert = false;
        vector<int> priority;
        // vector<int> priorityLeft{LEFT, DOWN, UP, RIGHT};
        // vector<int> priorityRight{RIGHT, UP, DOWN, LEFT};
        // vector<int> priority = priorityRight;
    };
#ifdef __cplusplus
}
#endif

#endif