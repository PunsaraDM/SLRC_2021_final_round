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

void Master::communicate()
{
    for{int i==0, i<2, i++}
    {
        GetReceiver(i);
        if (i) 
        {varRx[i] = pathfinderright->travel_maze(juncTypeRx[i], pathStateRx[i], boxTypeRx[i]); 
    }



}

void Master::GetReceiver(int rx)
{

    if (receiver[rx]->getQueueLength() > 0)
    {
        string RxMessage[rx]((const char *)receiver[rx]->getData());

        cout <<"received msg "<< RxMessage[rx] << endl;
        receiver[rx]->nextPacket();
        
        // for (int i = 0; i < 7; i++)
        // {
        //   tempStr = TxMessage[i];
        //   if (i == 0)
        //     varRx[0][0] = stoi(tempStr);
        //   else if (i == 1)
        //     varRx[1][0] = stoi(tempStr);
        //   else if (i == 2)
        //     varRx[2][0] = stoi(tempStr);
        //   else if (i == 3)
        //     varRx[2][1] = stoi(tempStr);
        //   else if (i == 4)
        //     varRx[3][0] = stoi(tempStr);
        //   else if (i == 5)
        //     varRx[4][0] = stoi(tempStr);
        //   else if (i == 6)
        //     varRx[4][1] = stoi(tempStr);
        // }

        //cout << "rxmsg: " << varRx[0][0] << varRx[1][0] << varRx[2][0] << varRx[2][1] << varRx[3][0] << varRx[4][0] << varRx[4][1] << endl;

        for (int i = 0; i < 7; i++)
        {
        tempStr = RxMessage[rx][i];
        if (i == 0)
            juncTypeRx[rx] = stoi(tempStr);
        else if (i == 1)
            pathStateRx[rx][0] = stoi(tempStr);
        else if (i == 2)
            pathStateRx[rx][1] = stoi(tempStr);
        else if (i == 3)
            pathStateRx[rx][2] = stoi(tempStr);
        else if (i == 4)
            pathStateRx[rx][3] = stoi(tempStr);
        else if (i == 5)
            boxTypeRx[rx][0] = stoi(tempStr);
        else if (i == 6)
            boxTypeRx[rx][1] = stoi(tempStr);
        }

        cout << "juncType: " << juncTypeRx[rx] << endl;
        cout << "pathState: " << pathStateRx[rx][0] << pathStateRx[rx][1] << pathStateRx[rx][2] << pathStateRx[rx][3] << endl;
        cout << "boxType: " << boxTypeRx[rx][0] << boxTypeRx[rx][1] << endl;

    }
}




