#include "Robot.h"
#include <bits/stdc++.h>
#include "Maze.h"

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define INVALID 5

#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE_COL 4
#define NOCOLOR 5

#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4
#define PATCHNOBOX 5

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
// #define NAVIGATE_STATE = (SEARCHED = 0), (VISIT = 1)

//  DIRECTION = DIRECTION INV_PATCH[VALID = TRUE(HAS A WHITE BOX) / INVALID(DOES NOT HAVE A WHITE WHITE BOX), WHITE BOX PLACING DIRECTION] JUNC_TYPE[VISITED_JUNC_TYPE]
//                                  BOX_GRAB [POSITION:NEGELECT 0, LOWER 1, MIDDLE 2, UPPER 3, COLOR:RGB = 123, WHITE = 4, NOCOLOR = 5]

using namespace std;

Robot::Robot(int startCol, int startRow, int travel_direction)
{
    robot_col = startCol;
    robot_row = startRow;
    travel_dir = travel_direction;
}

void Robot::travel_direction(int direction)
{
    switch (direction)
    {
    case 0:
    case 4:
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
    if (generator.maze.junctions[robot_col][robot_row].content_state == WHITE)
    {
        has_white = true;
    }
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

vector<vector<int>> Robot::travel_maze()
{
    direction_to_travel = UP;
    vector<vector<int>> packet;

    while (maze.visited < 63 && maze.discovered < 6)
    {
        int *paths;
        int junction_content_state;
        vector<int> junction_content;

        //get available directions from the current position
        if (maze.junctions[robot_col][robot_row].get_state() != DISCOVERED)
        {
            junction_content_state = find_junction_content_state();
            paths = find_available_directions();
            junction_content = find_junction_content();
        }
        else
        {
            if (maze.junctions[robot_col][robot_row].content_state == WHITE)
            {
                has_white = true;
            }
            junction_content_state = maze.junctions[robot_col][robot_row].content_state;
            paths = maze.junctions[robot_col][robot_row].get_paths();
            junction_content = maze.junctions[robot_col][robot_row].get_content();
        }

        cout << "robot: (" << robot_col << "," << robot_row << ")\n";

        maze.update_junction(robot_col, robot_row, junction_content, junction_content_state, has_white);

        if (junction_content_state != INVERTED || has_white)
        {
            maze.update_path(robot_col, robot_row, paths);
        }

        direction_to_travel = strategy.find_next_direction(robot_col, robot_row, maze, LEFT, last_direction, has_white);

        if (junction_content_state == INVERTED && has_white)
        {
            has_white = false;
        }

        travel_direction(direction_to_travel);
        cout << "has box: " << has_white << '\n';

        update_robot_position(direction_to_travel);
        packet = create_next_data_packet();
        cout << "data packet: "
             << "\n";
        for (int i = 0; i < packet.size(); i++)
        {
            for (int j = 0; j < packet[i].size(); j++)
            {
                cout << packet[i][j] << " | ";
            }
            cout << "\n";
        }
        cout << "------------------------"
             << "\n";
        last_direction = direction_to_travel;
    }
    return packet;
}

vector<vector<int>> Robot::create_next_data_packet()
{
    vector<vector<int>> data_packet;
    vector<int> navigate_state;
    vector<int> direction{get_local_direction()};
    vector<int> inv_patch{has_white, get_invert_box_dir()};
    vector<int> junc_type{NORMAL};
    vector<int> box_grab{NEGLECT, NOCOLOR};
    if (maze.junctions[robot_col][robot_row].get_state() == DISCOVERED)
    {
        navigate_state.push_back(NAVIGATE_STATE_VISITED);
        junc_type[0] = maze.junctions[robot_col][robot_row].content_state;
        for (int i = 0; i < strategy.white_locations.size(); i++)
        {
            vector<int> loc = strategy.white_locations[i];
            if (loc[0] == robot_col && loc[1] == robot_row)
            {
                box_grab[1] = WHITE_COL;
                box_grab[0] = LOWER;
            }
        }
    }
    else
    {
        navigate_state.push_back(NAVIGATE_STATE_SEARCH);
    }
    data_packet.push_back(navigate_state);
    data_packet.push_back(direction);
    data_packet.push_back(inv_patch);
    data_packet.push_back(junc_type);
    data_packet.push_back(box_grab);

    return data_packet;
}

int Robot::get_local_direction()
{
    int local_dir = direction_to_travel - last_direction;
    // cout << "local direction: "
    //      << "direction_to_travel: " << direction_to_travel << " | last_direction" << direction_to_travel << "\n";
    if (local_dir < 0)
    {
        local_dir = 4 + local_dir;
    }
    return local_dir;
}

int Robot::get_invert_box_dir()
{
    int dir = RIGHT;
    if (robot_col == 0 && direction_to_travel == 2 || robot_col == COLS - 1 && direction_to_travel == 0 || robot_row == 0 && direction_to_travel == 1 || robot_row == ROWS - 1 && direction_to_travel == 3)
    {
        dir = LEFT;
    }

    return dir;
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
}
