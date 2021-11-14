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
#define WHITE_PATCH 2
#define INVERTED 3
#define NORMAL 4
#define INVERTWHITE 5
#define PATCHNOBOX 6

#define LOWER 1
#define MIDDLE 2

#define DOUBLETOP 2
#define DOUBLEBOTTOM 1
#define TOP 3

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

void Maze::update_junction(int column, int row, vector<int> content, int junction_content_state, bool has_white, int robot)
{
    junctions[column][row].content_state = junction_content_state;
    if (junctions[column][row].get_state() != DISCOVERED)
    {
        visited += 1;
    }

    int junc_state = DISCOVERED;

    if (junction_content_state == COLORED && junctions[column][row].get_state() != DISCOVERED)
    {
        vector<int> junc{column, row};
        int count = 0;
        for (int i = 1; i > -1; i--)
        {
            cout << "colors: " << content[i] << "\n";
            vector<int> colored_context{column, row, content[i],i+1,false};
            if (RED <= content[i] && content[i] <= BLUE)
            {
                vector<int> vec{column, row};
                if (robot == LEFT)
                {
                    left_colors[content[i] - 1].push_back(vec);
                }
                else
                {
                    right_colors[content[i] - 1].push_back(vec);
                }
                count += 1;
                colored_sequential.push_back(colored_context);
            }
        }
        discovered += count;

        for (size_t i = 0; i < 2; i++)
        {
            int state = i + 1;
            if (count == 2)
            {
                if (i == 0)
                {
                    state = DOUBLEBOTTOM;
                }
                else
                {
                    state = DOUBLETOP;
                }
            }
            else
            {
                state = TOP;
            }
            vector<int> val{column, row, state, robot, true};
            cout << "inserting: " << column << ", " << row << ", " << state << content[i] << "\n";

            if (RED <= content[i] && content[i] <= BLUE)
            {
                if (content[i] == RED)
                {
                    colored_junctions[RED - 1].push_back(val);
                }
                else if (content[i] == GREEN)
                {
                    colored_junctions[GREEN - 1].push_back(val);
                }
                else
                {
                    colored_junctions[BLUE - 1].push_back(val);
                }
            }
        }
    }

    else if (junction_content_state == WHITE_PATCH && !has_white)
    {
        junctions[column][row].content_state = PATCHNOBOX;
        vector<int> junc{column, row};
        white_junctions.push_back(junc);
    }

    else if (junction_content_state == INVERTED && !has_white)
    {
        cout << "set to visited without WHITE_PATCH"
             << "\n";
        junc_state = VISITEDWITHOUTWHITE;
    }

    if (junction_content_state == INVERTED && has_white)
    {
        junctions[column][row].content_state = INVERTWHITE;
    }

    junctions[column][row].set_state(junc_state);
}

void Maze::update_path(int column, int row, vector<int> paths, int robot)
{
    bool juncs_found = junctions[column][row].found_junctions_set;
    junctions[column][row].set_paths(paths);
    for (size_t i = 0; i < 4; i++)
    {
        bool joined = junctions[column][row].set_found_by(i, robot, paths[i]);
        if (!paths_joined)
        {
            paths_joined = joined;
        }
    }

    if (column > 0)
    {

        update_path_helper(juncs_found, column - 1, row, robot, RIGHT, paths[LEFT]);
    }
    if (column + 1 < COLS)
    {
        update_path_helper(juncs_found, column + 1, row, robot, LEFT, paths[RIGHT]);
    }
    if (row > 0)
    {
        update_path_helper(juncs_found, column, row - 1, robot, UP, paths[DOWN]);
    }
    if (row + 1 < ROWS)
    {
        update_path_helper(juncs_found, column, row + 1, robot, DOWN, paths[UP]);
    }
    junctions[column][row].found_junctions_set = true;
}

void Maze::update_path_helper(bool juncs_found, int new_col, int new_row, int robot, int found_dir, int state)
{
    junctions[new_col][new_row].set_path(found_dir, state);
    bool joined = junctions[new_col][new_row].set_found_by(found_dir, robot, state);
    if (!paths_joined)
    {
        paths_joined = joined;
    }
    if (!juncs_found)
    {
        junctions[new_col][new_row].increment_found_junc();
    }
    check_inverted(new_col, new_row);
}

string Maze::print_content(vector<int> content)
{
    string text = "";

    for (size_t i = 0; i < content.size(); i++)
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

bool Maze::color_match()
{
    int count = 0;
    for (size_t i = 0; i < left_colors.size(); i++)
    {
        if (left_colors[i].size() > 0)
        {
            count += 1;
        }
    }
    for (size_t i = 0; i < right_colors.size(); i++)
    {
        if (right_colors[i].size() > 0)
        {
            count += 1;
        }
    }
    if (count == 6)
    {
        return true;
    }
    return false;
}
