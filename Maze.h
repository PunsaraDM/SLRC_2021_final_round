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

        Maze();
        void initialize();
        void update_path(int column, int row, int paths[]);
        void update_junction(int column, int row, vector<int> content,int junction_content_state);
        string print_content(vector<int> content);

    private:
    };
#ifdef __cplusplus
}
#endif

#endif