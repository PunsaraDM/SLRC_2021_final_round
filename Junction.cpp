#include "Junction.h"
#include <bits/stdc++.h>

//paths
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

#define COLS 9
#define ROWS 7


using namespace std;

void Junction::set_paths(int foundPaths[])
{
    for (int i = 0; i < 4; i++)
    {
        if(foundPaths[i] != NOTACCESIBLE){
            paths[i] = foundPaths[i];
        }
    }    
}

void Junction::set_path(int direction,int state)
{
    if(state != NOTACCESIBLE){
        paths[direction] = state;    
    }
}

void Junction::set_state(int received)
{
    state = received;   
}

int Junction::get_state()
{
    return state;   
}

int Junction::get_path(int direction){
    return paths[direction];
}

int* Junction::get_paths(){
    return paths;
}


void Junction::set_content(vector<int> recieved)
{
    content = recieved;    
}

vector<int> Junction::get_content()
{
    return content;    
}
