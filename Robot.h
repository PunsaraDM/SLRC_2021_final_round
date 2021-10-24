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


#ifdef __cplusplus
extern "C"
{
#endif
    class Robot
    {
    public:
        void travel_maze();
        void travel_direction(int direction);
        int *find_available_directions();
        vector<int> find_junction_content();
        bool check_direction(int dir, int *paths);
        void update_robot_position(int direction);
        void update_target_position();

    private:
        Maze maze;
        Strategy strategy;
        Generator generator;
        int robot_col = 0;
        int robot_row = 0;
        int target_col = 0;
        int target_row = 1;
        bool isTravelUp = true;
    };
#ifdef __cplusplus
}
#endif

#endif