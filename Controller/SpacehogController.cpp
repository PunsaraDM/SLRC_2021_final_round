#include "LineFollower.h"
#define TIME_STEP 16

int main(int argc, char **argv)
{

    LineFollower follower;


    while (follower.step(8) != -1)
    {
        follower.task();
        break;

    };

    return 0;
};


