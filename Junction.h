#ifndef JUNCTION_H
#define JUNCTION_H

#define UNDISCOVERED 0
#define DISCOVERED 1

#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

#define INVERTED 1
#define WHITE 2
#define RED 3
#define GREEN 4
#define BLUE 5



#ifdef __cplusplus
extern "C"
{
#endif
    class Junction 
    {
    public:
        void set_paths(int paths[]);
        void set_path(int direction, int state);
        int get_path(int direction);
        int get_state();
        void set_state(int state);
        void set_content(int content[]);
        int* get_content();

    private:
        int paths[4] = {-1};
        int state = UNDISCOVERED;
        int content[3] = {-1};
    };
#ifdef __cplusplus
}
#endif

#endif