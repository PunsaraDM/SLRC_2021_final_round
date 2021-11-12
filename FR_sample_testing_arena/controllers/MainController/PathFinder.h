#include "Strategy.h"
#include "Maze.h"

#ifndef PATHFINDER_H
#define PATHFINDER_H

#define UNDISCOVERED 0
#define DISCOVERED 1

#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

#define RED 1
#define GREEN 2
#define BLUE 3

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#ifdef __cplusplus
extern "C"
{
#endif
    class PathFinder
    {
    public:
        PathFinder(int startCol, int startRow, int robot);
        vector<vector<int>> travel_maze(int juncType, vector<int> path_state, vector<int> box_type);
        void travel_direction(int direction);
        vector<int> find_junction_content(vector<int> box_type);
        int find_junction_content_state();
        void update_robot_position(int direction);
        vector<vector<int>> create_next_data_packet();
        int get_local_direction();
        int get_invert_box_dir();
        vector<int> adjust_path_state_to_global(vector <int> paths);

    private:
        Maze maze;
        Strategy strategy;
        int robot;
        int travel_dir = LEFT;
        int direction_to_travel = UP;
        int robot_col = 0;
        int robot_row = 0;
        bool isTravelUp = true;
        int last_direction = RIGHT;
        bool has_white = true;
        bool scan_over = false;
    };
#ifdef __cplusplus
}
#endif

#endif