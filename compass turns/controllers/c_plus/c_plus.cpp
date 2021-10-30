#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Compass.hpp>
#include <math.h>
#include <bits/stdc++.h>
#include<iostream>
#include <cmath>

#define TIME_STEP 32
using namespace webots;
using namespace std;

Robot *robot = new Robot();
int timeStep = (int)robot->getBasicTimeStep();

Motor *wheels[4];
char wheels_names[4][8] = {"wheel1", "wheel2", "wheel3", "wheel4"};
void init_motors(){
  for (int i = 0; i < 4; i++)
  {
    wheels[i] = robot->getMotor(wheels_names[i]);
    wheels[i]->setPosition(INFINITY);
    wheels[i]->setVelocity(0.0);
  }      
}

DistanceSensor *ds[3];
char dsNames[3][10] = {"ds_left", "ds_right", "ds_mid"};
void init_distancesensors(){
  for (int i = 0; i < 3; i++)
    {
        ds[i] = robot->getDistanceSensor(dsNames[i]);
        ds[i]->enable(TIME_STEP);
    }
}

Compass *compass;
void init_compass(){
  compass = robot->getCompass("compass");
  compass->enable(TIME_STEP);
}

/////////////////// Function to get the initial postion by number of 90's from north
double getComDir(){
  const double *cVals = compass->getValues();
  double rad = atan2(cVals[0], cVals[2]);
  double bearing = (rad - 1.5708)/M_PI * 180.0;
  if(bearing < 0.0){
    bearing += 360.0;
  }

  bearing = remainder(bearing, 360.0); 
  bearing = round(bearing*1000.0)/1000.0;
  double ninetyCount = round(1000.0*bearing / 90.0)/1000.0;

  return remainder(ninetyCount, 4.0);
}

//////////////////////// Function to turn robot to a certain angle
void turnAng(float a){
  double initNinetyCount;
  initNinetyCount = getComDir();  //# of 90's in initial position 

  double targetNinetyCount = round(1000.0*remainder((initNinetyCount + a/90.0),4.0))/1000.0;  //# of 90's for the target
  targetNinetyCount = remainder(targetNinetyCount, 4.0);

  int initTurnSide = 1;
  if(a < 0.0){
    initTurnSide = -1;
  }
      
  float expoVal = 1.0;
  float turnError = 0.0;
  float initTurnError = turnError;
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
    /*
    if (initTurnError!=4){
      if(3.9<turnError){
        expoVal *= 0.5;
        initTurnSide = turnSide;
        if (turnError>0.001){
          return;
        }
      }
    }
    else if (initTurnError!=0){
      if(turnError<0.01){
        expoVal *= 0.5;
        initTurnSide = turnSide;
        if (3.99<turnError){
          return;
        }
      }
    }*/
    
    if(initTurnSide != turnSide){
      expoVal *= 0.5;
      initTurnSide = turnSide;
    }
    
    //set default for the right turn
    double leftSpeed = 3.0*turnSide*expoVal;
    double rightSpeed = -3.0*turnSide*expoVal;
    wheels[0]->setVelocity(leftSpeed);
    wheels[1]->setVelocity(rightSpeed);
    wheels[2]->setVelocity(leftSpeed);
    wheels[3]->setVelocity(rightSpeed);

  }
}

/////////////////////////////Function to move robot forward
void go_forward(){
  for (int i = 0; i < 4; i++){
    wheels[i]->setVelocity(3.25);
  }
}
    
///////////////////////////Function to turn robot 90 degrees left       
void turn_left(){
  turnAng(-90.0);
}
    
///////////////////////////Function to turn robot 90 degrees right    
void turn_right(){
  turnAng(90.0);
}
    
///////////////////////////Function to turn robot 90 degrees right    
void turn_back(){
  turnAng(180.0);
}
    
///////////////////////////Function to stop robot
void stop(){
  for (int i = 0; i < 4; i++){
    wheels[i]->setVelocity(0);
  }
}
    
    
int main(int argc, char **argv) {
  init_distancesensors();
  init_motors();
  init_compass();
  
  while (robot->step(timeStep) != -1) {
    float left_distance_sensor = ds[0]->getValue();
    float right_distance_sensor = ds[1]->getValue();
    float front_distance_sensor = ds[2]->getValue();
    
    if (front_distance_sensor < 1500.0){
      if ((left_distance_sensor < 4000.0) and (right_distance_sensor > 4000.0)){
        turn_right();
      }
      //else if (left_distance_sensor > 4000 and right_distance_sensor < 4000)
      else{
        turn_left();
      }
    }
      
    else{
      if (left_distance_sensor > 4000 and right_distance_sensor > 4000){       
        stop();
      }   
      else{
        go_forward();
      } 
    }

  };
  delete robot;
  return 0;
}