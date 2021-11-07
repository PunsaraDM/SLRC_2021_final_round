#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include "Navigator.h"
#include "SensorGroup.h"
#include "MotorGroup.h"
#include <math.h>
#include <cmath>
#include <bits/stdc++.h>

#define TIME_STEP 32
using namespace webots;
using namespace std;

Navigator::Navigator()
{
    sensorGroup = new SensorGroup();
    motorGroup = new MotorGroup();
    sensorGroup->initialize(this);
    motorGroup->initialize(this);
}

////////////////////////////////////////////////////////// delay /////////////////////////////////////////////////////////////////////////

void Navigator::passive_wait(double targetLeft, double targetRight)
{
    double dif;
    double effectiveLeft, effectiveRight;
    do
    {
        if (step(TIME_STEP) == -1)
            exit(EXIT_SUCCESS);

        effectiveLeft = sensorGroup->get_encoder_val(GRIP_EN_LEFT);
        effectiveRight = sensorGroup->get_encoder_val(GRIP_EN_RIGHT);
        dif = min(fabs(effectiveLeft - targetLeft), fabs(effectiveRight - targetRight));

    } while (dif > DELTA_double);
}

//TODO: give additional parameters to above function to specify encoders

void Navigator::passive_wait_servo(int servo, double target)
{
    double dif;
    double effective;
    do
    {
        if (step(TIME_STEP) == -1)
            exit(EXIT_SUCCESS);

        effective = sensorGroup->get_encoder_val(servo);
        dif = fabs(effective - target);             //check here again
    } while (dif > DELTA_single);
}

void Navigator::delay(int time)
{
    int target = time / TIME_STEP;
    int count = 0;
    while (step(TIME_STEP) != -1){
        count += 1;
        if (count > target)
            break;

    }
}

////////////////////////////////////////////////////////// follow line methods /////////////////////////////////////////////////////////////////////////

void Navigator::follow_line(float Kp, float Kd, float minSpd, float baseSpd, float maxSpd)
{
    int position = sensorGroup->qtr_read_line();
    int error = position - 3500;

    double controlValue = (error * Kp) + (error - wallFollowPreviousError) * Kd;

    lineFollowPreviousError = error;

    rightSpeed = baseSpd - controlValue;
    leftSpeed = baseSpd + controlValue;

    if (rightSpeed < minSpd)
        rightSpeed = minSpd;
    else if (rightSpeed > maxSpd)
        rightSpeed = maxSpd;

    if (leftSpeed < minSpd)
        leftSpeed = minSpd;
    else if (leftSpeed > maxSpd)
        leftSpeed = maxSpd;
    cout<<leftSpeed<<"  "<<rightSpeed<<endl;
    motorGroup->set_velocity(leftSpeed, rightSpeed);
}

void Navigator::follow_line_until_junc_detect_fast()
{
    int count = DEACCELERATE_COUNT;
    while (step(TIME_STEP) != -1)
    {
        if(sensorGroup->is_junction_detected() == true)
            break;
        if (count>-1)
        {
            baseSpeed = CONST_BASE_SPEED - count;
            count-=0.5;
        }
        follow_line(0.001,0.0,7.0,baseSpeed,17.0);
    }

}

// void Navigator::follow_line_until_junc_detect_slow()
// {
//     while (step(TIME_STEP) != -1)
//     {
//         if(sensorGroup->is_junction_detected() == true)
//             break;
//         follow_line(0.01,0.0,2.5,5.0,7.5);
//     }

// }

// void Navigator::follow_line_until_segment_detect()
// {
//     while (step(TIME_STEP) != -1)
//     {
//         if(sensorGroup->is_line_segment_detected() == true)
//             break;
//         follow_line(0.01,0.0,2.5,5.0,7.5);
//     }

// }

////////////////////////////////////////////////////////// turns /////////////////////////////////////////////////////////////

// void Navigator::complete_turn(int dir, bool goForward)
// {
//     if (goForward == true)
//     {
//         go_forward_specific_distance(5.8);
//     }

//     sensorGroup->stabilize_encoder(this);

//     int sign = 1;
//     if (dir == LEFT)
//         sign = -1;

//     double initialLeftENcount = sensorGroup->get_encoder_val(LEFT);
//     double initialRightENcount = sensorGroup->get_encoder_val(RIGHT);

//     double leftCount = initialLeftENcount + (sign * TURN90_EN_COUNT);
//     double rightCount = initialRightENcount - (sign * TURN90_EN_COUNT);

//     if (dir == BACK)
//     {
//         leftCount = initialLeftENcount + TURN180_EN_COUNT;
//         rightCount = initialRightENcount - TURN180_EN_COUNT;
//     }

//     motorGroup->set_control_pid(4.5, 0, 0);
//     motorGroup->set_velocity(10, 10);

//     motorGroup->set_position(leftCount, rightCount);

//     passive_wait(leftCount, rightCount);

//     motorGroup->enable_motor_velocity_control();
// }

// void Navigator::go_forward_specific_distance(double distance)
// {

//     sensorGroup->stabilize_encoder(this);

//     double initialLeftENcount = sensorGroup->get_encoder_val(LEFT);
//     double initialRightENcount = sensorGroup->get_encoder_val(RIGHT);

//     motorGroup->set_control_pid(8, 0, 0);
//     motorGroup->set_velocity(7.5, 7.5);

//     motorGroup->set_position(distance + initialLeftENcount, distance + initialRightENcount);
//     passive_wait(distance + initialLeftENcount, distance + initialRightENcount);
//     motorGroup->enable_motor_velocity_control();
// }

