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

using namespace webots;

#ifndef MASTER_H
#define MASTER_H

#define UNDISCOVERED 0
#define DISCOVERED 1

#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

#define RED 1
#define GREEN 2
#define BLUE 3

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#ifdef __cplusplus
extern "C"
{
#endif
    class Master : public Robot
    {
    public:
        void initMaster();
        void initialize();
        void main_control();
        void receive(int rx);
        void emmit(int tx);
        Master();

    private:
        Maze maze;
        int rx = 0;
        PickStrategy pick_strategy;
        PathFinder *pathfinder_left;
        PathFinder *pathfinder_right;
        bool scan_just_over = true;

        static const int rx_count = 2;
        Receiver *receiver[rx_count];
        char rx_name[rx_count][10] = {"receiver1", "receiver2"};

        static const int tx_count = 2;
        Emitter *emitter[tx_count];
        char tx_name[tx_count][10] = {"emitter1", "emitter2"};

        vector<vector<int>> var{{0}, {0}, {0, 0}, {0}, {0, 0}};

        int juncTypeRx = 0;
        vector<int> pathStateRx{0, 0, 0, 0}; // down path is always keep as notaccesible since robot coming from that side
        vector<int> boxTypeRx{0, 0};
    };
#ifdef __cplusplus
}
#endif

#endif