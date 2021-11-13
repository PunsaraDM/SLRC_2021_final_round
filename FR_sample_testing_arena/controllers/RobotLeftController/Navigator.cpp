#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include "Navigator.h"
#include "SensorGroup.h"
#include "MotorGroup.h"
#include <math.h>
#include <cmath>
#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <cstring>

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

void Navigator::passive_wait_wheel(double targetLeft, double targetRight)
{
  double dif;
  double effectiveLeft, effectiveRight;
  do
  {
    if (step(TIME_STEP) == -1)
      exit(EXIT_SUCCESS);

    effectiveLeft = sensorGroup->get_encoder_val(0);
    effectiveRight = sensorGroup->get_encoder_val(1);
    dif = min(fabs(effectiveLeft - targetLeft), fabs(effectiveRight - targetRight));
  } while (dif > 0.1);
}

// TODO: give additional parameters to above function to specify encoders

void Navigator::passive_wait_servo(int servo, double target)
{
  double dif;
  double effective;
  do
  {
    if (step(TIME_STEP) == -1)
      exit(EXIT_SUCCESS);

    effective = sensorGroup->get_encoder_val(servo);
    dif = fabs(effective - target); // check here again
  } while (dif > DELTA_single);
}

void Navigator::delay(int time)
{
  int target = time / TIME_STEP;
  int count = 0;
  while (step(TIME_STEP) != -1)
  {
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

  double controlValue = (error * Kp) + (error - lineFollowPreviousError) * Kd;

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
  // cout<<leftSpeed<<"  "<<rightSpeed<<endl;
  motorGroup->set_velocity(leftSpeed, rightSpeed);
}

void Navigator::go_forward_specific_distance(double distance)
{
  // sensorGroup->stabilize_encoder(this);

  double initialLeftENcount = sensorGroup->get_encoder_val(EN_LEFT);
  double initialRightENcount = sensorGroup->get_encoder_val(EN_RIGH);

  // motorGroup->set_control_pid(8, 0, 0);
  double angle = distance / WHEEL_RADIUS;

  motorGroup->set_position(angle + initialLeftENcount, angle + initialRightENcount);
  motorGroup->set_velocity(7.5, 7.5);

  passive_wait_wheel(angle + initialLeftENcount, angle + initialRightENcount);
  motorGroup->enable_motor_velocity_control();
}

void Navigator::go_backward_specific_distance(double distance)
{
  // sensorGroup->stabilize_encoder(this);

  double initialLeftENcount = sensorGroup->get_encoder_val(EN_LEFT);
  double initialRightENcount = sensorGroup->get_encoder_val(EN_RIGH);

  // motorGroup->set_control_pid(8, 0, 0);
  double angle = distance / WHEEL_RADIUS;

  motorGroup->set_position(initialLeftENcount - angle, initialRightENcount - angle);
  motorGroup->set_velocity(7.5, 7.5);

  passive_wait_wheel(initialLeftENcount - angle, initialRightENcount - angle);
  motorGroup->enable_motor_velocity_control();
}

// Function to get the initial postion by number of 90's from north
double Navigator::getComDir()
{
  const double *cVals = sensorGroup->get_compass_value();
  double rad = atan2(cVals[0], cVals[2]);
  double bearing = (rad - 1.5708) / M_PI * 180.0;
  if (bearing < 0.0)
  {
    bearing += 360.0;
  }

  bearing = remainder(bearing, 360.0);
  bearing = round(bearing * turnAccu) / turnAccu;
  double ninetyCount = round(turnAccu * bearing / 90.0) / turnAccu;

  return remainder(ninetyCount, 4.0);
}

// Function to turn robot to a certain angle
void Navigator::turnAng(float angle)
{
  double initNinetyCount;
  initNinetyCount = getComDir(); //# of 90's in initial position

  double targetNinetyCount = round(turnAccu * remainder((initNinetyCount + angle / 90.0), 4.0)) / turnAccu; //# of 90's for the target
  targetNinetyCount = remainder(targetNinetyCount, 4.0);

  int initTurnSide = 1;
  if (angle < 0.0)
  {
    initTurnSide = -1;
  }

  float expoVal = 1.0;
  float turnError = 0.0;
  int turnSide;

  while ((step(TIME_STEP) != -1) && (initNinetyCount != targetNinetyCount))
  {
    initNinetyCount = getComDir();
    // cout<<turnError<<endl;
    if (targetNinetyCount < initNinetyCount)
    {
      turnError = targetNinetyCount + 4 - initNinetyCount;
    }
    else
    {
      turnError = targetNinetyCount - initNinetyCount;
    }

    if (turnError <= 2)
    {
      turnSide = 1;
    }
    else
    {
      turnSide = -1;
    }

    if (initTurnSide != turnSide)
    {
      expoVal *= expoValTurnThresh;
      initTurnSide = turnSide;
    }

    // set default for the right turn
    double leftSpeed = turnSpeed * turnSide * expoVal;
    double rightSpeed = -turnSpeed * turnSide * expoVal;
    // cout<<leftSpeed<<"   "<<rightSpeed<<endl;
    motorGroup->set_velocity(leftSpeed, rightSpeed);
  }
  motorGroup->robot_stop();
}

void Navigator::turn(int dir)
{
  if (dir == LEFT)
    turnAng(-90.0);
  else if (dir == RIGHT)
    turnAng(90.0);
  else if (dir == DOWN)
    turnAng(180.0);
}

////////////////////////////////////////////////////////////////////ARM////////////////////////////////////////////////////

void Navigator::arm_base_move(double target)
{
  motorGroup->set_linear_target_PID(2, target, linearMotorVelocity);
  passive_wait_servo(2, target);
}

void Navigator::arm_vertical_move(double target)
{
  motorGroup->set_linear_target(3, target, linearMotorVelocity);
  passive_wait_servo(3, target);
}

void Navigator::arm_grab_box(double targetLeft, double targetRight)
{
  motorGroup->set_linear_target_PID(4, targetLeft, linearMotorVelocity);
  motorGroup->set_linear_target_PID(5, targetRight, linearMotorVelocity);
  passive_wait(targetLeft, targetRight);
}

//////////////////////////new robot arm codes////////////////////////

//---box search functions
void Navigator::place_white_box_before_search() // place the white box before searching and centering a color box
{
  arm_base_move(distArmBase_place);
  arm_vertical_move(verticalGround);
  arm_grab_box(grabDist_min, grabDist_min); // release the box
  arm_vertical_move(verticalHighest);
}

void Navigator::grab_white_box_after_search() // grab the white box before searching and centering a color box
{
  arm_base_move(distArmBase_place - 0.02);
  arm_vertical_move(verticalGround);
  arm_base_move(distArmBase_place);
  arm_grab_box(grabDistBlue, grabDistBlue);
  arm_vertical_move(verticalHighest);
  arm_base_move(distArmBase_carry);
}

int Navigator::search_box_color(int level)
{
  double currentDist = distArmBase_place + 0.07;
  arm_base_move(currentDist);
  arm_grab_box(grabDist_min, grabDist_min);

  if (level == 1) // lower search
    arm_vertical_move(verticalGround);
  else if (level == 2) // upper search
    arm_vertical_move(verticalGround + 0.04);

  while (step(TIME_STEP) != -1)
  {
    if ((sensorGroup->get_distance_value(0)) < 0.2)
    {
      double distToDetectLocal = grabDistToDetectColor;
      arm_grab_box(distToDetectLocal, distToDetectLocal);
      bool isDetected = true;
      while (isDetected)
      {
        int colorBox = sensorGroup->get_colour(0);
        if (colorBox == 1)
        { // Red box
          cout << "RED BOX" << endl;
          if (level == 1)
          {
            currentDist += 0.005;
            arm_base_move(currentDist);
            delay(500);
            arm_grab_box(grabDistRed, grabDistRed);
            delay(500);
            centre_box(distArmBase_centre_red);
            arm_grab_box(grabDist_min, grabDist_min);
          }
          return RED;
        }
        else if (colorBox == 2)
        { // Green box
          cout << "GREEN BOX" << endl;
          if (level == 1)
          {
            currentDist += 0.005;
            arm_base_move(currentDist);
            delay(500);
            arm_grab_box(grabDistGreen, grabDistGreen);
            delay(500);
            centre_box(distArmBase_centre_green);
            arm_grab_box(grabDist_min, grabDist_min);
          }
          return GREEN;
        }
        else if (colorBox == 3)
        { // Blue box
          cout << "BLUE BOX" << endl;
          if (level == 1)
          {
            currentDist += 0.005;
            arm_base_move(currentDist);
            delay(500);
            arm_grab_box(grabDistBlue, grabDistBlue);
            delay(500);
            centre_box(distArmBase_centre_blue);
            arm_grab_box(grabDist_min, grabDist_min);
          }
          return BLUE;
        }
        else if (colorBox == 4)
        { // Blue box
          cout << "WHITE BOX" << endl;
          if (level == 1)
          {
            currentDist += 0.005;
            arm_base_move(currentDist);
            delay(500);
            arm_grab_box(grabDistBlue, grabDistBlue);
            delay(500);
            centre_box(distArmBase_centre_white);
            arm_grab_box(grabDist_min, grabDist_min);
          }
          return WHITE_CLR;
        }
        else
        { // Not detected any color
          cout << "Not yet detect any color" << endl;
          distToDetectLocal += 0.005;
          arm_grab_box(distToDetectLocal, distToDetectLocal);
        }
      }
    }
    else if (distArmBase_mid <= currentDist)
    {
      if (level == 1)
        cout << "lower level : NO BOX" << endl;
      else
        cout << "middle level : NO BOX" << endl;
      return NO_COLOR;
    }
    else
    {
      currentDist += 0.005;
      arm_base_move(currentDist);
    }
  }
  return 0;
}

void Navigator::centre_box(double distance) // centre the box
{
  arm_base_move(distance);
}

void Navigator::arm_parking() // place the robot arm in the intermediate position
{
  arm_grab_box(grabDist_min, grabDist_min);
  arm_vertical_move(verticalHighest);
  arm_base_move(distArmBase_carry);
}

void Navigator::arm_carrying() // place the robot arm in the intermediate position
{
  arm_vertical_move(verticalHighest);
  arm_base_move(distArmBase_carry);
}

void Navigator::box_search_algo(bool haveBox)
{
  if (var[INV_PATCH][BOX_CARRY] == TRUE)
    place_white_box_before_search(); // place the white box before searching and centering a color box

  int clr = search_box_color(1); // seach for a lower box

  if (clr < 5)
  {
    cout << "found lower box" << endl;
    boxType[LOWER - 1] = clr; // updates box count and color

    if (clr == 4) // updating junction types
      juncType = WHITE_PATCH;
    else
      juncType = COLORED;
  }
  else // didnt found a box
  {
    juncType = PATCHNOBOX;
  }

  if (clr < 4)
  {
    clr = search_box_color(2); // seach for a upper box
    if (clr < 5)
    {
      boxType[MIDDLE - 1] = clr; // updates box count and color
      cout << "found upper box" << endl;
    }
  }

  // if white box need to be picked call function
  // currently robot doesnt have a box and next box is a white one
  if ((var[INV_PATCH][BOX_CARRY] == FALSE) and (boxType[LOWER - 1] == WHITE_CLR))
  {
    cout << "grabbed a white box while in discovery satate." << endl;
    grab_box(WHITE_CLR, LOWER); // box_carry need to update??
  }
  else
    arm_parking();

  if (var[INV_PATCH][BOX_CARRY] == TRUE)
    grab_white_box_after_search(); // grab the white box after searching and centering a color box
}

//---box grabbing
void Navigator::grab_box(int color, int level)
{
  if (level == 1) // lower level
    arm_vertical_move(verticalGround);
  else if (level == 2) // upper level
    arm_vertical_move(verticalGround + 0.04);
  if (color == 1)
  {
    arm_base_move(distArmBase_centre_red);
    arm_grab_box(grabDistRed, grabDistRed);
  }
  else if (color == 2)
  {
    arm_base_move(distArmBase_centre_green);
    arm_grab_box(grabDistGreen, grabDistGreen);
  }
  else if (color == 3)
  {
    arm_base_move(distArmBase_centre_blue);
    arm_grab_box(grabDistBlue, grabDistBlue);
  }
  else if (color == 4)
  {
    arm_base_move(distArmBase_centre_white);
    arm_grab_box(grabDistBlue, grabDistBlue);
  }
  arm_carrying();
  carryingBox = color; // is this the proper place??
}

void Navigator::place_white_box_in_red_square()
{
  arm_base_move(distArmBase_inverted_place);
  arm_vertical_move(verticalGround);
  arm_grab_box(grabDistToDetectColor, grabDistToDetectColor); // release the box
  arm_vertical_move(verticalHighest);
  arm_base_move(distArmBase_carry);
}

void Navigator::grab_white_box_from_red_square()
{
  arm_base_move(distArmBase_inverted_place);
  arm_vertical_move(verticalGround);
  arm_grab_box(grabDistBlue, grabDistBlue);
  arm_vertical_move(verticalHighest);
  arm_base_move(distArmBase_carry);
}

void Navigator::place_box(int color) // at the placement square
{
  if (color == RED)
  {
    arm_base_move(0.1512);
    arm_vertical_move(verticalGround);
    arm_grab_box(grabDist_min, grabDist_min); // release the box
  }
  else if (color == GREEN)
  {
    arm_base_move(0.0512 - 0.01);
    arm_vertical_move(verticalGround);
    arm_base_move(0.0512);
    arm_grab_box(0.0232, 0.1432); // release the box
    arm_grab_box(grabDist_min, 0.1232 + 0.02);
  }
  else if (color == BLUE)
  {
    arm_base_move(0.0512 - 0.01);
    arm_vertical_move(verticalGround);
    arm_base_move(0.0512);
    arm_grab_box(0.1632, 0.0232); // release the box
    arm_grab_box(0.1632 - 0.02, grabDist_min);
  }
  arm_carrying();
  arm_grab_box(grabDist_min, grabDist_min);
}
/*
void Navigator::final_stack_box(int color, int row) // at the placement square
{
  // function calls after go_forward_specific_distance(0.099);
  if (color == GREEN)
  {
    if (row == 2)
      go_forward_specific_distance(-0.01);
    arm_base_move(0.0510);
    arm_grab_box(0.0232 - 0.01, 0.1432 - 0.01);
    arm_vertical_move(verticalGround);
    arm_grab_box(0.0232, 0.1432);
    arm_grab_box(grabDistGreen, grabDistGreen);
    arm_vertical_move(verticalHighest);
    if (row == 2)
      go_forward_specific_distance(0.01);
    arm_base_move(0.1512 + 0.01);
    arm_vertical_move(verticalGround + 0.04);
    arm_grab_box(grabDist_min, grabDist_min);
  }
  else if (color == BLUE)
  {
    if (row == 2)
      go_forward_specific_distance(-0.01);
    arm_base_move(0.0510);
    arm_grab_box(0.1632 - 0.01, 0.0232 - 0.01);
    arm_vertical_move(verticalGround);
    arm_grab_box(0.1632, 0.0232);
    arm_grab_box(grabDistBlue, grabDistBlue);
    arm_vertical_move(verticalHighest);
    if (row == 2)
      go_forward_specific_distance(0.01);
    arm_base_move(0.1512 + 0.02);
    arm_vertical_move(verticalGround + 0.08);
    arm_grab_box(grabDist_min, grabDist_min);
  }
  arm_carrying();
  arm_grab_box(grabDist_min, grabDist_min);
}*/

void Navigator::final_stack_box(int color) // at the placement square
{
  // function calls after go_forward_specific_distance(0.099);
  if (color == GREEN)
  {
    //arm_base_move(0.0510);
    arm_grab_box(0.0232 - 0.02, 0.1432 - 0.02);
    arm_vertical_move(verticalGround);
    arm_base_move(0.0510);
    arm_grab_box(0.0232, 0.1432);
    arm_grab_box(grabDistGreen, grabDistGreen);
    arm_vertical_move(verticalHighest);
    arm_base_move(0.1512 + 0.01);
    arm_vertical_move(verticalGround + 0.04);
    arm_grab_box(grabDist_min, grabDist_min);
  }
  else if (color == BLUE)
  {
    //arm_base_move(0.0510);
    arm_grab_box(0.1632 - 0.02, 0.0232 - 0.02);
    arm_vertical_move(verticalGround);
    arm_base_move(0.0510);
    arm_grab_box(0.1632, 0.0232);
    arm_grab_box(grabDistBlue, grabDistBlue);
    arm_vertical_move(verticalHighest);
    arm_base_move(0.1512 + 0.02);
    arm_vertical_move(verticalGround + 0.08);
    arm_grab_box(grabDist_min, grabDist_min);
  }
  arm_carrying();
  arm_grab_box(grabDist_min, grabDist_min);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Navigator::is_junction_detected()
{
  if ((sensorGroup->get_digital_value(LINE_DETECT_LEFT) == WHITE) or (sensorGroup->get_digital_value(LINE_DETECT_RIGHT) == WHITE))
    return true;
  else
    return false;
}

void Navigator::visit_junction(int junctype)
{
  if (junctype == NORMAL)
    visit_normal_junc();
  else if (junctype == INVERTED or junctype == INVERTWHITE)
    visit_inv_junc();
  else if ((junctype == WHITE_PATCH) or (junctype == COLORED) or (junctype == PATCHNOBOX))
    visit_white_patch(); // need lower or upper box and box color
}

void Navigator::discover_junction()
{
  go_forward_specific_distance(0.005);

  if (((sensorGroup->get_digital_value(LINE_DETECT_LEFT) == WHITE) and (sensorGroup->get_digital_value(QTR_0) == WHITE) and (sensorGroup->get_digital_value(LINE_DETECT_RIGHT) == WHITE) and (sensorGroup->get_digital_value(QTR_7) == WHITE)) and ((sensorGroup->get_digital_value(QTR_3) == BLACK) or (sensorGroup->get_digital_value(QTR_4) == BLACK)))
  {
    // inverted patch detected
    cout << "inverted" << endl;
    juncType = INVERTED;
    // if inverted and invalid
    // next direction back
    if (var[INV_PATCH][BOX_CARRY] == TRUE)
      discover_inv_junc(var[INV_PATCH][INV_DIRECTION]);
    // else junction will not be descovered. pathStates will be default values and juncType will be INVERTED
    print_pathState();
  }
  else if ((sensorGroup->get_digital_value(LINE_DETECT_LEFT) == WHITE) or (sensorGroup->get_digital_value(LINE_DETECT_RIGHT) == WHITE))
  {

    //   (((sensorGroup->get_digital_value(LINE_DETECT_LEFT) == WHITE) and (sensorGroup->get_digital_value(QTR_0) == WHITE) and (sensorGroup->get_digital_value(QTR_1) == WHITE)
    // and (sensorGroup->get_digital_value(QTR_2) == WHITE) and (sensorGroup->get_digital_value(QTR_3) == WHITE)) or
    // ((sensorGroup->get_digital_value(LINE_DETECT_RIGHT) == WHITE) and (sensorGroup->get_digital_value(QTR_4) == WHITE) and (sensorGroup->get_digital_value(QTR_5) == WHITE)
    // and (sensorGroup->get_digital_value(QTR_6) == WHITE) and (sensorGroup->get_digital_value(QTR_7) == WHITE)))
    // normal juction or a white patch
    discover_path(RIGHT);
    discover_path(LEFT);
    go_forward_specific_distance(0.035); // forward untill line papsses

    if ((sensorGroup->get_digital_value(LINE_DETECT_LEFT) == WHITE) and (sensorGroup->get_digital_value(QTR_0) == WHITE) and (sensorGroup->get_digital_value(QTR_1) == WHITE) and (sensorGroup->get_digital_value(QTR_2) == WHITE) and (sensorGroup->get_digital_value(QTR_3) == WHITE) and (sensorGroup->get_digital_value(LINE_DETECT_RIGHT) == WHITE) and (sensorGroup->get_digital_value(QTR_4) == WHITE) and (sensorGroup->get_digital_value(QTR_5) == WHITE) and (sensorGroup->get_digital_value(QTR_6) == WHITE) and (sensorGroup->get_digital_value(QTR_7) == WHITE))
    {
      // white patch detected
      cout << "white patch" << endl;
      discover_white_patch();
      print_pathState();
    }
    else
    {
      // a normal junction
      cout << "normal junction" << endl;
      juncType = NORMAL;
      discover_path(UP);
      print_pathState();
      go_forward_specific_distance(0.102);
    }
  }
  else
  {
    cout << "couldnt identify a junction" << endl;
  }
}

void Navigator::discover_path(int side) // side shouldnt be down
{
  int clrSensor = CS_FRONT;

  if (side == LEFT)
    clrSensor = CS_LEFT;
  else if (side == RIGHT)
    clrSensor = CS_RIGHT;

  int color = sensorGroup->get_colour(clrSensor);
  // cout<<"side: "<<side<<" color: "<<color<<endl;

  if (color == WHITE_CLR)
    pathState[side] = DISCOVERED;
  else if (color == RED)
    pathState[side] = NOPATH;
  else if (color == NO_COLOR)
    pathState[side] = NOTACCESIBLE;
}

void Navigator::discover_white_patch()
{
  motorGroup->qtr_servo(QTR_UP, 2.0);
  delay(500);
  go_forward_specific_distance(0.06); // forward until side lines discover
  discover_path(RIGHT);
  discover_path(LEFT);
  box_search_algo();
  go_forward_specific_distance(0.115); // forward until front line appears to clr sensor
  motorGroup->qtr_servo(QTR_DOWN, 2.0);
  delay(1200); // delay to dqtr down
  discover_path(UP);
  go_forward_specific_distance(0.019); // forward until wheels come to the side lines
}

void Navigator::visit_white_patch(bool initial)
{
  motorGroup->qtr_servo(QTR_UP, 2.0);
  delay(500);
  go_forward_specific_distance(0.099); // forward until side lines discover

  if (initial)
  {
    int clr = search_box_color(1);
    var[BOX_GRAB][COLOR] = clr;
    var[BOX_GRAB][POSITION] = LOWER;
  }

  if (var[BOX_GRAB][POSITION] > 0)
    grab_box(var[BOX_GRAB][COLOR], var[BOX_GRAB][POSITION]);
  // else to neglect boxes

  motorGroup->qtr_servo(QTR_DOWN, 2.0); //
  if (initial == false)
    go_forward_specific_distance(0.13); // forward until wheels come to the side lines
}

void Navigator::discover_inv_junc(int boxDir)
{
  if (var[INV_PATCH][INV_DIRECTION] == LEFT)
  {
    motorGroup->qtr_servo(QTR_UP, 2.0);
    turn(LEFT);
    place_white_box_in_red_square();
    motorGroup->qtr_servo(QTR_DOWN, 2.0);
    turn(RIGHT);
  }
  else if (var[INV_PATCH][INV_DIRECTION] == RIGHT)
  {
    motorGroup->qtr_servo(QTR_UP, 2.0);
    turn(RIGHT);
    place_white_box_in_red_square();
    motorGroup->qtr_servo(QTR_DOWN, 2.0);
    turn(LEFT);
  }
  // else: go without placing the box
  go_forward_specific_distance(0.09); // forward until side lines discover
  discover_path(RIGHT);
  discover_path(LEFT);
  go_forward_specific_distance(0.115); // forward until front line appears to clr sensor
  discover_path(UP);
  go_forward_specific_distance(0.02); // forward until wheels come to the side lines
}

void Navigator::visit_inv_junc()
{
  // go without placing the box
  go_forward_specific_distance(0.226); // forward until wheels come to the side lines
}

void Navigator::visit_normal_junc()
{
  go_forward_specific_distance(0.142);
}

void Navigator::stall_cell()
{
  motorGroup->qtr_servo(QTR_UP, 2.0);
  delay(500);
  go_backward_specific_distance(0.132);
  grab_box(var[BOX_GRAB][COLOR], var[BOX_GRAB][POSITION]);
  go_forward_specific_distance(0.132);
  motorGroup->qtr_servo(QTR_DOWN, 2.0);
  delay(500);
}

void Navigator::resetVariables()
{
  for (int i = 0; i < 2; i++)
    boxType[i] = 0;

  pathState[0] = UNDISCOVERED;
  pathState[1] = UNDISCOVERED;
  pathState[2] = NOTACCESIBLE; // down path is always keep as notaccesible since robot coming from that side
  pathState[3] = UNDISCOVERED;
  juncType = UNDISCOVERED;
}

void Navigator::print_pathState()
{
  int state = 0;
  for (int i = 0; i < 4; i++)
  {
    state = pathState[i];
    if (state == DISCOVERED)
      cout << "DISCOVERED ";
    else if (state == NOPATH)
      cout << "NOPATH ";
    else if (state == NOTACCESIBLE)
      cout << "NOTACCESIBLE ";
    else if (state == UNDISCOVERED)
      cout << "UNDISCOVERED ";
  }
  cout << "" << endl;
}

void Navigator::follow_line_until_junc_detect()
{
  while (step(TIME_STEP) != -1)
  {
    follow_line(0.0007, 0.002, 5.5, 6.5, 7.5);
    if (is_junction_detected())
    {
      motorGroup->robot_stop();
      break;
    }
  }
}
/*
void Navigator::goto_placement_cell(bool final)
{
  visit_normal_junc(); // go forward
  follow_line_until_junc_detect();

  motorGroup->qtr_servo(QTR_UP, 2.0);
  delay(1500);

  bool distanceAdjust = false;

  if (carryingBox == RED)
    go_forward_specific_distance(0.099);
  else if (carryingBox == BLUE and greenBoxCount == 0)
  {
    go_forward_specific_distance(0.099);
    blueBoxCount = 1;
  }
  else if (carryingBox == GREEN and blueBoxCount == 0)
  {
    go_forward_specific_distance(0.099);
    greenBoxCount = 1;
  }
  else
  {
    distanceAdjust = true;
    go_forward_specific_distance(0.089);
  }

  place_box(carryingBox);

  if(final)
  {
    if (distanceAdjust){
      go_forward_specific_distance(0.01);
    }
    final_stack_box(GREEN, greenBoxCount);
    final_stack_box(BLUE, blueBoxCount);
    go_backward_specific_distance(0.15);
    motorGroup->qtr_servo(QTR_DOWN, 2.0);
  }
  else
  {
    go_backward_specific_distance(0.15);
    motorGroup->qtr_servo(QTR_DOWN, 2.0);
    turn(BACK);
    follow_line_until_junc_detect();
    visit_normal_junc();
  }
  carryingBox = NO_COLOR;

}
*/

void Navigator::goto_placement_cell(bool final)
{
  boxCountPlacement += 1;
  if (boxCountPlacement != 2)
  {
    visit_normal_junc(); // go forward
  }

  follow_line_until_junc_detect();
  motorGroup->qtr_servo(QTR_UP, 2.0);
  delay(500);

  go_forward_specific_distance(0.109);

  place_box(carryingBox);

  if (final)
  {
    final_stack_box(GREEN);
    final_stack_box(BLUE);
    go_backward_specific_distance(0.15);
    motorGroup->qtr_servo(QTR_DOWN, 2.0);
  }
  else
  {
    go_backward_specific_distance(0.15);
    motorGroup->qtr_servo(QTR_DOWN, 2.0);
    turn(BACK);
    follow_line_until_junc_detect();
    visit_normal_junc();
  }
  carryingBox = NO_COLOR;
}

void Navigator::initial_phase()
{
  go_forward_specific_distance(0.1);
  follow_line_until_junc_detect();
  visit_normal_junc();
  turn(LEFT);
  follow_line_until_junc_detect();
  // var[BOX_GRAB][COLOR] = WHITE_CLR;
  // var[BOX_GRAB][POSITION] = LOWER;
  visit_white_patch(true);
  turn(DOWN);
  follow_line_until_junc_detect();
  visit_normal_junc();
  turn(LEFT);
  follow_line_until_junc_detect();
}

void Navigator::one_cell()
{
  //[NAVIGATE_STATE , DIRECTION , INV_PATCH[BOX_CARRY,INV_DIRECTION],JUNC_TYPE, BOX_GRAB[POSITION,COLOR]]

  string TxMessage = make_TxMessage();
  sensorGroup->emmit(TxMessage);

  while (step(TIME_STEP) != -1)
  {
    if (sensorGroup->receiver_ready())
    {
      string RxMessage = sensorGroup->receive();
      update_var(RxMessage);
      break;
    }
  }
  //var = pathFinder->travel_maze(juncType, pathState, boxType); // need to know we are carrying box
  if (var[NAVIGATE_STATE][0] != STALL)
  {
    turn(var[DIRECTION][0]);
    follow_line_until_junc_detect();
  }
  if (var[NAVIGATE_STATE][0] == SEARCH)
  {
    resetVariables();
    cout << "in discovery state" << endl;
    discover_junction();
  }
  else if (var[NAVIGATE_STATE][0] == VISIT)
  {
    cout << "in visiting state" << endl;
    visit_junction(var[JUNC_TYPE][0]);
  }
  else if (var[NAVIGATE_STATE][0] == PLACEMENT)
  {
    cout << "in PLACEMENT state" << endl;
    goto_placement_cell();
  }
  else if (var[NAVIGATE_STATE][0] == FINISH_PLACEMENT)
  {
    cout << "in FINISH_PLACEMENT state" << endl;
    goto_placement_cell(true);
    cout << "TASK COMPLETED" << endl;
    taskCompleted = true;
  }
  else if (var[NAVIGATE_STATE][0] == STALL)
  {
    cout << "in STALL state" << endl;
    stall_cell();
  }
}

void Navigator::update_var(string RxMessage)
{
  string tempStr;

  for (int i = 0; i < 7; i++)
  {
    tempStr = RxMessage[i];
    if (i == 0)
      var[0][0] = stoi(tempStr);
    else if (i == 1)
      var[1][0] = stoi(tempStr);
    else if (i == 2)
      var[2][0] = stoi(tempStr);
    else if (i == 3)
      var[2][1] = stoi(tempStr);
    else if (i == 4)
      var[3][0] = stoi(tempStr);
    else if (i == 5)
      var[4][0] = stoi(tempStr);
    else if (i == 6)
      var[4][1] = stoi(tempStr);
  }

  //cout << "rxmsg: " << var[0][0] << var[1][0] << var[2][0] << var[2][1] << var[3][0] << var[4][0] << var[4][1] << endl;
}

string Navigator::make_TxMessage()
{
  string TxMessage;
  int temp;
  for (int i = 0; i < 7; i++)
  {
    if (i == 0)
      temp = juncType;
    else if (i == 1)
      temp = pathState[0];
    else if (i == 2)
      temp = pathState[1];
    else if (i == 3)
      temp = pathState[2];
    else if (i == 4)
      temp = pathState[3];
    else if (i == 5)
      temp = boxType[0];
    else if (i == 6)
      temp = boxType[1];

    TxMessage = TxMessage + to_string(temp);
  }
  return TxMessage;
}

void Navigator::task()
{
  delay(500);
  initial_phase();
  follow_line_until_junc_detect();
  resetVariables();
  discover_junction();

  while (step(TIME_STEP) != -1)
  {
    // string TxMessage = make_TxMessage();
    // sensorGroup ->emmit(TxMessage);
    one_cell();
    if (taskCompleted)
      break;
  }

  // var = {{1},{0},{1,1},{4},{2,2}};
  // arm_carrying();
  // arm_grab_box(grabDistBlue, grabDistBlue);
  // follow_line_until_junc_detect();
  // delay(5000);
  // motorGroup->qtr_servo(QTR_UP, 2.0);
  // delay(2000);
  // go_forward_specific_distance(0.099); // 0.089 , 0.099
  // final_stack_box(GREEN, 2);
  // final_stack_box(BLUE, 1);
  // resetVariables();  // //visit_junction(var[JUNC_TYPE][0]);
  // discover_junction();
  // delay(3000);
}

void Navigator::test()
{
  // follow_line(0.0007,0.002,5.5,6.5,7.5);
  // cout<<sensorGroup->get_ir_value(0)<<endl;
  go_forward_specific_distance(0.045);
  motorGroup->robot_stop();
  //
  // cout<<sensorGroup->get_digital_value(8) << sensorGroup->get_digital_value(0)
  //   << sensorGroup->get_digital_value(1) << sensorGroup->get_digital_value(2)
  //   << sensorGroup->get_digital_value(3)
  //   << sensorGroup->get_digital_value(4)<< sensorGroup->get_digital_value(5)
  //   << sensorGroup->get_digital_value(6) << sensorGroup->get_digital_value(7)
  //   << sensorGroup->get_digital_value(9)<< endl;
  // cout<<sensorGroup->get_ir_value(8) <<" "<< sensorGroup->get_ir_value(0)<<" "
  //     << sensorGroup->get_ir_value(1) <<" "<< sensorGroup->get_ir_value(2)<<" "
  //     << sensorGroup->get_ir_value(3)<<" "
  //     << sensorGroup->get_ir_value(4)<<" "<< sensorGroup->get_ir_value(5)<<" "
  //     << sensorGroup->get_ir_value(6) <<" "<< sensorGroup->get_ir_value(7)<<" "
  //     << sensorGroup->get_ir_value(9)<< endl;
}
