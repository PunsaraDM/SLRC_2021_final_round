#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Compass.hpp>
#include <math.h>
#include <bits/stdc++.h>
#include<iostream>
#include <cmath>

#define TIME_STEP 32
using namespace webots;
using namespace std;

#define turnAccu 1000

Robot *robot = new Robot();
int timeStep = (int)robot->getBasicTimeStep();

Motor *linear_motors[4];
char motor_names[4][20] = {"base_motor","vertical_motor","gripper_left_motor","gripper_right_motor"};

PositionSensor *linear_encoders[4];
char linear_encoder_names[4][24] = {"base_motor_pos","vertical_motor_pos","gripper_left_motor_pos","gripper_right_motor_pos"};

Motor *wheels[2];
char wheels_names[2][15] = {"motor_left", "motor_right"};

void init_motors(){
  for (int i=0; i < 4; i++){
    linear_motors[i] = robot->getMotor(motor_names[i]);
    linear_motors[i] -> setPosition(0.0);
  }

  for (int i=0; i < 4; i++){
    linear_encoders[i] = robot->getPositionSensor(linear_encoder_names[i]);
    linear_encoders[i]->enable(TIME_STEP);
  }
  
  for (int i = 0; i < 2; i++) {
  wheels[i] = robot->getMotor(wheels_names[i]);
  wheels[i]->setPosition(INFINITY);
  wheels[i]->setVelocity(0.0);
  }

}

Compass *compass;
void init_compass(){
  compass = robot->getCompass("compass");
  compass->enable(TIME_STEP);
}

//////////////////////////////////////Compas//////////////////

//Function to stop robot
void stop(){
  for (int i = 0; i < 2; i++){
    wheels[i]->setVelocity(0);
  }
}

//Function to get the initial postion by number of 90's from north
double getComDir(){
  const double *cVals = compass->getValues();
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
void turnAng(float a){
  double initNinetyCount;
  initNinetyCount = getComDir();  //# of 90's in initial position 

  double targetNinetyCount = round(turnAccu*remainder((initNinetyCount + a/90.0),4.0))/turnAccu;  //# of 90's for the target
  targetNinetyCount = remainder(targetNinetyCount, 4.0);

  int initTurnSide = 1;
  if(a < 0.0){
    initTurnSide = -1;
  }
      
  float expoVal = 1.0;
  float turnError = 0.0;
  //float initTurnError = turnError;
  int turnSide;

  while((robot->step(timeStep) != -1) && (initNinetyCount != targetNinetyCount)){
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
      expoVal *= 0.5;
      initTurnSide = turnSide;
    }
    
    //set default for the right turn
    double leftSpeed = 5.0*turnSide*expoVal;
    double rightSpeed = -5.0*turnSide*expoVal;
    //cout<<leftSpeed<<"   "<<rightSpeed<<endl;
    wheels[0]->setVelocity(leftSpeed);
    wheels[1]->setVelocity(rightSpeed);

  }
  stop();
}

//Function to move robot forward
void go_forward(){
  for (int i = 0; i < 2; i++){
    wheels[i]->setVelocity(5.0);
  }
}
    
//Function to turn robot 90 degrees left       
void turn_left(){
  turnAng(-90.0);
}
    
//Function to turn robot 90 degrees right    
void turn_right(){
  cout<<"turn right"<<endl;
  turnAng(90.0);
}
    
//Function to turn robot 90 degrees right    
void turn_back(){
  turnAng(180.0);
}
    



////////////////////////////Robot Arm////////////////////////////////////
void passive_wait(int motor_index, double target){
  const double DELTA = 0.001;
  double dif;
  double effective;
  do
  {
    if (robot->step(timeStep) == -1){
      exit(EXIT_SUCCESS);
    }
    effective = linear_encoders[motor_index]->getValue();
    //cout<<effective<<endl;
    dif = fabs(effective-target);             //check here again
    //cout<<"____"<<dif<<endl;
  } while (dif > DELTA);
}

void passive_thread_wait(double targetLeft, double targetRight)
{
    const double DELTA = 0.0001;
    double dif;
    double effectiveLeft, effectiveRight;
    do
    {
      if (robot->step(timeStep) == -1){
        exit(EXIT_SUCCESS);
      }
      effectiveLeft = linear_encoders[2]->getValue();
      effectiveRight = linear_encoders[3]->getValue();
      dif = min(fabs(effectiveLeft - targetLeft), fabs(effectiveRight - targetRight));
      //cout<<dif<<endl;
    } while (dif > DELTA);
}

void arm_base_move(double target){
  linear_motors[0] -> setPosition(target);
  linear_motors[0] -> setVelocity(0.05);
  passive_wait(0,target);
}

void arm_vertical_move(double target){
  linear_motors[1] -> setPosition(target);
  linear_motors[1] -> setVelocity(0.5);
  passive_wait(1,target);
}

void arm_grab_box(double targetLeft, double targetRight){
  linear_motors[2] -> setPosition(targetLeft);
  linear_motors[2] -> setVelocity(0.5);
  linear_motors[3] -> setPosition(targetRight);
  linear_motors[3] -> setVelocity(0.5);
  passive_thread_wait(targetLeft, targetRight);
}


int main(int argc, char **argv) {
  int flag = 1;
  init_motors();
  init_compass();
  while (robot->step(timeStep) != -1) {
    if (flag==1){
      turn_right();
      arm_base_move(0.04);
      arm_grab_box(0.06,0.06);
      arm_vertical_move(0.05);
      flag = 0;
      turn_right();
    }
    
    //arm_base_move(-0.07);
    //arm_vertical_move(0.03);
    
    for (int i = 0; i < 2; i++) {
      wheels[i] = robot->getMotor(wheels_names[i]);
      wheels[i]->setVelocity(5.0);
    } 
    
    //arm_vertical_move(0);
    //arm_base_move(0);
    //arm_grab_box(0.05,0.05);
    //arm_base_move(0.05);
  };

  delete robot;
  return 0;
}
