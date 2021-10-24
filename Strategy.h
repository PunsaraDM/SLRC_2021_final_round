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
        int find_next_direction(int target_col, int target_row, int robot_col, int robot_row, Maze maze);
        // bool compare_col(const vector<int> &v1, const vector<int> &v2);
    };
#ifdef __cplusplus
}
#endif

#endif