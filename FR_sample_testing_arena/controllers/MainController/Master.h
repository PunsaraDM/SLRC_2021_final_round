#include "Strategy.h"
#include "Maze.h"

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
    class Master
    {
    public:
        void initMaster(Master *master);
        void communicate();
        void GetReceiver(int rx);

        

    private:
        Master master;
        Maze maze;
        Strategy strategy;
        PathFinder* pathfinderleft = new PathFinder(0,0,LEFT);
        PathFinder* pathfinderright = new PathFinder(8,6,RIGHT);

        static const int rx_count = 2;
        Receiver *receiver[rx_count];
        char rx_name[rx_count][10] = {"receiver1","receiver2"};

        static const int tx_count = 2;
        Emitter *emitter[tx_count];
        char tx_name[tx_count][10] = {"emmiter1","emmiter2"};

        string RxMessage[2];

        string tempStr = "";
        vector<vector<vector<int>>> varRx{ {{0}, {0}, {0, 0}, {0}, {0, 0}}, {{0}, {0}, {0, 0}, {0}, {0, 0}} };

        int juncTypeRx[2] = {0,0};
        vector<vector<int>> pathStateRx{{0, 0, 0, 0}, {0, 0, 0, 0}}; // down path is always keep as notaccesible since robot coming from that side
        vector<vector<int>> boxTypeRx{{0, 0},{0, 0}};
    };
#ifdef __cplusplus
}
#endif

#endif