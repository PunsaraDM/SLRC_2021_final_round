#include "Navigator.h"
#define TIME_STEP 32

int main(int argc, char **argv)
{

    Navigator follower;


    while (follower.step(TIME_STEP) != -1)
    {
        follower.test();
        //break;

    };

    return 0;
};


