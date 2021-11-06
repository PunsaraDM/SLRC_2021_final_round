#include "Strategy.h"
#include "Maze.h"
#include "Generator.h"

#ifndef ROBOT_H
#define ROBOT_H

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
    class Robot
    {
    public:
        Robot(int startCol, int startRow, int travel_direction);
        void travel_maze();
        void travel_direction(int direction);
        int *find_available_directions();
        vector<int> find_junction_content();
        int find_junction_content_state();
        bool check_direction(int dir, int *paths);
        void update_robot_position(int direction);

    private:
        Maze maze;
        Strategy strategy;
        Generator generator;
        int travel_dir = LEFT;
        int robot_col = 0;
        int robot_row = 0;
        bool isTravelUp = true;
        int last_direction = UP;
        bool has_white = true;
    };
#ifdef __cplusplus
}
#endif

#endif