#include "PathFinder.h"
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
// #define NAVIGATE_STATE = (SEARCHED = 0), (VISIT = 1)

//  DIRECTION = DIRECTION INV_PATCH[VALID = TRUE(HAS A WHITE_PATCH BOX) / INVALID(DOES NOT HAVE A WHITE_PATCH WHITE_PATCH BOX), WHITE_PATCH BOX PLACING DIRECTION] JUNC_TYPE[VISITED_JUNC_TYPE]
//                                  BOX_GRAB [POSITION:NEGELECT 0, LOWER 1, MIDDLE 2, UPPER 3, COLOR:RGB = 123, WHITE_PATCH = 4, NOCOLOR = 5]

using namespace std;

PathFinder::PathFinder(int startCol, int startRow)
{
    robot_col = startCol;
    robot_row = startRow;
}
vector<int> PathFinder::adjust_path_state_to_global(vector<int> paths)
{
    vector<int> temp_paths;
    for (int i = 0; i < 4; i++)
    {
        int local_dir = i - last_direction;
        // cout << "local direction: "
        //      << "direction_to_travel: " << direction_to_travel << " | last_direction" << direction_to_travel << "\n";
        if (local_dir < 0)
        {
            local_dir = 4 + local_dir;
        }
        temp_paths.push_back(paths[local_dir]);
    }
    return temp_paths;
}

void PathFinder::travel_direction(int direction)
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

//returns the array of content in the junction
vector<int> PathFinder::find_junction_content(vector<int> box_type)
{
    vector<int> content;
    for (size_t i = 0; i < box_type.size(); i++)
    {
        int num = box_type[i];
        for (int j = 0; j < num; j++)
        {
            content.push_back(i + 1);
        }
    }
    return content;
}

//juncType,pathState,boxType
vector<vector<int>> PathFinder::travel_maze(int juncType, vector<int> path_state, vector<int> box_type)
{
    vector<vector<int>> packet;
    vector<int> paths;
    int junction_content_state;
    vector<int> junction_content;
    bool white_box = has_white;

    //get available directions from the current position
    if (maze.junctions[robot_col][robot_row].get_state() != DISCOVERED)
    {
        if (juncType == WHITE_PATCH)
        {
            white_box = true;
        }
        paths = adjust_path_state_to_global(path_state);
        junction_content_state = juncType;
        junction_content = find_junction_content(box_type);
    }
    else
    {
        if (maze.junctions[robot_col][robot_row].content_state == WHITE_PATCH)
        {
            white_box = true;
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
        white_box = false;
    }

    travel_direction(direction_to_travel);
    has_white = white_box;
    cout << "has box: " << has_white << '\n';

    update_robot_position(direction_to_travel);
    packet = create_next_data_packet();
    cout << "data packet: "
         << "\n";
    for (size_t i = 0; i < packet.size(); i++)
    {
        for (size_t j = 0; j < packet[i].size(); j++)
        {
            cout << packet[i][j] << " | ";
        }
        cout << "\n";
    }
    cout << "------------------------"
         << "\n";
    last_direction = direction_to_travel;
    if (maze.visited < 63 && maze.discovered < 6)
    {
        scan_over = true;
    }
    return packet;
}

vector<vector<int>> PathFinder::create_next_data_packet()
{
    vector<vector<int>> data_packet;
    vector<int> navigate_state;
    vector<int> direction{get_local_direction()};
    vector<int> inv_patch{has_white, get_invert_box_dir()};
    vector<int> junc_type{NORMAL};
    vector<int> box_grab{NEGLECT, NOCOLOR};
    cout << robot_col << " " << robot_row << endl;
    if (maze.junctions[robot_col][robot_row].get_state() == DISCOVERED)
    {
        navigate_state.push_back(NAVIGATE_STATE_VISITED);
        junc_type[0] = maze.junctions[robot_col][robot_row].content_state;
        if (!has_white){
            for (size_t i = 0; i < strategy.white_locations.size(); i++)
            {
                vector<int> loc = strategy.white_locations[i];
                if (loc[0] == robot_col && loc[1] == robot_row)
                {
                    box_grab[1] = WHITE_COL;
                    box_grab[0] = LOWER;
                }
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

int PathFinder::get_local_direction()
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

int PathFinder::get_invert_box_dir()
{
    int dir = RIGHT;
    if ((robot_col == 0 && direction_to_travel == 2) || (robot_col == COLS - 1 && direction_to_travel == 0) || (robot_row == 0 && direction_to_travel == 1) || (robot_row == ROWS - 1 && direction_to_travel == 3))
    {
        dir = LEFT;
    }

    return dir;
}

void PathFinder::update_robot_position(int direction)
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