//Function to get the initial postion by number of 90's from north
double Navigator::getComDir(){
  const double *cVals = sensorGroup->get_compass_value();
  double rad = atan2(cVals[0], cVals[2]);
  double bearing = (rad - 1.5708)/M_PI * 180.0;
  if(bearing < 0.0){
    bearing += 360.0;
  }

  bearing = remainder(bearing, 360.0); 
  bearing = round(bearing*turnAccu)/turnAccu;
  double ninetyCount = round(turnAccu*bearing / 90.0)/turnAccu;

  return remainder(ninetyCount, 4.0);
}

//Function to turn robot to a certain angle
void Navigator::turnAng(float angle){
  double initNinetyCount;
  initNinetyCount = getComDir();  //# of 90's in initial position 

  double targetNinetyCount = round(turnAccu*remainder((initNinetyCount + angle/90.0),4.0))/turnAccu;  //# of 90's for the target
  targetNinetyCount = remainder(targetNinetyCount, 4.0);

  int initTurnSide = 1;
  if(angle < 0.0){
    initTurnSide = -1;
  }
      
  float expoVal = 1.0;
  float turnError = 0.0;
  int turnSide;

  while((step(TIME_STEP) != -1) && (initNinetyCount != targetNinetyCount)){
    initNinetyCount = getComDir();
    //cout<<turnError<<endl;
    if(targetNinetyCount < initNinetyCount){
      turnError = targetNinetyCount + 4 - initNinetyCount;
    }
    else{
      turnError = targetNinetyCount - initNinetyCount;
    }
        
    
    if(turnError <= 2){
      turnSide=1;
    }
    else{
      turnSide=-1;
    }
    
    if(initTurnSide != turnSide){
      expoVal *= expoValTurnThresh;
      initTurnSide = turnSide;
    }
    
    //set default for the right turn
    double leftSpeed = turnSpeed*turnSide*expoVal;
    double rightSpeed = -turnSpeed*turnSide*expoVal;
    //cout<<leftSpeed<<"   "<<rightSpeed<<endl;
    motorGroup->set_velocity(leftSpeed,rightSpeed);
  }
  motorGroup->robot_stop();
}

void Navigator::turn_left(){
  turnAng(-90.0);
}
    
//Function to turn robot 90 degrees right    
void Navigator::turn_right(){
  cout<<"turn right"<<endl;
  turnAng(90.0);
}
    
//Function to turn robot 90 degrees right    
void Navigator::turn_back(){
  turnAng(180.0);
}

////////////////////////////////////////////////////////////////////ARM////////////////////////////////////////////////////

void Navigator::arm_base_move(double target)
{
  motorGroup->set_linear_target(2,target,linearMotorVelocity);
  passive_wait_servo(2,target);
}

void Navigator::arm_vertical_move(double target)
{
  motorGroup->set_linear_target(3,target,linearMotorVelocity);
  passive_wait_servo(3,target);
}

void Navigator::arm_grab_box(double targetLeft, double targetRight)
{
  motorGroup->set_linear_target(4,targetLeft,linearMotorVelocity);
  motorGroup->set_linear_target(5,targetRight,linearMotorVelocity);
  passive_wait(targetLeft, targetRight);
}

void Navigator::detect_box_color_and_grab()
{
  double distToDetectLocal = grabDistToDetectColor;
  arm_grab_box(distToDetectLocal,distToDetectLocal);
  bool isDetected = true;
  while (isDetected)
  {
    int colorBox = sensorGroup->get_colour(0);
    if (colorBox == 1){ //Red box
      cout<<"RED"<<endl;
      arm_grab_box(grabDistRed,grabDistRed);
      break;
    }
    else if (colorBox == 2){ //Green box
      cout<<"GREEN"<<endl;
      arm_grab_box(grabDistGreen,grabDistGreen);
      break;
    }
    else if (colorBox == 3){ //Blue box
      cout<<"blue"<<endl;
      arm_grab_box(grabDistBlue,grabDistBlue);
      break;
    }
    else{ //Not detected any color
      cout<<"Not detect any color"<<endl;
      distToDetectLocal += 0.01;
      arm_grab_box(distToDetectLocal,distToDetectLocal);
    }
  }
}

void Navigator::task()
{
  int flag = 1;
  cout<<"in"<<endl;
  while (step(TIME_STEP) != -1) {
    // if (flag==1){
    //   //turn_right();
    //   arm_base_move(distArmBase_max);
    //   arm_grab_box(grabDistGreen,grabDistGreen);
    //   arm_vertical_move(verticalHighest);
    //   flag = 0;
    //   //turn_right();
    // }
    // follow_line(0.0007,0.002,5.5,6.5,7.5);
    // motorGroup->qtr_servo(QTR_DOWN,2.0);
    //arm_base_move(-0.07);
    //arm_vertical_move(0.03);
    //int clr = sensorGroup->get_colour(0);
    cout<<sensorGroup->get_colour(CS_ARM)<<"  "<<sensorGroup->get_colour(CS_LEFT)<<"  "<<sensorGroup->get_colour(CS_RIGHT)<<"  "<<sensorGroup->get_colour(CS_FRONT)<<endl;
    //motorGroup->set_velocity(5.0,5.0);

    //cout<<sensorGroup->get_distance_value(0)<<endl;

    //arm_vertical_move(0);
    //arm_base_move(0);
    //arm_grab_box(0.05,0.05);
    //arm_base_move(0.05);
  };

}

void Navigator::test()
{
    follow_line(0.0007,0.002,5.5,6.5,7.5);
    //cout<<sensorGroup->get_ir_value(0)<<endl;
}
