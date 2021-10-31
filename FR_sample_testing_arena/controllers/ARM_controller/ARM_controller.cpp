#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include <webots/PositionSensor.hpp>

#define TIME_STEP 32
using namespace webots;
using namespace std;

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
  while (robot->step(timeStep) != -1) {
    if (flag==1){
    arm_base_move(0.04);
    arm_grab_box(0.06,0.06);
    arm_vertical_move(0.05);
    flag = 0;
    }
    
    //arm_base_move(-0.07);
    arm_vertical_move(0.03);
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
