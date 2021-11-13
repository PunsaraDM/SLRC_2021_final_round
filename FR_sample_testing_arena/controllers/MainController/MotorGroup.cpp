#include <webots/Motor.hpp>
#include "MotorGroup.h"
#include "Navigator.h"
#include <math.h>
#include <bits/stdc++.h>

#define TIME_STEP 32

using namespace webots;
using namespace std;

MotorGroup::MotorGroup(){}

void MotorGroup::initialize(Navigator* follower){
  follower = follower;
  init_motors(follower);
    
}

void MotorGroup::init_motors(Navigator* follower)
{
    for (int i = 0; i < motor_count; i++)
    {
        motors[i] = follower->getMotor(motors_names[i]);
        motors[i]->setPosition(INFINITY);
        motors[i]->setVelocity(0.0);
    }
}

void MotorGroup::enable_motor_velocity_control()
{
    for (int i = 0; i < 2; i++)
    {
        motors[i]->setPosition(INFINITY);
        motors[i]->setVelocity(0.0);
    }
}

void MotorGroup::set_velocity(float leftSpeed, float rightSpeed)
{
    if (leftSpeed > 7.498) leftSpeed = 7.498;
    else if (leftSpeed < -7.498) leftSpeed = -7.498;
    
    if (rightSpeed > 7.498) rightSpeed = 7.498;
    else if (rightSpeed < -7.498) rightSpeed = -7.498;

    motors[0]->setVelocity(leftSpeed);
    motors[1]->setVelocity(rightSpeed);
}

void MotorGroup::robot_stop()
{
    motors[0]->setVelocity(0.0);
    motors[1]->setVelocity(0.0);
}

void MotorGroup::set_position(double leftPosition, double rightPosition)
{
    motors[0]->setPosition(leftPosition); //left
    motors[1]->setPosition(rightPosition);
}

void MotorGroup::set_linear_target(int motor, double tartget, double vel)
{
    motors[motor]->setPosition(tartget);
    motors[motor]->setVelocity(vel);
}

void MotorGroup::set_linear_target_PID(int motor, double tartget, double vel)
{
    motors[motor]->setControlPID(4.5, 0, 0);
    motors[motor]->setPosition(tartget);
    motors[motor]->setVelocity(vel);
}
void MotorGroup::qtr_servo(int tartget, double vel)
{
    motors[6]->setPosition(qtr_servo_target[tartget]);
    motors[6]->setVelocity(vel);

}

// void MotorGroup::set_velocity_servo(int servo, float speed)
// {
//     motors[servo]->setVelocity(speed);
// }

// void MotorGroup::set_position_servo(int servo, double position)
// {
//     motors[servo]->setPosition(position);
// }

// void MotorGroup::set_control_pid(float val1, float val2,float val3){
//     for (int i = 0; i < 4; i++)
//     {
//         motors[i]->setControlPID(val1, val2, val3);
//     }

// }

