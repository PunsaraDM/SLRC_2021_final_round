#include "PathFinder.h"
#include <bits/stdc++.h>
#include "Maze.h"
#include "PickStrategy.h"

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define STALL 4
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
#define PLACEMENT 2
#define PLACEMENT_FULL 3

#define DIRECTION 1
#define INV_PATCH 2
#define JUNC_TYPE 3
#define BOX_GRAB 4
#define NEGLECT 0
#define LOWER 1
#define MIDDLE 2

using namespace std;

PathFinder::PathFinder(int startCol, int startRow, Maze c_maze, int dir, PickStrategy pickStrategy)
{
    robot_col = startCol;
    robot_row = startRow;
    maze = c_maze;
    pick_strategy = pickStrategy;
    robot = dir;
    strategy = new Strategy(maze);
}

vector<int> PathFinder::adjust_path_state_to_global(vector<int> paths)
{
    vector<int> temp_paths;
    for (int i = 0; i < 4; i++)
    {
        int local_dir = i - last_direction;
        if (local_dir < 0)
        {
            local_dir = 4 + local_dir;
        }
        temp_paths.push_back(paths[local_dir]);
    }
    return temp_paths;
}

vector<vector<int>> PathFinder::travel_maze(int juncType, vector<int> path_state, vector<int> box_type)
{
    cout << "robot: (" << robot_col << "," << robot_row << ")\n";
    if (!scan_over)
    {
        return search_maze(juncType, path_state, box_type);
    }
    else
    {
        return travel_with_color();
    }
}

vector<vector<int>> PathFinder::travel_with_color()
{
    if ((robot == LEFT && robot_col == 0 && robot_row == 0 && last_direction != RIGHT) || (robot == RIGHT && robot_col == 8 && robot_row == 6 && last_direction != LEFT))
    {
        vector<vector<int>> packet;
        direction_to_travel = robot;
        update_robot_position(direction_to_travel);
        packet = create_next_data_packet();
        last_direction = robot;
        return packet;
    }
    else if ((robot == LEFT && robot_col == -1 && robot_row == 0) || (robot == LEFT && robot_col == 9 && robot_row == 6))
    {
        int dir = RIGHT;
        if (robot == RIGHT)
        {
            dir = LEFT;
        }
        last_direction = dir;
        direction_to_travel = dir;
        if (placement_back)
        {
            update_robot_position(direction_to_travel);
        }
        return create_next_data_packet();
    }
    else
    {
        vector<vector<int>> packet;
        direction_to_travel = pick_strategy.find_next_direction_pick(robot, maze);
        update_robot_position(direction_to_travel);
        packet = create_next_data_packet();
        last_direction = direction_to_travel;
        return packet;
    }
}

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

        if (robot == LEFT)
        {
            pick_order = pick_strategy.order_left;
        }
        else
        {
            pick_order = pick_strategy.order_right;
        }
        if (get_next_junc_color())
        {
            in_last = true;
            packet = create_next_data_packet();
            return packet;
        }
        else
        {
            return travel_with_color();
        }
    }
    else
    {

        direction_to_travel = strategy->find_next_direction(robot_col, robot_row, maze, LEFT, last_direction, has_white);

        if (junction_content_state == INVERTED && has_white)
        {
            white_box = false;
        }

        has_white = white_box;
        update_robot_position(direction_to_travel);
        packet = create_next_data_packet();
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
    vector<int> over{scan_over};

    if ((scan_over && robot_col == -1 && robot_row == 0 && robot == LEFT) || (scan_over && robot_col == 9 && robot_row == 6 && robot == RIGHT))
    {
        if (current_pick < 3 && placement_back)
        {
            junc_type[0] = maze.junctions[0][0].content_state;
            navigate_state.push_back(NAVIGATE_STATE_VISITED);
            placement_back = false;
            if (junc_type[0] == COLORED && get_next_junc_color())
            {
                box_grab[0] = current_pos;
                box_grab[1] = current_color;
            }
        }
        else if (current_pick < 3 && !placement_back)
        {
            navigate_state.push_back(PLACEMENT);
            placement_back = true;
        }
        else if (current_pick == 3)
        {
            navigate_state.push_back(PLACEMENT_FULL);
        }
    }

    else if (maze.junctions[robot_col][robot_row].get_state() == DISCOVERED)
    {
        junc_type[0] = maze.junctions[robot_col][robot_row].content_state;
        if (!scan_over && !has_white)
        {
            for (size_t i = 0; i < strategy->white_locations.size(); i++)
            {
                vector<int> loc = strategy->white_locations[i];
                if (loc[0] == robot_col && loc[1] == robot_row)
                {
                    box_grab[1] = WHITE_COL;
                    box_grab[0] = LOWER;
                }
            }
            navigate_state.push_back(NAVIGATE_STATE_VISITED);
        }
        else if (in_last)
        {
            navigate_state.push_back(STALL);
            box_grab[0] = current_pos;
            box_grab[1] = current_color;
            in_last = false;
        }
        else
        {
            navigate_state.push_back(NAVIGATE_STATE_VISITED);
        }

        if (scan_over && junc_type[0] == COLORED && !in_last && get_next_junc_color())
        {
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
    data_packet.push_back(over);

    cout << "data packet: "
         << "\n";
    for (size_t i = 0; i < data_packet.size(); i++)
    {
        for (size_t j = 0; j < data_packet[i].size(); j++)
        {
            cout << data_packet[i][j] << " | ";
        }
        cout << "\n";
    }
    cout << "------------------------"
         << "\n";

    return data_packet;
}

int PathFinder::get_local_direction()
{
    int local_dir = direction_to_travel - last_direction;
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

bool PathFinder::get_next_junc_color()
{
    bool found = false;
    int state = 0;
    current_color = NOCOLOR;

    if (maze.colored_junctions[pick_order[current_pick][0]][pick_order[current_pick][1]][0] == robot_col && maze.colored_junctions[pick_order[current_pick][0]][pick_order[current_pick][1]][1] == robot_row)
    {
        found = true;
        current_color = pick_order[current_pick][0] + 1;

        state = maze.colored_junctions[pick_order[current_pick][0]][pick_order[current_pick][1]][2];
    }

    if (state != LOWER && state != MIDDLE)
    {
        state = LOWER;
    }
    if (found)
    {
        current_pick += 1;
    }
    current_pos = state;
    return found;
}
