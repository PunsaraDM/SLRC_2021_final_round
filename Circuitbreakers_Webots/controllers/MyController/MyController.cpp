#include "LineFollower.h"
#define TIME_STEP 32

int main(int argc, char **argv)
{

    LineFollower follower;


    while (follower.step(32) != -1)
    {
        follower.test_camera();
    };

    return 0;
};
