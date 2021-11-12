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
        Master(int startCol, int startRow);
        vector<vector<int>> travel_maze(int juncType, vector<int> path_state, vector<int> box_type);
        vector<vector<int>> travel_with_color();
        vector<vector<int>> search_maze(int juncType, vector<int> path_state, vector<int> box_type);
        vector<int> find_junction_content(vector<int> box_type);
        int find_junction_content_state();
        void update_robot_position(int direction);
        vector<vector<int>> create_next_data_packet();
        int get_local_direction();
        int get_invert_box_dir();
        vector<int> adjust_path_state_to_global(vector<int> paths);
        void get_next_junc_color();

    private:
        Maze maze;
        Strategy strategy;
        PickStrategy pick_strategy;
        int travel_dir = LEFT;
        int direction_to_travel = UP;

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