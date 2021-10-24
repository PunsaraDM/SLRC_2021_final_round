#include "Maze.h"

#ifndef Generator_H
#define Generator_H

#define COLS 9
#define ROWS 7

#ifdef __cplusplus
extern "C"
{
#endif
    class Generator
    {
    public:
        int row_next[COLS] = {0};
        Maze maze;

        Generator();
        void initialize();
        void set_red_paths();
        void set_white_junctions();
        void set_inverted_junctions();
        void set_color_box_junctions();
        void print_maze();

    private:
        int min_red_path_count = 10;
        int max_red_path_count = 20;
        int min_white_junction = 4;
        int max_white_junction = 7;
        int min_inverted_junction = 4;
        int max_inverted_junction = 8;
        int min_box = 3;
        int max_box = 6;
    };
#ifdef __cplusplus
}
#endif

#endif