#include "Maze.h"
#include <bits/stdc++.h>

#define COLS 9
#define ROWS 7

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define UNDISCOVERED 0
#define DISCOVERED 1

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

#define RED 1
#define GREEN 2
#define BLUE 3

using namespace std;

//maze following
Maze::Maze()
{
    initialize();
}

void Maze::initialize()
{
    for (int i = 0; i < ROWS; i++)
    {
        junctions[0][i].set_path(LEFT, NOPATH);
    }
    for (int i = 0; i < ROWS; i++)
    {
        junctions[COLS - 1][i].set_path(RIGHT, NOPATH);
    }
    for (int i = 0; i < COLS; i++)
    {
        junctions[i][0].set_path(DOWN, NOPATH);
    }
    for (int i = 0; i < ROWS; i++)
    {
        junctions[i][ROWS - 1].set_path(UP, NOPATH);
    }
}

void Maze::update_junction(int column, int row, vector<int> content)
{
    // for (int i = 0; i < content.size(); i++)
    // {
    //     cout << content[i] << "\n";
    // }
    
    if (junctions[column][row].get_state() != DISCOVERED)
    {
        discovered += content.size();
        cout << print_content(content) << "\n";
        cout << "discovered:" << discovered << "\n";

    }
    junctions[column][row].set_state(DISCOVERED);
    junctions[column][row].set_content(content);
}

void Maze::update_path(int column, int row, int paths[])
{

    junctions[column][row].set_paths(paths);
    if (column > 0)
    {
        junctions[column - 1][row].set_path(RIGHT, paths[LEFT]);
    }
    if (column + 1 < COLS)
    {
        junctions[column + 1][row].set_path(LEFT, paths[RIGHT]);
    }
    if (row > 0)
    {
        junctions[column][row - 1].set_path(UP, paths[DOWN]);
    }
    if (row + 1 < ROWS)
    {
        junctions[column][row + 1].set_path(DOWN, paths[UP]);
    }
}


string Maze::print_content(vector<int> content)
{
    string text = "";

    for (int i = 0; i < content.size(); i++)
    {
        switch (content[i])
        {
        case RED:
            text += "R";
            break;

        case BLUE:
            text += "B";
            break;

        case GREEN:
            text += "G";
            break;

        default:
            break;
        }
    }
    if (content.size() == 1)
    {
        text += " ";
    }
    return text;
}