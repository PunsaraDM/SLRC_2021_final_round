#include "Master.h"
#include "PathFinder.h"
#include <bits/stdc++.h>
#include "Maze.h"
#include <webots/Receiver.hpp>
#include <webots/Emitter.hpp>
#include <iostream>
#include <math.h>
#include <webots/Robot.hpp>
#include <webots/LED.hpp>
#include <string>
#include <vector>
#include <cstring>

#define TIME_STEP 64

using namespace std;
using namespace webots;

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define INVALID 5

Master::Master()
{
    initMaster();
}

void Master::initMaster()
{
    maze = new Maze();
    pick_strategy = new PickStrategy();
    pathfinder_left = new PathFinder(0, 0, maze, LEFT, pick_strategy);
    pathfinder_right = new PathFinder(8, 6, maze, RIGHT, pick_strategy);
    for (int i = 0; i < rx_count; i++)
    {
        receiver[i] = getReceiver(rx_name[i]);
        receiver[i]->enable(TIME_STEP);
    }
    for (int i = 0; i < tx_count; i++)
    {
        emitter[i] = getEmitter(tx_name[i]);
        // emmiter[i]->enable(TIME_STEP);
    }
}

void Master::main_control()
{
    if (!(pathfinder_left->pick_junc_available) && !(pathfinder_right->pick_junc_available))
    {
        find_back_path(0, pick_strategy->left_stack);
        find_back_path(1, pick_strategy->right_stack);

        if (lcount <= rcount)
        {
            pick_strategy->add_to_stack(RIGHT, rightbackpath);
        }
        else
        {
            pick_strategy->add_to_stack(LEFT, leftbackpath);
        }
    }

    else if (!(pathfinder_left->junc_available) && !(pathfinder_right->junc_available))
    {
        if (pathfinder_left->strategy->backtracking_white || pathfinder_left->strategy->backtracking_invert)
            find_back_path(0, pathfinder_left->strategy->shortest_path);
        else
            find_back_path(0, pathfinder_left->strategy->robot_stack);

        if (pathfinder_right->strategy->backtracking_white || pathfinder_right->strategy->backtracking_invert)
            find_back_path(1, pathfinder_right->strategy->shortest_path);
        else
            find_back_path(1, pathfinder_right->strategy->robot_stack);

        if (lcount <= rcount)
        {
            pathfinder_right->strategy->add_to_stack(rightbackpath);
        }
        else
        {
            pathfinder_left->strategy->add_to_stack(leftbackpath);
        }
    }
    else if (receiver[rx]->getQueueLength() > 0)
    {
        searchFinish = false;
        receive(rx);
        if (maze->discovered == 6 && scan_just_over && (maze->paths_joined || maze->color_match()))
        {
            msgCount += 1;
            cout << "mesg" << msgCount << "," << rx << "\n";
            if (msgCount == 1)
            {
                if (rx == 0)
                {
                    cout << "left:" << pathfinder_left->robot_col << "," << pathfinder_left->robot_row;
                    maze->junctions[pathfinder_left->robot_col][pathfinder_left->robot_row].set_state(DISCOVERED);
                }
                else
                {
                    cout << "right:" << pathfinder_right->robot_col << "," << pathfinder_right->robot_row;
                    maze->junctions[pathfinder_right->robot_col][pathfinder_right->robot_row].set_state(DISCOVERED);
                }

                rx = rx + 1;
                if (rx > 1)
                    rx = 0;
                searchFinish = true;
            }
            else if (msgCount == 2)
            {
                if (rx == 0)
                {
                    cout << "left:" << pathfinder_left->robot_col << "," << pathfinder_left->robot_row;
                    maze->junctions[pathfinder_left->robot_col][pathfinder_left->robot_row].set_state(DISCOVERED);
                }
                else
                {
                    cout << "right:" << pathfinder_right->robot_col << "," << pathfinder_right->robot_row;
                    maze->junctions[pathfinder_right->robot_col][pathfinder_right->robot_row].set_state(DISCOVERED);
                }
                cout << "all six found"
                     << "\n";
                scan_just_over = false;
                one_processing = false;

                pick_strategy->initialize(maze, pathfinder_left->robot_col, pathfinder_left->robot_row, pathfinder_right->robot_col, pathfinder_right->robot_row, maze->color_match());

                pathfinder_left->scan_just_over = true;
                pathfinder_left->scan_over = true;
                pathfinder_right->scan_just_over = true;
                pathfinder_right->scan_over = true;

                var = pathfinder_left->initiate_pick();
                emmit(0);
                var = pathfinder_right->initiate_pick();
                emmit(1);
            }
        }
        else
        {
            if (rx == 0)
            {
                cout << "Called 3"
                     << "\n";

                var = pathfinder_left->travel_maze(juncTypeRx, pathStateRx, boxTypeRx);
                emmit(rx);
            }
            else
            {
                cout << "Called 4"
                     << "\n";

                var = pathfinder_right->travel_maze(juncTypeRx, pathStateRx, boxTypeRx);

                emmit(rx);
            }
        }
    }

    if (!searchFinish)
    {
        rx = rx + 1;
        if (rx > 1)
            rx = 0;
    }
}

