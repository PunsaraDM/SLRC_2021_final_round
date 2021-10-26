#include "Generator.h"
#include <bits/stdc++.h>
#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;

#define COLS 9
#define ROWS 7

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

#define RED 1
#define GREEN 2
#define BLUE 3

#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4

//directions
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define TOTAL_PATH 94

using namespace std;

//Generator following
Generator::Generator()
{
    initialize();
    print_maze();
}

void Generator::initialize()
{
    set_red_paths();
    set_white_junctions();
    // set_inverted_junctions();
    set_color_box_junctions();
}

void Generator::set_red_paths()
{
    int red_path_count = rand() % (max_red_path_count - min_red_path_count + 1) + min_red_path_count;
    cout << "in red paths"
         << "\n";
    //setup red_paths
    vector<vector<int>> reds;
    for (int i = 0; i < red_path_count; i++)
    {
        bool found = false;
        while (!found)
        {
            int direction = rand() % 4;
            int col = rand() % COLS;
            int row = rand() % ROWS;
            bool hasNeighbour = false;
            int neighbour[3] = {col, row, direction};
            switch (direction)
            {
            case UP:
                if (row + 1 < ROWS)
                {
                    hasNeighbour = true;
                    neighbour[1] = row + 1;
                    neighbour[2] = DOWN;
                }
                break;
            case RIGHT:
                if (col + 1 < COLS)
                {
                    hasNeighbour = true;
                    neighbour[0] = col + 1;
                    neighbour[2] = LEFT;
                }
                break;
            case DOWN:
                if (0 < row)
                {
                    hasNeighbour = true;
                    neighbour[1] = row - 1;
                    neighbour[2] = UP;
                }
                break;
            case LEFT:
                if (0 < col)
                {
                    hasNeighbour = true;
                    neighbour[0] = col - 1;
                    neighbour[2] = RIGHT;
                }
                break;

            default:
                cout << "Invalid"
                     << "\n";
                break;
            }
            int path_state_current = maze.junctions[col][row].get_path(direction);
            int path_state_neighbour = UNDISCOVERED;
            if (hasNeighbour)
            {
                path_state_neighbour = maze.junctions[neighbour[0]][neighbour[1]].get_path(neighbour[2]);
            }
            if (path_state_current != NOPATH && path_state_neighbour != NOPATH)
            {
                vector<int> path{col, row, direction};
                reds.push_back(path);
                maze.junctions[col][row].set_path(direction, NOPATH);
                if (hasNeighbour)
                {
                    maze.junctions[neighbour[0]][neighbour[1]].set_path(neighbour[2], NOPATH);
                }

                found = true;
            }
        }
    }
    // cout << "no paths: " << reds.size() << "\n";
    // for (int i = 0; i < reds.size(); i++)
    // {

    //     cout << reds[i][0] << "," << reds[i][1] << "," << reds[i][2] << "\n";
    //     cout << maze.junctions[reds[i][0]][reds[i][1]].get_path(reds[i][2]) << "\n";
    // }
}

void Generator::set_white_junctions()
{
    cout << "in white junctions"
         << "\n";

    int white_junction_count = rand() % (max_white_junction - min_white_junction + 1) + min_white_junction;
    cout << "white_junction_count: " << white_junction_count << "\n";

    //setup white_junctions
    for (int i = 0; i < white_junction_count; i++)
    {
        bool found = false;
        while (!found)
        {
            int col = rand() % COLS;
            int row = rand() % ROWS;
            int junction_state = maze.junctions[col][row].content_state;
            // cout << "content_state: " << junction_state<< "\n";

            if (junction_state == NORMAL)
            {
                maze.junctions[col][row].content_state = WHITE;
                found = true;
            }
        }
    }
}
void Generator::set_inverted_junctions()
{
    cout << "in inverted junctions"
         << "\n";

    int inverted_junction_count = rand() % (max_inverted_junction - min_inverted_junction + 1) + min_inverted_junction;
    for (int i = 0; i < inverted_junction_count; i++)
    {
        bool found = false;
        while (!found)
        {
            int col = rand() % COLS;
            int row = rand() % ROWS;
            int junction_state = maze.junctions[col][row].content_state;
            if (junction_state == NORMAL)
            {
                maze.junctions[col][row].content_state = INVERTED;
                found = true;
            }
        }
    }
}
void Generator::set_color_box_junctions()
{
    cout << "in color box"
         << "\n";

    int box_junction_count = rand() % (max_box - min_box + 1) + min_box;
    vector<vector<int>> permutations{{2, 2, 2}, {1, 1, 2, 2}, {1, 1, 1, 1, 2}, {1, 1, 1, 1, 1, 1}};
    vector<int> color_order{RED, RED, GREEN, GREEN, BLUE, BLUE};
    random_shuffle(begin(color_order), end(color_order));

    int permutation_index = box_junction_count - 3;
    int color_index = 0;
    vector<int> order = permutations[permutation_index];

    for (int i = 0; i < order.size(); i++)
    {
        bool found = false;
        while (!found)
        {
            int col = rand() % COLS;
            int row = rand() % ROWS;
            int junction_state = maze.junctions[col][row].content_state;
            if (junction_state == NORMAL)
            {
                vector<int> content(color_order.begin() + color_index, color_order.begin() + color_index + order[i]);
                maze.junctions[col][row].content_state = COLORED;
                maze.junctions[col][row].set_content(content);
                color_index += order[i];
                found = true;
                cout << col << "," << row << "\n";
            }
        }
    }
}

void Generator::print_maze()
{
    cout << "in print maze"
         << "\n";
    int nopath = 0;
    ofstream myfile;
    myfile.open("maze.txt");
    for (int i = ROWS - 1; i >= 0; i--)
    {
        for (int k = 0; k < 2; k++)
        {
            for (int j = 0; j < COLS; j++)
            {
                vector<int> content = maze.junctions[j][i].get_content();

                if (k == 1 && (maze.junctions[j][i].get_path(DOWN) != NOPATH))
                {
                    myfile << "|"
                           << "\t\t";
                }
                else if (k == 1 && (maze.junctions[j][i].get_path(DOWN) == NOPATH) && i != 0)
                {
                    myfile << "X"
                           << "\t\t";
                }
                if (k == 0 && (maze.junctions[j][i].get_path(RIGHT) != NOPATH))
                {
                    if (content.size())
                    {
                        myfile << print_content(content) << " -----"
                               << "\t";
                    }
                    else
                    {
                        myfile << j << "," << i << " -----"
                               << "\t";
                    }
                }
                else if (k == 0 && (maze.junctions[j][i].get_path(RIGHT) == NOPATH))
                {
                    if (j != COLS - 1)
                    {
                        myfile << j << "," << i << " xxxxx"
                               << "\t";
                    }
                    else
                    {
                        if (content.size())
                        {
                            myfile << print_content(content) << " -----"
                                   << "\t";
                        }
                        else
                        {
                            myfile << j << "," << i << "\t";
                        }
                    }

                    // cout << j << "," << i << "," << RIGHT << "\n";
                }
            }

            myfile << "\n"
                   << "\n";
        }
    }

    cout << "generated test maze"
         << "\n";
    myfile.close();
}

string Generator::print_content(vector<int> content)
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