#include "Maze.h"
#include <bits/stdc++.h>
#include <cmath>

using namespace std;

#ifndef STRATEGY_H
#define STRATEGY_H
// directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define INVALID 5

#ifdef __cplusplus
extern "C"
{
#endif
    class Strategy
    {
    public:
        vector<vector<int>> white_locations;
        Strategy(Maze *c_maze, vector<int> priority_vec);
        int find_next_direction(int robot_col, int robot_row, int last_direction, bool has_white);
        int find_next_direction_normal(vector<int> visited, vector<int> unvisited);
        int get_from_priority(vector<int> juncs, bool is_visited);
        int get_opposite_dir(int direction);
        int find_shortest_path(int col1, int row1, int col2, int row2);
        vector<int> get_reverse_path(vector<int> path);
        void add_to_stack(vector<int> seq);

        vector<int> shortest_path;
        vector<int> robot_stack;
        bool backtracking_white = false;
        bool backtracking_invert = false;
        bool backtracking = false;

    private:
        Maze *maze;
        int current_col = 0;
        int current_row = 0;
        int invert_row = 0;
        int invert_col = 0;
        vector<int> priority{LEFT, DOWN, UP, RIGHT};
    };
#ifdef __cplusplus
}
#endif

#endif