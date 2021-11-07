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
        cout << "set to visited without white" << "\n";
        junc_state = VISITEDWITHOUTWHITE;
    }

    junctions[column][row].set_state(junc_state);
}

void Maze::update_path(int column, int row, int paths[])
{
    int temp_white_dis[2] = {-1, junctions[column][row].white_dis[1] + 1};
    int temp_inverted_dis[2] = {-1, junctions[column][row].inverted_dis[1] + 1};

    junctions[column][row].set_paths(paths);

    if (column > 0)
    {
        junctions[column - 1][row].set_path(RIGHT, paths[LEFT]);
        check_inverted(column - 1, row);
        temp_white_dis[0] = RIGHT;
        temp_inverted_dis[0] = RIGHT;
        junctions[column - 1][row].update_distances(temp_white_dis, temp_inverted_dis);
    }
    if (column + 1 < COLS)
    {
        temp_white_dis[0] = LEFT;
        temp_inverted_dis[0] = LEFT;
        junctions[column + 1][row].set_path(LEFT, paths[RIGHT]);
        check_inverted(column + 1, row);
        junctions[column + 1][row].update_distances(temp_white_dis, temp_inverted_dis);
    }
    if (row > 0)
    {
        temp_white_dis[0] = UP;
        temp_inverted_dis[0] = UP;
        junctions[column][row - 1].set_path(UP, paths[DOWN]);
        check_inverted(column, row - 1);
        junctions[column][row - 1].update_distances(temp_white_dis, temp_inverted_dis);
    }
    if (row + 1 < ROWS)
    {
        temp_white_dis[0] = DOWN;
        temp_inverted_dis[0] = DOWN;
        junctions[column][row + 1].set_path(DOWN, paths[UP]);
        check_inverted(column, row + 1);
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

void Maze::update_inverted_chain(int inverted_col, int inverted_row, int current_col, int current_row)
{
    vector<int> coord{current_col, current_row};
    for (int i = 0; i < junctions[inverted_col][inverted_row].inverted_chain_parent.size(); i++)
    {
        int col = junctions[inverted_col][inverted_row].inverted_chain_parent[i][0];
        int row = junctions[inverted_col][inverted_row].inverted_chain_parent[i][1];
        junctions[col][row].inverted_chain_children.push_back(coord);
    }
    junctions[current_col][current_row].inverted_chain_parent.reserve(junctions[current_col][current_row].inverted_chain_parent.size() + distance(junctions[inverted_col][inverted_row].inverted_chain_parent.begin(), junctions[inverted_col][inverted_row].inverted_chain_parent.end()));
    junctions[current_col][current_row].inverted_chain_parent.insert(junctions[current_col][current_row].inverted_chain_parent.end(), junctions[inverted_col][inverted_row].inverted_chain_parent.begin(), junctions[inverted_col][inverted_row].inverted_chain_parent.end());
}

void Maze::check_inverted(int col, int row)
{
    cout << "checking: " << col << "," << row << ":" << junctions[col][row].found_paths << "\n";
    if (junctions[col][row].found_paths == 4 && junctions[col][row].get_state() == VISITEDWITHOUTWHITE)
    {
        cout << "resetting state back to discovered" <<"\n";
        junctions[col][row].set_state(DISCOVERED);

        for (int i = 0; i < junctions[col][row].inverted_chain_children.size(); i++)
        {
            int col_in = junctions[col][row].inverted_chain_children[i][0];
            int row_in = junctions[col][row].inverted_chain_children[i][1];
            vector<int> coord{col, row};
            vector<vector<int>> new_inverted_parent;
            for (int j = 0; j < junctions[col_in][row_in].inverted_chain_parent.size(); j++)
            {
                int c_col = junctions[col_in][row_in].inverted_chain_parent[i][0];
                int c_row = junctions[col_in][row_in].inverted_chain_parent[i][1];
                if (c_col != col || c_row != row)
                {
                    vector<int> coord{c_col, c_row};
                    new_inverted_parent.push_back(coord);
                }
            }

            junctions[col_in][row_in].inverted_chain_parent = new_inverted_parent;
            junctions[col_in][row_in].set_state(DISCOVERED);
        }
    }
}