#include <bits/stdc++.h>

using namespace std;

#ifndef JUNCTION_H
#define JUNCTION_H

#define UNDISCOVERED 0
#define DISCOVERED 1
#define SKIPPED 2

#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1
#define SKIPPATH 2

#define RED 1
#define GREEN 2
#define BLUE 3

#define COLORED 1
#define WHITE 2
#define INVERTED 3
#define NORMAL 4


#ifdef __cplusplus
extern "C"
{
#endif
    class Junction 
    {
    public:
        int content_state = NORMAL;
        int unavailable_paths=0;
        int inverted_dis[2] = {-1,1000};
        int white_dis[2] = {-1,10000};
        
        void set_paths(int paths[]);
        void set_path(int direction, int state);
        int get_path(int direction);
        int* get_paths();
        int get_state();
        void set_state(int state);
        void set_content(vector<int> content);
        void update_distances(int white[],int inverted[]);
        vector<int> get_content();

    private:
        int paths[4] = {0};
        int state = UNDISCOVERED;
        vector<int> content;
    };
#ifdef __cplusplus
}
#endif

#endif