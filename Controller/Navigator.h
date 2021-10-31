#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include "SensorGroup.h"
#include "MotorGroup.h"

using namespace webots;

#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#ifdef __cplusplus
extern "C"
{
#endif

    class Navigator : public Robot
    {
    public:
        Navigator();

        void task();
        void test();

        void passive_wait(double targetLeft, double targetRight);
        void passive_wait_servo(int servo, double target);
        void delay(int time);

        void follow_line(float Kp, float Kd, float minSpd, float baseSpd, float maxSpd);
        void follow_line_until_junc_detect_fast();
        //void follow_line_until_junc_detect_slow();
        //void follow_line_until_segment_detect();

        //void complete_turn(int dir, bool goForward = true);
        //void go_forward_specific_distance(double distance);
        double getComDir();
        void turnAng(float angle);
        void turn_left();
        void turn_right();
        void turn_back();

        void arm_base_move(double target);
        void arm_vertical_move(double target);
        void arm_grab_box(double targetLeft, double targetRight);

    private:
        SensorGroup *sensorGroup;
        MotorGroup *motorGroup;

        int BLACK = 0;
        int WHITE = 1;

        int LEFT = 0;
        int RIGHT = 1;
        int BACK = 2;
        int FRONT = 4;
        int ARM = 2;    //index of arm servo
        int BOX = 3;    //index of box servo

        int GRIP_EN_LEFT = 4;
        int GRIP_EN_RIGHT = 5;

        float leftSpeed;
        float rightSpeed;

        float MAX_VELOCITY = 20.0;
        float MIN_VELOCITY = 0.0;
        float CONST_BASE_SPEED = 12.0;
        float baseSpeed = CONST_BASE_SPEED;
        float DEACCELERATE_COUNT = 10;

         
        // int DS_SENSOR_BOX = 3;
        // int DS_SENSOR_FRONT = 2;
        // int DS_SENSOR_RIGHT = 1;
        // int DS_SENSOR_LEFT = 0;
        // int TOF_RIGHT = 5;
        // int TOF_LEFT = 4;

        // int CAM_ARM = 0;
        // int CAM_BACK = 1;
        // int CAM_FRONT = 2;

        float rightIRVal = 0;
        float leftIRVal = 0;

        float TURN90_EN_COUNT = 5.85;
        float TURN180_EN_COUNT = 11.4;

        int turnAccu 1000;


        // float WALL_MAX_VELOCITY = 7.0;
        // float WALL_MIN_VELOCITY = 2.0;
        // float WALL_FOLLOW_BASE_SPEED = 4.0;


        float wallFollowPreviousError = 0.0;
        float lineFollowPreviousError = 0.0;

        // int POS_ARM_UP = 0;
        // int POS_ARM_DOWN = 1;
        // int POS_ARM_DEFAULT = 2;
        // int POS_BOX_UP = 3;
        // int POS_BOX_DOWN = 4;
        // int POS_BOX_DEFAULT = 5;
        float servoPosition[6] = {0.8,-0.59,0.0,1.6,-0.1,0.1};

    };
#ifdef __cplusplus
}
#endif

#endif

