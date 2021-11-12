// Team Name: Xyber Robotics

#include <webots/Receiver.hpp>
#include <iostream>
#include <math.h>
#include <webots/Robot.hpp>
#include <webots/LED.hpp>

#define TIME_STEP 64

using namespace std;
using namespace webots;

Robot *robot = new Robot();

Receiver *receiver;
char rec_name[10] = {"receiver"};

LED *led[4];
char led_name[4][6] = {"led_1", "led_2", "led_3", "led_4"};

void initReceiver()
{ //this function initialize the receiver. This is an example.Don't need to change anything in here.
  receiver = robot->getReceiver(rec_name);
  receiver->enable(TIME_STEP);
}

void initLED()
{
  //*******************************************************************************
  //initialize the LEDs here
  for (int i = 0; i <= 3; i++)
  {
    led[i] = robot->getLED(led_name[i]);
  }
  //*******************************************************************************
}

void ReceiveAndDecode()
{
  //*******************************************************************************
  //type your code to receive the message, decode it and get the required colour numbers
  if (receiver->getQueueLength() > 0)
  {
    string message((const char *)receiver->getData());
    int led_num = message[0] - '0' - 1;
    int col_num = message[2] - '0';
    led[led_num]->set(col_num);
    receiver->nextPacket();
  }
  //*******************************************************************************
}

int main(int argc, char **argv)
{

  initReceiver();
  initLED();

  //*********************************************************************************************
  //call the ReceiveAndDecode() function and light up the LEDs in here.Use a while loop if needed.
  while (robot->step(TIME_STEP) != -1)
  {
    ReceiveAndDecode();
  }

  //**********************************************************************************************

  delete robot;
  return 0;
}