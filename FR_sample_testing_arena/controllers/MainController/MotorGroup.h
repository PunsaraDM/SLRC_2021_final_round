#include <webots/Motor.hpp>


using namespace webots;

#ifndef MOTORGROUP_H
#define MOTORGROUP_H
class Navigator;

class MotorGroup
{
public:
    MotorGroup();

    void initialize(Navigator* follower);
    void init_motors(Navigator* follower);

    void enable_motor_velocity_control();
    void set_velocity(float leftSpeed, float rightSpeed);
    void robot_stop();
    void set_position(double leftPosition, double rightPosition);
    void set_linear_target(int motor, double tartget, double vel);
    void set_linear_target_PID(int motor, double tartget, double vel);
    //void set_control_pid(float val1, float val2,float val3);

    void qtr_servo(int tartget, double vel);

    // void set_velocity_servo(int servo, float speed);
    // void set_position_servo(int servo, double position);

private:
    Navigator *follower;

    static const int motor_count = 7;
    Motor *motors[motor_count];
    char motors_names[motor_count][20] = {"motor_left", "motor_right", "base_motor","vertical_motor","gripper_left_motor","gripper_right_motor","qtr_motor"};

    double qtr_servo_target[2] = {0,1.5707};
};

#endif