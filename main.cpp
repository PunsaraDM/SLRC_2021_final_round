#include "Robot.h"
using namespace std;


int main(int argc, char **argv)
{
    cout<< "simulation started"<<"\n";
    Robot robot(0,0,3);
    robot.travel_maze();

    return 0;
};
