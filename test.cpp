#include "Maze.h"
#include <bits/stdc++.h>
//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

//squares
#define INVERTED 1
#define WHITE 2
#define RED 3
#define GREEN 4
#define BLUE 5

//size
#define COLS 9
#define ROWS 7

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

void travel_direction(int direction)
{
    switch (direction)
    {
    case 0:
        std::cout << "UP"
                  << "\n";
        break;
    case 1:
        std::cout << "RIGHT"
                  << "\n";
        break;
    case 2:
        std::cout << "DOWN"
                  << "\n";
        break;
    case 3:
        std::cout << "LEFT"
                  << "\n";
        break;

    default:
        std::cout << "UNDEFINED"
                  << "\n";
        break;
    }
}

//returns the array of paths in the following order {UP,RIGHT,DOWN,LEFT}
//if the path is not found or is red return -1. else path not discovered yet put 0. path discovered put 1.

int *find_available_directions()
{
    static int dir[4] = {0};
    dir[2] = 1;
    dir[0] = 1;
    return dir;
}

//returns the array of content in the junction
int *find_junction_content()
{
    static int dir[4] = {0};
    dir[2] = 1;
    dir[0] = 1;
    return dir;
}

bool check_direction(int dir, int *paths)
{
    if (*(paths + dir) == 0)
    {
        return false;
    }
    return true;
}

void travel_maze(Maze maze)
{
    int visited = 0;
    int current_col = 0;
    int current_row = 0;
    int robot_col = 0;
    int robot_row = 0;
    int blocked = false;
    int direction_to_travel = UP;

    while (visited < COLS * ROWS)
    {
        visited += 1;
        int *paths;
        int *junction;

        //get available directions from the current position
        paths = find_available_directions();
        //get state of the junction
        junction = find_junction_content();

        maze.update_junction(current_col, current_row, junction);
        maze.update_path(current_col, current_row, junction);

        if (current_row < ROWS)
        {
            if (current_col < robot_col)
            {
                if (blocked)
                {
                    if (check_direction(UP, paths))
                    {
                        direction_to_travel = UP;
                        robot_row += 1;
                        blocked = false;
                    }
                    else
                    {
                        direction_to_travel = RIGHT;
                        robot_col += 1;
                    }
                }
                else
                {
                    if (check_direction(LEFT, paths))
                    {
                        direction_to_travel = LEFT;
                        robot_col -= 1;
                    }
                    else if (check_direction(UP, paths))
                    {
                        direction_to_travel = UP;
                        robot_row += 1;
                    }
                    else if (check_direction(RIGHT, paths))
                    {
                        direction_to_travel = RIGHT;
                        blocked = true;
                        robot_col += 1;
                    }
                }
            }
            else if (current_col == robot_col)
            {
                int next_row = maze.row_next[current_col];
                if (next_row > current_row)
                {
                    if (check_direction(UP, paths))
                    {
                        direction_to_travel = UP;
                        current_row += 1;
                    }
                    else if (check_direction(RIGHT, paths))
                    {
                        direction_to_travel = RIGHT;
                        blocked = true;
                        robot_col += 1;
                    }
                }
                else
                {
                    if (check_direction(DOWN, paths))
                    {
                        direction_to_travel = DOWN;
                        current_row -= 1;
                        maze.row_next[current_col] += maze.row_next[current_col];
                    }
                }
            }
        }
        else
        {
            if (check_direction(RIGHT, paths))
            {
                direction_to_travel = RIGHT;
                current_col += 1;
                current_row += 0;
            }

        }

        travel_direction(direction_to_travel);
    }
}

void print_maze()
{
    int col = 9;
    int rows = 6;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < col; j++)
        {
            std::cout << "- ";
        }
        std::cout << "\n";
        for (int j = 0; j < col; j++)
        {
            std::cout << "| ";
        }
        std::cout << "\n";
    }
}

int main(int argc, char **argv)
{
    Maze m;
    travel_maze(m);

    return 0;
};
