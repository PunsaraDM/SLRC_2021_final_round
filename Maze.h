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
        
        Maze();
        void initialize();
        void update_path(int column, int row,int paths[]);
        void update_junction(int column, int row, int content[]);

    private:
        Junction junctions[COLS][ROWS];

    };
#ifdef __cplusplus
}
#endif

#endif