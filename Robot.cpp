#include "Robot.h"
#include <bits/stdc++.h>
#include "Maze.h"

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define RED 1
#define GREEN 2
#define BLUE 3

#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4

//size
#define COLS 9
#define ROWS 7

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1
#define SKIPPATH 2

using namespace std;

void Robot::travel_direction(int direction)
{
    switch (direction)
    {
    case 0:
        std::cout << "travel_direction: UP"
                  << "\n";
        break;
    case 1:
        std::cout << "travel_direction: RIGHT"
                  << "\n";
        break;
    case 2:
        std::cout << "travel_direction: DOWN"
                  << "\n";
        break;
    case 3:
        std::cout << "travel_direction: LEFT"
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

int *Robot::find_available_directions()
{
    int *paths = generator.maze.junctions[robot_col][robot_row].get_paths();
    int path_state = DISCOVERED;

    //if the junction has more than 1 colored boxed the three paths must be skipped
    if (generator.maze.junctions[robot_col][robot_row].content_state == COLORED && generator.maze.junctions[robot_col][robot_row].get_content().size() > 1)
    {
        path_state = SKIPPATH;
    }

    for (int i = 0; i < 4; i++)
    {
        if (*(paths + i) == UNDISCOVERED)
        {
            *(paths + i) = path_state;
        }
    }
    return paths;
}

//returns the array of content in the junction
vector<int> Robot::find_junction_content()
{
    return generator.maze.junctions[robot_col][robot_row].get_content();
}

int Robot::find_junction_content_state()
{
    return generator.maze.junctions[robot_col][robot_row].content_state;
}

bool Robot::check_direction(int dir, int *paths)
{
    if (*(paths + dir) == 0)
    {
        return false;
    }
    return true;
}

void Robot::travel_maze()
{
    int direction_to_travel = UP;

    while (maze.discovered < 6)
    {
        int *paths;
        int junction_content_state;
        vector<int> junction_content;

        //get available directions from the current position
        paths = find_available_directions();
        junction_content_state = find_junction_content_state();
        //get state of the junction
        junction_content = find_junction_content();

        maze.update_junction(robot_col, robot_row, junction_content, junction_content_state);
        maze.update_path(robot_col, robot_row, paths);

        direction_to_travel = strategy.find_next_direction(target_col, target_row, robot_col, robot_row, maze, last_direction);

        last_direction = direction_to_travel;
        //update_current_location
        travel_direction(direction_to_travel);
        update_robot_position(direction_to_travel);
        update_target_position();
        cout << "------------------------"
             << "\n";
    }
}

void Robot::update_robot_position(int direction)
{

    switch (direction)
    {
    case UP:
        robot_row += 1;
        break;

    case RIGHT:
        robot_col += 1;
        break;

    case DOWN:
        robot_row -= 1;
        break;

    case LEFT:
        robot_col -= 1;
        break;

    default:
        cout << "Invalid direction"
             << "\n";
        break;
    }
    cout << "robot: (" << robot_col << "," << robot_row << ")\n";
}

void Robot::update_target_position()
{
    if (robot_col == target_col && robot_row == target_row)
    {
        bool found = false;
        while (!found)
        {
            if (isTravelUp && target_row == ROWS - 1)
            {
                target_col += 1;
                target_row = ROWS - 1;
                isTravelUp = false;
            }
            else if (!isTravelUp && target_row == 0)
            {
                target_col += 1;
                target_row = 0;
                isTravelUp = true;
            }
            else
            {
                if (isTravelUp)
                {
                    target_row += 1;
                }
                else
                {
                    target_row -= 1;
                }
            }
            if (maze.junctions[target_col][target_row].get_state() == UNDISCOVERED)
            {
                found = true;
            }
        }
        cout << "target: (" << target_col << "," << target_row << ")\n";
    }
}
