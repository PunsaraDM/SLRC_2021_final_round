#include <bits/stdc++.h>

using namespace std;

#ifndef JUNCTION_H
#define JUNCTION_H

#define UNDISCOVERED 0
#define DISCOVERED 1

#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1
#define SKIPPATH 2

#define RED 1
#define GREEN 2
#define BLUE 3

#define COLORED 1
#define WHITE_PATCH 2
#define INVERTED 3
#define NORMAL 4


#define RESERVED 0
#define UNRESERVED 1

#ifdef __cplusplus
extern "C"
{
#endif
    class Junction
    {
    public:
        int content_state = NORMAL;
        int found_paths = 0;
        int found_junctions = 0;
        bool found_junctions_set = false;
        int travel_state = UNRESERVED;


        void set_paths(vector<int> paths);
        void set_path(int direction, int state);
        int get_path(int direction);
        vector<int> get_paths();
        int get_state();
        void set_state(int state);
        void set_content(vector<int> content);
        vector<int> get_content();
        void increment_found_junc();
        bool set_found_by(int dir, int robot);

    private:
        vector<int> paths{0, 0, 0, 0};
        vector<vector<int>> found_by{vector<int>(), vector<int>(), vector<int>(), vector<int>()};
        int state = UNDISCOVERED;
        vector<int> content;
    };
#ifdef __cplusplus
}
#endif

#endif