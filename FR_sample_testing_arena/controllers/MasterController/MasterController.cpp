#include "Master.h"
#define TIME_STEP 32

int main(int argc, char **argv)
{

    Master master;


    while (master.step(TIME_STEP) != -1)
    {
        master.main_control();
        break;

    };

    return 0;
};