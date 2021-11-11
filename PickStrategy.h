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
        vector<vector<int>> white_locations;
        void find_combinations(Maze m);
        int find_shortest_path(int col1, int row1, int col2, int row2, int color);
        void discover_shortest_paths(vector<vector<vector<int>>> locations);
        vector<vector<int>> find_order(vector<int> selected, vector<vector<vector<int>>> locations);
        bool check_for_top_boxes(int red, int green, int blue, vector<vector<vector<int>>> locations);
        int get_opposite_dir(int direction);

    private:
        vector<int> shortest_path;
        Maze c_maze;
        vector<vector<vector<int>>> shortest_path_seq;
        vector<int> red_distances;
        vector<int> blue_distances;
        vector<int> green_distances;

        vector<int> selected_left;
        vector<int> selected_right;
    };
#ifdef __cplusplus
}
#endif

#endif