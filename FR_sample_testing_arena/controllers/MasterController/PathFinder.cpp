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

#define RESERVED 0
#define UNRESERVED 1
using namespace std;

PathFinder::PathFinder(int startCol, int startRow, Maze *c_maze, int dir, PickStrategy *pickStrategy)
{
    robot_col = startCol;
    robot_row = startRow;
    maze = c_maze;
    pick_strategy = pickStrategy;
    robot = dir;
    vector<int> priority{RIGHT, DOWN, UP, LEFT};
    last_direction = LEFT;
    if (robot == LEFT)
    {
        last_direction = RIGHT;
        vector<int> priorityLeft{LEFT, DOWN, UP, RIGHT};
        priority = priorityLeft;
    }
    strategy = new Strategy(maze, priority);
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
    cout << "robot: (" << robot_col << "," << robot_row << "): " << robot << "\n";
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
        zero_achieved = true;
        came_back = false;
        check_and_set_available_direction();
        packet = create_next_data_packet();
        last_direction = robot;
        return packet;
    }
    else if ((robot == LEFT && robot_col == -1 && robot_row == 0) || (robot == RIGHT && robot_col == 9 && robot_row == 6))
    {
        int dir = RIGHT;
        if (robot == RIGHT)
        {
            dir = LEFT;
        }
        last_direction = dir;
        direction_to_travel = dir;
        placement = !placement;
        check_and_set_available_direction();
        return create_next_data_packet();
    }
    else
    {
        vector<vector<int>> packet;
        direction_to_travel = pick_strategy->find_next_direction_pick(robot, maze);
        maze->junctions[robot_col][robot_row].travel_state = UNRESERVED;
        get_next_junc_color();
        if (!waiting_for_top)
        {
            pick_junc_available = check_and_set_available_direction();
        }
        else
        {
            pick_junc_available = false;
        }
        maze->junctions[robot_col][robot_row].travel_state = RESERVED;
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
    if (maze->junctions[robot_col][robot_row].get_state() != DISCOVERED)
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
        if (maze->junctions[robot_col][robot_row].content_state == WHITE_PATCH)
        {
            white_box = true;
        }
        junction_content_state = maze->junctions[robot_col][robot_row].content_state;
        paths = maze->junctions[robot_col][robot_row].get_paths();
        junction_content = maze->junctions[robot_col][robot_row].get_content();
    }

    maze->update_junction(robot_col, robot_row, junction_content, junction_content_state, has_white, robot);

    if (junction_content_state != INVERTED || has_white)
    {
        maze->update_path(robot_col, robot_row, paths, robot);
    }

    direction_to_travel = strategy->find_next_direction(robot_col, robot_row, last_direction, has_white);

    if (junction_content_state == INVERTED && has_white)
    {
        white_box = false;
    }

    has_white = white_box;
    maze->junctions[robot_col][robot_row].travel_state = UNRESERVED;
    junc_available = check_and_set_available_direction();
    maze->junctions[robot_col][robot_row].travel_state = RESERVED;
    packet = create_next_data_packet();
    if (junc_available && (direction_to_travel != INVALID))
    {
        last_direction = direction_to_travel;
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
    vector<int> over{scan_over};

    if (!junc_available || !pick_junc_available || waiting_for_top || (direction_to_travel == INVALID))
    {
        navigate_state.push_back(STALL);
    }
    else if ((robot == LEFT && robot_col == -1 && robot_row == 0 && zero_achieved) || (robot == RIGHT && robot_col == 9 && robot_row == 6 && zero_achieved))
    {
        navigate_state.push_back(NAVIGATE_STATE_VISITED);
        zero_achieved = false;
    }
    else if ((robot == LEFT && robot_col == -1 && robot_row == 0 && placement && !zero_achieved) || (robot == RIGHT && robot_col == 9 && robot_row == 6 && placement && !zero_achieved))
    {
        if (current_pick < 3)
        {
            navigate_state.push_back(PLACEMENT);
        }
        else if (current_pick == 3)
        {
            navigate_state.push_back(PLACEMENT_FULL);
        }
        came_back = true;
    }
    else if ((robot == LEFT && robot_col == 0 && robot_row == 0 && !zero_achieved && came_back) || (robot == RIGHT && robot_col == 8 && robot_row == 6 && !zero_achieved && came_back))
    {
        junc_type[0] = maze->junctions[0][0].content_state;
        navigate_state.push_back(NAVIGATE_STATE_VISITED);
        if (junc_type[0] == COLORED && pick_color_box)
        {
            box_grab[0] = current_pos;
            box_grab[1] = current_color;
        }
    }

    else if (maze->junctions[robot_col][robot_row].get_state() == DISCOVERED)
    {
        junc_type[0] = maze->junctions[robot_col][robot_row].content_state;
        if (!scan_over && !has_white && junc_type[0] == WHITE_PATCH)
        {
            box_grab[1] = WHITE_COL;
            box_grab[0] = LOWER;
            has_white = true;
            maze->junctions[robot_col][robot_row].content_state = PATCHNOBOX;
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

        if (scan_over && junc_type[0] == COLORED && !in_last && pick_color_box)
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

    cout << "data packet: " << robot
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

vector<int> PathFinder::update_robot_position(int direction)
{
    vector<int> loc;
    int row = robot_row;
    int col = robot_col;
    switch (direction)
    {
    case UP:
        row += 1;
        break;

    case RIGHT:
        col += 1;
        break;

    case DOWN:
        row -= 1;
        break;

    case LEFT:
        col -= 1;
        break;

    default:
        cout << "Invalid direction"
             << "\n";
        break;
    }

    loc.push_back(col);
    loc.push_back(row);
    return loc;
}

void PathFinder::get_next_junc_color()
{
    vector<int> loc = update_robot_position(direction_to_travel);
    int state = 0;
    current_color = NOCOLOR;
    waiting_for_top = false;
    state = NEGLECT;
    pick_color_box = false;
    if (current_pick < 3)
    {
        if (maze->colored_junctions[pick_order[current_pick][0]][pick_order[current_pick][1]][0] == loc[0] && maze->colored_junctions[pick_order[current_pick][0]][pick_order[current_pick][1]][1] == loc[1] && !box_carrying)
        {
            pick_color_box = true;
            box_carrying = true;
            cout << "inside color"
                 << "\n";

            state = maze->colored_junctions[pick_order[current_pick][0]][pick_order[current_pick][1]][2];
            cout << "state:" << state << "\n";
            current_color = pick_order[current_pick][0] + 1;

            if (state == LOWER)
            {
                cout << "inside lower"
                     << "\n";
                for (size_t i = 0; i < maze->colored_sequential.size(); i++)
                {
                    if (maze->colored_sequential[i][0] == loc[0] && maze->colored_sequential[i][1] == loc[1] && maze->colored_sequential[i][3] == MIDDLE && maze->colored_sequential[i][4] == false)
                    {
                        waiting_for_top = true;
                        break;
                    }
                }
                if (!waiting_for_top)
                {

                    for (size_t i = 0; i < maze->colored_sequential.size(); i++)
                    {
                        if (maze->colored_sequential[i][0] == loc[0] && maze->colored_sequential[i][1] == loc[1] && maze->colored_sequential[i][3] == state)
                        {
                            maze->colored_sequential[i][4] = true;
                            break;
                        }
                    }
                }
            }
            else
            {
                waiting_for_top = false;
                for (size_t i = 0; i < maze->colored_sequential.size(); i++)
                {
                    if (maze->colored_sequential[i][0] == loc[0] && maze->colored_sequential[i][1] == loc[1])
                    {
                        maze->colored_sequential[i][4] = true;
                        break;
                    }
                }
            }
        }

        if (state == 3)
        {
            cout << "in 3"
                 << "\n";
            state = LOWER;
        }
        if (pick_color_box)
        {
            current_pick += 1;
        }
        current_pos = state;
    }
}

vector<vector<int>> PathFinder::initiate_pick()
{
    pick_order = pick_strategy->get_pick_order(robot);
    vector<vector<int>> packet;
    get_next_junc_color();
    if (pick_color_box)
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

bool PathFinder::check_and_set_available_direction()
{
    vector<int> loc = update_robot_position(direction_to_travel);
    if ((robot_col == -1 && robot_row == 0 && !placement) || (robot_col == 9 && robot_row == 6 && !placement))
    {
        box_carrying = false;
        robot_col = loc[0];
        robot_row = loc[1];
        return true;
    }
    else if ((robot_col == -1 && robot_row == 0 && placement) || (robot_col == 9 && robot_row == 6 && placement))
    {
        return true;
    }
    else if ((loc[0] == -1 && loc[1] == 0) || (loc[0] == 9 && loc[1] == 6))
    {
        robot_col = loc[0];
        robot_row = loc[1];
        return true;
    }
    else if (maze->junctions[loc[0]][loc[1]].travel_state == RESERVED)
    {
        if (scan_over)
        {
            if (robot == LEFT)
            {
                pick_strategy->left_stack.insert(pick_strategy->left_stack.begin(), direction_to_travel);
            }
            else
            {
                pick_strategy->right_stack.insert(pick_strategy->right_stack.begin(), direction_to_travel);
            }
        }
        else
        {
            if (strategy->backtracking)
            {
                strategy->robot_stack.insert(strategy->robot_stack.begin(), direction_to_travel);
            }
            else if (strategy->backtracking_invert || strategy->backtracking_white)
            {
                strategy->shortest_path.insert(strategy->shortest_path.begin(), direction_to_travel);
            }
            else
            {
                strategy->robot_stack.pop_back();
            }
        }

        return false;
    }
    else
    {
        robot_col = loc[0];
        robot_row = loc[1];
        return true;
    }
}
