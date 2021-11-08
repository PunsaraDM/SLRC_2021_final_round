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
        void passive_wait_wheel(double targetLeft, double targetRight);
        void passive_wait_servo(int servo, double target);
        void delay(int time);

        void follow_line(float Kp, float Kd, float minSpd, float baseSpd, float maxSpd);
        void follow_line_until_junc_detect_fast();
        //void follow_line_until_junc_detect_slow();
        //void follow_line_until_segment_detect();

        //void complete_turn(int dir, bool goForward = true);
        void go_forward_specific_distance(double distance);
        bool is_junction_detected();
        double getComDir();
        void turnAng(float angle);
        void turn_left();
        void turn_right();
        void turn_back();

        void arm_base_move(double target);
        void arm_vertical_move(double target);
        void arm_grab_box(double targetLeft, double targetRight);
        void detect_box_color_and_centre();
        void place_white_box_before_centre();
        void grab_white_box_after_centre();
        void place_white_box_in_red_square();
        void grab_white_box_from_red_square();

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

        int QTR_0 = 0;
        int QTR_1 = 1;
        int QTR_2 = 2;
        int QTR_3 = 3;
        int QTR_4 = 4;
        int QTR_5 = 5;
        int QTR_6 = 6;
        int QTR_7 = 7;
        int LINE_DETECT_LEFT = 8;
        int LINE_DETECT_RIGHT = 9;

        //junction state
        int COLORED = 1;
        int WHITE_PATCH = 2;
        int INVERTED = 3;
        int NORMAL = 4;
        //path state
        int NOTACCESIBLE = -2;
        int NOPATH = -1;
        int DISCOVERED = 1;
        //box type
        int RED = 1;
        int GREEN = 2;
        int BLUE = 3;
        int WHITE = 4;

        int detectedJunction = 0;

        int CS_ARM = 0;
        int CS_LEFT = 1;
        int CS_RIGHT = 2;
        int CS_FRONT = 3;

        float rightIRVal = 0;
        float leftIRVal = 0;

        float TURN90_EN_COUNT = 5.85;
        float TURN180_EN_COUNT = 11.4;



        // float WALL_MAX_VELOCITY = 7.0;
        // float WALL_MIN_VELOCITY = 2.0;
        // float WALL_FOLLOW_BASE_SPEED = 4.0;

        float lineFollowPreviousError = 0.0;

        float WHEEL_RADIUS = 0.02;

        // int POS_ARM_UP = 0;
        // int POS_ARM_DOWN = 1;
        // int POS_ARM_DEFAULT = 2;
        // int POS_BOX_UP = 3;
        // int POS_BOX_DOWN = 4;
        // int POS_BOX_DEFAULT = 5;
        float servoPosition[6] = {0.8,-0.59,0.0,1.6,-0.1,0.1};

        int QTR_UP = 0;
        int QTR_DOWN = 1;


        ////////////For robot arm/////////////// 
        double grabDist_min = 0.0;
        double grabDistRed = 0.0732;
        double grabDistGreen = 0.0832;
        double grabDistBlue = 0.0932;
        double grabDistToDetectColor = 0.07;
        double linearMotorVelocity = 0.2;
        double turnSpeed = 5.0;
        double expoValTurnThresh = 0.5;
        int turnAccu = 10000;
        const double DELTA_double = 0.0001;    //for grippers
        const double DELTA_single = 0.001;     //other linear motors
        double distArmBase_max = 0.19;
        double distArmBase_mid = 0.16;  //distance to grab a box
        double distArmBase_centre = 0.12;   //centre the box in white square
        double distArmBase_min = 0.0;
        double distArmBase_place = 0.01;     //place white box before searching and centering a box
        double distArmBase_carry = 0.05; 
        double verticalGround = 0.0;
        //double verticalLowest = ;
        double verticalMiddle = 0.06;   //functions start and ends by placing the gripper
        double verticalHighest = 0.12;
        //for inverted patches
        double distArmBase_inverted_place = 0.2;    //greater than max lenghth - distance to red box
    };
#ifdef __cplusplus
}
#endif

#endif

