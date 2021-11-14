#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include "SensorGroup.h"
#include "MotorGroup.h"
#include "PathFinder.h"
#include <vector>

using namespace webots;
using namespace std;

#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

#define EN_LEFT 0
#define EN_RIGH 1

//box type
#define RED 1
#define GREEN 2
#define BLUE 3
#define WHITE_CLR 4
#define NO_COLOR 5

//junction state
#define COLORED 1
#define WHITE_PATCH 2
#define INVERTED 3
#define NORMAL 4
#define INVERTWHITE 5
#define PATCHNOBOX 6        //in discovery state where white patches with no boxes

//path state
#define NOTACCESIBLE -2
#define NOPATH -1
#define UNDISCOVERED 0
#define DISCOVERED 1

//color sensors
#define CS_ARM 0
#define CS_LEFT 1
#define CS_RIGHT 2
#define CS_FRONT 3

#define NAVIGATE_STATE 0
#define DIRECTION 1
#define INV_PATCH 2
#define BOX_CARRY 0
#define INV_DIRECTION 1
#define JUNC_TYPE 3
#define BOX_GRAB 4
#define POSITION 0
#define COLOR 1

#define SEARCH 0
#define VISIT 1
#define PLACEMENT 2
#define FINISH_PLACEMENT 3
#define STALL 4

#define FALSE 0
#define TRUE 1

#define NEGLECT 0
#define LOWER 1
#define MIDDLE 2
#define UPPER 3

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

        void go_forward_specific_distance(double distance);
        void go_backward_specific_distance(double distance);
        void discover_junction();
        double getComDir();
        void turnAng(float angle);
        void turn(int dir);

        void arm_base_move(double target);
        void arm_vertical_move(double target);
        void arm_grab_box(double targetLeft, double targetRight);

        bool is_junction_detected();
        void visit_junction(int junctype);
        void discover_path(int side);
        void discover_white_patch();
        void visit_white_patch(bool initial = false);
        void discover_inv_junc(int boxDir);
        void visit_inv_junc();
        void resetVariables();
        void visit_normal_junc();
        void print_pathState();
        void stall_cell();

        void goto_placement_cell(bool final = false);
        void follow_line_until_junc_detect();
        void initial_phase();
        void one_cell();

        //new robot arm functions////
        void box_search_algo(bool haveBox = true);
        void place_white_box_before_search();
        void grab_white_box_after_search();
        int search_box_color(int level);
        void centre_box(double distance);
        void arm_parking();
        void grab_box(int color, int level);
        void place_white_box_in_red_square();
        void grab_white_box_from_red_square();
        void place_box(int color);
        void arm_carrying();
        // void final_stack_box(int color,int row); 
        void final_stack_box(int color); 

        void update_var(string RxMessage);
        string make_TxMessage();

{
    private:
        SensorGroup *sensorGroup;
        MotorGroup *motorGroup;
        PathFinder *pathFinder;

        int BLACK = 0;
        int WHITE = 1;

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

        vector<vector<int>> var {{0},{0},{0,0},{0},{0,0}};

        vector<int> boxType {0,0};
        vector<int> pathState {UNDISCOVERED,UNDISCOVERED,NOTACCESIBLE,UNDISCOVERED};   //down path is always keep as notaccesible since robot coming from that side
        int juncType = UNDISCOVERED;

        int detectedJunction = 0;

        int currentBoxColor = NO_COLOR;
        bool carriedGreenorBlue = false;

        float rightIRVal = 0;
        float leftIRVal = 0;

        float TURN90_EN_COUNT = 5.85;
        float TURN180_EN_COUNT = 11.4;

        int carryingBox = NO_COLOR;
        bool taskCompleted = false;
        int greenBoxCount = 0;
        int blueBoxCount = 0;

        float lineFollowPreviousError = 0.0;

        float WHEEL_RADIUS = 0.02;

        float servoPosition[6] = {0.8,-0.59,0.0,1.6,-0.1,0.1};

        int QTR_UP = 0;
        int QTR_DOWN = 1;

        int boxCountPlacement = 0;  //to solve the error in goto_placement_cell
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
        //double distArmBase_centre = 0.1;   //centre the box in white square
        double distArmBase_centre_red = 0.095;
        double distArmBase_centre_green = 0.105;
        double distArmBase_centre_blue = 0.115;
        double distArmBase_centre_white = 0.115;
        double distArmBase_min = 0.0;
        double distArmBase_place = 0.00;     //place white box before searching and centering a box
        double distArmBase_carry = 0.03; 
        double verticalGround = 0.0;
        double verticalMiddle = 0.06;   //functions start and ends by placing the gripper
        double verticalHighest = 0.12;
        //for inverted patches
        double distArmBase_inverted_place = 0.2;    //greater than max lenghth - distance to red box
    };
#ifdef __cplusplus
}
#endif

#endif

