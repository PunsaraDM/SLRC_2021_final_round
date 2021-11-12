#include "PickStrategy.h"
#include "PathFinder.h"
#include "Strategy.h"
#include "Maze.h"

#ifndef MASTER_H
#define MASTER_H

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
    class Master
    {
    public:
        void communicate();

        vector<vector<int>> get_next_data(int juncType, vector<int> path_state, vector<int> box_type, int robot);

    private:
        Maze maze;
        Strategy strategy;
        vector<int> priorityLeft{LEFT, DOWN, UP, RIGHT};
        vector<int> priorityRight{RIGHT, UP, DOWN, LEFT};
        PickStrategy *pick_strategy = new PickStrategy(maze);
        PathFinder *path_finder_left = new PathFinder(0, 0, LEFT, maze, priority_left);
        PathFinder *path_finder_right = new PathFinder(8, 6, RIGHT, maze, priority_right);
    };
#ifdef __cplusplus
}
#endif

#endif