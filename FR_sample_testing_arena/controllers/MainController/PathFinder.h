#include "PickStrategy.h"
#include "Strategy.h"
#include "Maze.h"

#ifndef PATHFINDER_H
#define PATHFINDER_H

#define UNDISCOVERED 0
#define DISCOVERED 1

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE_COL 4
#define NOCOLOR 5

#define COLORED 1
#define WHITE_PATCH 2
#define INVERTED 3
#define NORMAL 4
#define INVERTWHITE 5
#define PATCHNOBOX 6

//size
#define COLS 9
#define ROWS 7

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1
#define SKIPPATH 2

//junction
#define VISITEDWITHOUTWHITE 2

//data packet structure
#define NAVIGATE_STATE 0
#define NAVIGATE_STATE_SEARCH 0
#define NAVIGATE_STATE_VISITED 1
#define DIRECTION 1
#define INV_PATCH 2
#define JUNC_TYPE 3
#define BOX_GRAB 4
#define NEGLECT 0
#define LOWER 1
#define MIDDLE 2
#define UPPER 3

#ifdef __cplusplus
extern "C"
{
#endif
    class PathFinder
    {
    public:
        PathFinder(int startCol, int startRow);
        vector<vector<int>> travel_maze(int juncType, vector<int> path_state, vector<int> box_type);
        vector<vector<int>> travel_with_color();
        vector<vector<int>> search_maze(int juncType, vector<int> path_state, vector<int> box_type);
        vector<int> find_junction_content(vector<int> box_type);
        int find_junction_content_state();
        void update_robot_position(int direction);
        vector<vector<int>> create_next_data_packet();
        int get_local_direction();
        int get_invert_box_dir();
        vector<int> adjust_path_state_to_global(vector<int> paths);
        void get_next_junc_color();

    private:
        Maze maze;
        Strategy strategy;
        PickStrategy pick_strategy;
        int travel_dir = LEFT;
        int direction_to_travel = UP;
        int robot_col = 0;
        int robot_row = 0;
        bool isTravelUp = true;
        int last_direction = RIGHT;
        bool has_white = true;
        bool scan_over = false;
        int current_color = NOCOLOR;
        int current_pos = LOWER;
    };
#ifdef __cplusplus
}
#endif

#endif