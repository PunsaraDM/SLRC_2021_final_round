#include "Junction.h"

#ifndef MAZE_H
#define MAZE_H

#define COLS 9
#define ROWS 7

#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE_COL 4
#define NOCOLOR 5

#define NEGLECT 0
#define LOWER 1
#define MIDDLE 2
#define UPPER 3

#ifdef __cplusplus
extern "C"
{
#endif
    class Maze
    {
    public:
        int row_next[COLS] = {0};
        int discovered = 0;
        int visited = 0;
        bool next_colored = false;
        bool paths_joined = false;
        int color = NOCOLOR;
        int pos = NEGLECT;
        Junction junctions[COLS][ROWS];
        vector<vector<vector<int>>> colored_junctions{vector<vector<int>>(), vector<vector<int>>(), vector<vector<int>>()};
        vector<vector<vector<int>>> right_colors{vector<vector<int>>(), vector<vector<int>>(), vector<vector<int>>()};
        vector<vector<vector<int>>> left_colors{vector<vector<int>>(), vector<vector<int>>(), vector<vector<int>>()};
        vector<vector<int>> colored_sequential;
        vector<vector<int>> white_junctions;

        Maze();
        void initialize();
        void update_path(int column, int row, vector<int> paths, int robot);
        void update_junction(int column, int row, vector<int> content, int junction_content_state, bool has_white, int robot);
        void check_inverted(int col, int row);
        string print_content(vector<int> content);
        void update_path_helper(bool juncs_found, int new_col, int new_row, int robot, int found_dir, int state);
        bool color_match();

    private:
    };
#ifdef __cplusplus
}
#endif

#endif