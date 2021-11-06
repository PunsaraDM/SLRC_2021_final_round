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

#define VISITEDWITHOUTWHITE 2


using namespace std;

//maze following
Maze::Maze()
{
    initialize();
}

void Maze::initialize()
{
    junctions[0][0].inverted_dis[1] = 0;
    junctions[0][0].white_dis[1] = 0;

    for (int i = 0; i < ROWS; i++)
    {
        junctions[0][i].set_path(LEFT, NOPATH);
        junctions[0][i].unavailable_paths += 1;
    }
    for (int i = 0; i < ROWS; i++)
    {
        junctions[COLS - 1][i].set_path(RIGHT, NOPATH);
        junctions[0][i].unavailable_paths += 1;
    }
    for (int i = 0; i < COLS; i++)
    {
        junctions[i][0].set_path(DOWN, NOPATH);
        junctions[0][i].unavailable_paths += 1;
    }
    for (int i = 0; i < ROWS; i++)
    {
        junctions[i][ROWS - 1].set_path(UP, NOPATH);
        junctions[0][i].unavailable_paths += 1;
    }
}

void Maze::update_junction(int column, int row, vector<int> content, int junction_content_state, bool has_white)
{
    junctions[column][row].content_state = junction_content_state;
    if (junctions[column][row].get_state() != DISCOVERED)
    {
        visited += 1;
        discovered += content.size();
        cout << print_content(content) << "\n";
    }

    int junc_state = DISCOVERED;
    
    if(junction_content_state == COLORED){
        vector<int> junc {column,row};
        junc.reserve(junc.size() + distance(content.begin(),content.end()));
        junc.insert(junc.end(),content.begin(),content.end());
        colored_junctions.push_back(junc);
    }
    
    else if(junction_content_state == WHITE){
        vector<int> junc {column,row};
        white_junctions.push_back(junc);
    }
    
    else if(junction_content_state == INVERTED && !has_white){
        junc_state = VISITEDWITHOUTWHITE;
    }

    junctions[column][row].set_state(junc_state);
    
}

void Maze::update_path(int column, int row, int paths[])
{
    int temp_white_dis[2] = {-1, junctions[column][row].white_dis[1] + 1};
    int temp_inverted_dis[2] = {-1, junctions[column][row].inverted_dis[1] + 1};
    if(junctions[column][row].content_state == INVERTED){
        
    }
    junctions[column][row].set_paths(paths);

    if (column > 0)
    {
        junctions[column - 1][row].set_path(RIGHT, paths[LEFT]);
        temp_white_dis[0] = RIGHT;
        temp_inverted_dis[0] = RIGHT;
        junctions[column - 1][row].update_distances(temp_white_dis, temp_inverted_dis);
    }
    if (column + 1 < COLS)
    {
        temp_white_dis[0] = LEFT;
        temp_inverted_dis[0] = LEFT;
        junctions[column + 1][row].set_path(LEFT, paths[RIGHT]);
        junctions[column + 1][row].update_distances(temp_white_dis, temp_inverted_dis);

    }
    if (row > 0)
    {
        temp_white_dis[0] = UP;
        temp_inverted_dis[0] = UP;
        junctions[column][row - 1].set_path(UP, paths[DOWN]);
        junctions[column][row - 1].update_distances(temp_white_dis, temp_inverted_dis);

    }
    if (row + 1 < ROWS)
    {
        temp_white_dis[0] = DOWN;
        temp_inverted_dis[0] = DOWN;
        junctions[column][row + 1].set_path(DOWN, paths[UP]);
        junctions[column][row + 1].update_distances(temp_white_dis, temp_inverted_dis);
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