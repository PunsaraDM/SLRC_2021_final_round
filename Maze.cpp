#include "Maze.h"
#include <bits/stdc++.h>

#define COLS 9
#define ROWS 7


#define UNDISCOVERED 0
#define DISCOVERED 1

#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

using namespace std;


//maze following
Maze::Maze(){
    initialize();
}


void Maze::initialize()
{
    for (int i = 0; i < ROWS; i++)
    {
        int paths[4] = {0,0,0,-1};
        junctions[0][i].set_paths(paths);
    }
    for (int i = 0; i < ROWS; i++)
    {
        int paths[4] = {0,-1,0,0};
        junctions[COLS-1][i].set_paths(paths);
    }
    for (int i = 0; i < COLS; i++)
    {
        int paths[4] = {0,0,-1,0};
        junctions[i][0].set_paths(paths);
    }
    for (int i = 0; i < ROWS; i++)
    {
        int paths[4] = {-1,0,0,0};
        junctions[i][ROWS-1].set_paths(paths);
    }
    
}

void Maze::update_junction(int column, int row, int content[]){
    junctions[column][row].set_state(DISCOVERED);
    junctions[column][row].set_content(content);
}


void Maze::update_path(int column, int row,int paths[]){
    junctions[column][row].set_paths(paths);
}

