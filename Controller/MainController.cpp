#include "Navigator.h"
#define TIME_STEP 32

int main(int argc, char **argv)
{

    Navigator follower;


    while (follower.step(8) != -1)
    {
        follower.task();
        break;

    };

    return 0;
};


