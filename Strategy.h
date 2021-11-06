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
        int find_next_direction(int target_col, int target_row, int robot_col, int robot_row, Maze maze, int last_direction);
        // bool compare_col(const vector<int> &v1, const vector<int> &v2);
        int find_next_direction_normal(vector<vector<int>> visited, vector<vector<int>> unvisited);
        int find_next_direction_colored_with_one(vector<vector<int>> visited, vector<vector<int>> unvisited);
        int find_next_direction_colored_with_two(vector<vector<int>> visited, vector<vector<int>> unvisited);
        int find_next_direction_inverted(vector<vector<int>> visited, vector<vector<int>> unvisited, int last_direction, Maze maze,int robot_col ,int robot_row);
        int find_next_direction_white(vector<vector<int>> visited, vector<vector<int>> unvisited);
        int find_distance_to_target_position(int target_col, int target_row, int pos_col, int pos_row);
        bool check_existence(vector<vector<int>> arr, int val);
    };
#ifdef __cplusplus
}
#endif

#endif