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

#define DOUBLETOP 1
#define DOUBLEBOTTOM 2
#define TOP 3
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

vector<vector<int>> PathFinder::travel_maze(int juncType, vector<int> path_state, vector<int> box_type)
{
    if (!scan_over)
    {
        cout << "junc content color received:"
             << "\n";
        for (size_t i = 0; i < 2; i++)
        {
            cout << box_type[i] << "\n";
        }
        return search_maze(juncType, path_state, box_type);
    }
    else
    {
        return travel_with_color();
    }
}

vector<vector<int>> PathFinder::travel_with_color()
{
    vector<vector<int>> packet;
    cout << "robot: (" << robot_col << "," << robot_row << ")\n";
    direction_to_travel = pick_strategy.find_next_direction_pick(robot_col, robot_row, LEFT, maze);
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
    return packet;
}

//juncType,pathState,boxType
vector<vector<int>> PathFinder::search_maze(int juncType, vector<int> path_state, vector<int> box_type)
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
        junction_content = box_type;
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

    if (maze.discovered == 6)
    {
        cout << "over"
             << "\n";
        scan_over = true;
        pick_strategy.left_start_col = robot_col;
        pick_strategy.left_start_row = robot_row;
        pick_strategy.initialize(maze);
        return travel_with_color();
    }
    else
    {

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
        return packet;
    }
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
        if (!scan_over && !has_white)
        {
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
        else if (scan_over && junc_type[0] == COLORED)
        {
            get_next_junc_color();
            box_grab[0] = current_pos;
            box_grab[1] = current_color;
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

void PathFinder::get_next_junc_color()
{
    bool found = false;
    int count = 0;
    int index = 0;
    current_color = NOCOLOR;
    for (size_t i = 0; i < maze.colored_sequential.size(); i++)
    {
        if (maze.colored_sequential[i][0] == robot_col && maze.colored_sequential[i][1] == robot_row)
        {
            if (!found)
            {
                current_color = maze.colored_sequential[i][2];
                found = true;
                index = i;
            }
            count += 1;
        }
    }

    if (found)
    {
        maze.colored_sequential.erase(maze.colored_sequential.begin() + index);
    }

    if (count == 2)
    {
        current_pos = MIDDLE;
    }
    else if (count == 1)
    {
        current_pos = LOWER;
    }
    else
    {
        current_pos = NEGLECT;
    }
}