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

#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4
#define INVERTWHITE 5

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
        junctions[0][i].found_junctions += 1;
        junctions[0][i].found_paths += 1;
    }
    for (int i = 0; i < ROWS; i++)
    {
        junctions[COLS - 1][i].set_path(RIGHT, NOPATH);
        junctions[COLS - 1][i].found_junctions += 1;
        junctions[COLS - 1][i].found_paths += 1;
    }
    for (int i = 0; i < COLS; i++)
    {
        junctions[i][0].set_path(DOWN, NOPATH);
        junctions[i][0].found_junctions += 1;
        junctions[i][0].found_paths += 1;
    }
    for (int i = 0; i < ROWS; i++)
    {
        junctions[i][ROWS - 1].set_path(UP, NOPATH);
        junctions[i][ROWS - 1].found_junctions += 1;
        junctions[i][ROWS - 1].found_paths += 1;
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

    if (junction_content_state == COLORED)
    {
        vector<int> junc{column, row};
        junc.reserve(junc.size() + distance(content.begin(), content.end()));
        junc.insert(junc.end(), content.begin(), content.end());
        colored_junctions.push_back(junc);
    }

    else if (junction_content_state == WHITE)
    {
        vector<int> junc{column, row};
        white_junctions.push_back(junc);
    }

    else if (junction_content_state == INVERTED && !has_white)
    {
        cout << "set to visited without white"
             << "\n";
        junc_state = VISITEDWITHOUTWHITE;
    }

    if (junction_content_state == INVERTED && has_white)
    {
        junctions[column][row].content_state = INVERTWHITE;
    }

    junctions[column][row].set_state(junc_state);
}

void Maze::update_path(int column, int row, vector<int> paths)
{
    junctions[column][row].set_paths(paths);
    if (column > 0)
    {
        junctions[column - 1][row].set_path(RIGHT, paths[LEFT]);
        if (!junctions[column][row].found_junctions_set)
        {
            junctions[column - 1][row].increment_found_junc();
        }
        check_inverted(column - 1, row);
    }
    if (column + 1 < COLS)
    {
        junctions[column + 1][row].set_path(LEFT, paths[RIGHT]);
        if (!junctions[column][row].found_junctions_set)
        {
            junctions[column + 1][row].increment_found_junc();
        }
        check_inverted(column + 1, row);
    }
    if (row > 0)
    {
        junctions[column][row - 1].set_path(UP, paths[DOWN]);
        if (!junctions[column][row].found_junctions_set)
        {
            junctions[column][row - 1].increment_found_junc();
        }
        check_inverted(column, row - 1);
    }
    if (row + 1 < ROWS)
    {
        junctions[column][row + 1].set_path(DOWN, paths[UP]);
        if (!junctions[column][row].found_junctions_set)
        {
            junctions[column][row + 1].increment_found_junc();
        }
        check_inverted(column, row + 1);
    }
    junctions[column][row].found_junctions_set = true;
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

void Maze::check_inverted(int col, int row)
{
    // cout << "checking: " << col << "," << row << ":" << junctions[col][row].found_paths << "\n";
    if (junctions[col][row].found_paths == 4 && junctions[col][row].get_state() == VISITEDWITHOUTWHITE)
    {
        cout << "resetting state back to discovered"
             << "\n";
        junctions[col][row].set_state(DISCOVERED);
    }
}