void Master::find_back_path(int robot, vector<int> forward_path)
{
    vector<int> backpath{};
    int leftcol = pathfinder_left->robot_col;
    int leftrow = pathfinder_left->robot_row;

    int rightcol = pathfinder_right->robot_col;
    int rightrow = pathfinder_right->robot_row;

    if (robot == 0)
    {
        forwardcol = leftcol;
        forwardrow = leftrow;
        backcol = rightcol;
        backrow = rightrow;
    }
    else
    {
        forwardcol = rightcol;
        forwardrow = rightrow;
        backcol = leftcol;
        backrow = leftrow;
    }

    bool found_way_out = false;
    int count = 0;

    if (forward_path.size() == 1)
    {
        count = 100000;
    }
    else
    {
        for (size_t i = 0; i < forward_path.size(); i++)
        {
            vector<vector<int>> achievables{{backcol, backrow + 1},
                                            {backcol + 1, backrow},
                                            {backcol, backrow - 1},
                                            {backcol - 1, backrow}};

            vector<int> target{0, 0};
            if (i != forward_path.size() - 1)
            {
                target = update_robot_position(forward_path[i + 1], backcol, backrow);
            }

            for (int j = 0; j < 4; j++)
            {
                if ((maze->junctions[achievables[j][0]][achievables[j][1]].get_state() == DISCOVERED) && !(forwardcol == achievables[j][0] && forwardrow == achievables[j][1]))
                {
                    if (forward_path.size() - 1 == i)
                    {
                        backpath.push_back(j);
                        found_way_out = true;
                        break;
                    }
                    else if (!(target[0] == achievables[j][0] && target[1] == achievables[j][1]))
                    {
                        backpath.push_back(j);
                        found_way_out = true;
                        break;
                    }
                }
            }

            if (found_way_out)
            {
                break;
            }
            else
            {
                backpath.push_back(forward_path[i + 1]);
                count = count + 1;
            }

            vector<int> leftnext = update_robot_position(forward_path[i], forwardcol, forwardrow);
            vector<int> rightnext = update_robot_position(backpath[i], backcol, backrow);

            forwardcol = leftnext[0];
            forwardrow = leftnext[1];
            backcol = rightnext[0];
            backrow = rightnext[1];
        }
    }

    if (robot == 0)
    {
        lcount = count;
        rightbackpath = backpath;
    }
    else
    {
        rcount = count;
        leftbackpath = backpath;
    }
}

// void Master::main_control()
// {
//     while(master->step(TIME_STEP) != -1)
//     {
//         // current_left_robot_col = pathfinder_left -> robot_col;
//         // current_left_robot_row = pathfinder_left -> robot_row;

//         // current_right_robot_col = pathfinder_right -> robot_col;
//         // current_right_robot_row = pathfinder_right -> robot_row;

//         for{int i==0, i<2, i++}
//         {
//             if (receiver[i]->getQueueLength() > 0)
//             {
//             receive(i);
//             if (i==0) var[i] = pathfinder_left->travel_maze(juncTypeRx[i], pathStateRx[i], boxTypeRx[i]);
//             else var[i] = pathfinder_right->travel_maze(juncTypeRx[i], pathStateRx[i], boxTypeRx[i]);
//         }

//         target_left_robot_col = pathfinder_left -> robot_col;
//         target_left_robot_row = pathfinder_left -> robot_row;

