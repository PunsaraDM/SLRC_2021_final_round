#include "Junction.h"

#ifndef MAZE_H
#define MAZE_H

#define COLS 9
#define ROWS 7

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
        Junction junctions[COLS][ROWS];
        vector<vector<int>> colored_junctions;
        vector<vector<int>> white_junctions;

        Maze();
        void initialize();
        void update_path(int column, int row, vector<int> paths);
        void update_junction(int column, int row, vector<int> content, int junction_content_state, bool has_white);
        void check_inverted(int col, int row);
        string print_content(vector<int> content);

    private:
    };
#ifdef __cplusplus
}
#endif

#endif