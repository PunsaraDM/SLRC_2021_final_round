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

#define TIME_STEP 64

using namespace std;
using namespace webots;

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


void Master::initMaster(Master *master)
{
    for (int i = 0; i < rx_count; i++)
    {
    receiver[i] = master->getReceiver(rx_name[i]);
    receiver[i]->enable(TIME_STEP);
    }
    for (int i = 0; i < tx_count; i++)
    {
    emmiter[i] = master->getEmmiter(tx_name[i]);
    emmiter[i]->enable(TIME_STEP);
    }
}

void Master::main_control()
{
    int rx = 0;
    while(master->step(TIME_STEP) != -1)
    {
        if (receiver[rx]->getQueueLength() > 0)
        {
            receive(rx);
            if (rx==0) 
            {
                var = pathfinder_left->travel_maze(juncTypeRx, pathStateRx, boxTypeRx); 
                if (maze.junctions[pathfinder_left ->robot_col][pathfinder_left ->robot_row].get_state() == RESERVED)
                {
                    
                }
                else
                {
                    maze.junctions[pathfinder_left ->robot_col][pathfinder_left ->robot_row].set_state() = RESERVED;
                    emmit(rx);
                }
            }
            else 
            {
                var = pathfinder_right->travel_maze(juncTypeRx, pathStateRx, boxTypeRx);
                if (maze.junctions[pathfinder_right ->robot_col][pathfinder_right ->robot_row].get_state() == RESERVED)
                {

                }
                else
                {
                    maze.junctions[pathfinder_right ->robot_col][pathfinder_right ->robot_row].set_state() = RESERVED;
                    emmit(rx);
                }
            }
        }
        rx = rx + 1;
        if (rx > 1) rx=0;

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

void Master::receive(int rx)
{

    string RxMessage((const char *)receiver[rx]->getData());

    cout <<"received msg "<< RxMessage << endl;
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

    string tempStr;
    for (int i = 0; i < 7; i++)
    {
    tempStr = RxMessage[i];
    if (i == 0)
        juncTypeRx = stoi(tempStr);
    else if (i == 1)
        pathStateRx[0] = stoi(tempStr);
    else if (i == 2)
        pathStateRx[1] = stoi(tempStr);
    else if (i == 3)
        pathStateRx[2] = stoi(tempStr);
    else if (i == 4)
        pathStateRx[3] = stoi(tempStr);
    else if (i == 5)
        boxTypeRx[0] = stoi(tempStr);
    else if (i == 6)
        boxTypeRx[1] = stoi(tempStr);
    }

    cout << "juncType: " << juncTypeRx << endl;
    cout << "pathState: " << pathStateRx[0] << pathStateRx[1] << pathStateRx[2] << pathStateRx[3] << endl;
    cout << "boxType: " << boxTypeRx[0] << boxTypeRx[1] << endl;


}

void emmit(int tx)
{
    string TxMessage = "";

    for (int i = 0; i < var.size(); i++)
    {
        for (int j = 0; j < var[i].size(); j++)
        {
            TxMessage = TxMessage + to_string(var[i][j]);
        }
    }
    
    cout << "txmsg: " << TxMessage << endl;
    emitter[tx]->send(TxMessage.c_str(), (int)strlen(TxMessage.c_str()) + 1);


}