//         target_right_robot_col = pathfinder_right -> robot_col;
//         target_right_robot_row = pathfinder_right -> robot_row;

//         if ((target_left_robot_col==current_right_robot_col) && (target_left_robot_row == current_right_robot_row) ) && ((target_right_robot_col==current_left_robot_col) && (target_right_robot_row==current_left_robot_row))
//         {
//             //back one robot out of the path of other robot
//         }

//         else if ((target_left_robot_col == target_right_robot_col) && (target_left_robot_row==target_right_robot_row))
//         {
//             //one robot wait and other proceeds
//             emmit(0);

//         }

//         else
//         {
//             emmit(0);
//             emmit(1);
//         }

//     }

// }

// void Master::update_maze(int col, int row, int juncType, vector<int> path_state, vector<int> box_type)
// {
//     // paths = adjust_path_state_to_global(path_state);
//     // junction_content_state = juncType;
//     // junction_content = box_type;
//     maze->junctions[col][row].set_state(DISCOVERED);
//     // pathfinder_left->adjust_path_state_to_global(path_state);
//     // maze->update_junction(pathfinder_left->robot_col, pathfinder_left->robot_row, juncTypeRx, boxTypeRx, pathfinder_left->has_white, LEFT);
//     // if (junction_content_state != INVERTED || has_white)
//     // {
//     //     maze->update_path(pathfinder_left->robot_col, pathfinder_left->robot_row, boxTypeRx, LEFT);
//     // }
// }

void Master::receive(int rx)
{

    string RxMessage((const char *)receiver[rx]->getData());

    receiver[rx]->nextPacket();

    // for (int i = 0; i < 7; i++)
    // {
    //   tempStr = TxMessage[i];
    //   if (i == 0)
    //     var[0][0] = stoi(tempStr);
    //   else if (i == 1)
    //     var[1][0] = stoi(tempStr);
    //   else if (i == 2)
    //     var[2][0] = stoi(tempStr);
    //   else if (i == 3)
    //     var[2][1] = stoi(tempStr);
    //   else if (i == 4)
    //     var[3][0] = stoi(tempStr);
    //   else if (i == 5)
    //     var[4][0] = stoi(tempStr);
    //   else if (i == 6)
    //     var[4][1] = stoi(tempStr);
    // }

    //cout << "rxmsg: " << var[0][0] << var[1][0] << var[2][0] << var[2][1] << var[3][0] << var[4][0] << var[4][1] << endl;

    string tempStr = "";
    size_t i = 0;
    int j = 0;
    while (i < RxMessage.size())
    {
        tempStr = RxMessage[i];
        if (tempStr == "-")
        {
            i = i + 1;
            tempStr = tempStr + RxMessage[i];
        }
        //cout<<"temp"<<tempStr<<endl;
        if (j == 0)
            juncTypeRx = stoi(tempStr);
        else if (j == 1)
            pathStateRx[0] = stoi(tempStr);
        else if (j == 2)
            pathStateRx[1] = stoi(tempStr);
        else if (j == 3)
            pathStateRx[2] = stoi(tempStr);
        else if (j == 4)
            pathStateRx[3] = stoi(tempStr);
        else if (j == 5)
            boxTypeRx[0] = stoi(tempStr);
        else if (j == 6)
            boxTypeRx[1] = stoi(tempStr);
        i = i + 1;
        j = j + 1;
    }

    // cout << "juncType: " << juncTypeRx << endl;
    // cout << "pathState: " << pathStateRx[0] << pathStateRx[1] << pathStateRx[2] << pathStateRx[3] << endl;
    // cout << "boxType: " << boxTypeRx[0] << boxTypeRx[1] << endl;
}

void Master::emmit(int tx)
{
    string TxMessage = "";

    for (size_t i = 0; i < var.size(); i++)
    {
        for (size_t j = 0; j < var[i].size(); j++)
        {
            TxMessage = TxMessage + to_string(var[i][j]);
        }
    }

    cout << "txmsg: " << TxMessage << endl;
    emitter[tx]->send(TxMessage.c_str(), (int)strlen(TxMessage.c_str()) + 1);
}

vector<int> Master::update_robot_position(int direction, int robot_col, int robot_row)
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