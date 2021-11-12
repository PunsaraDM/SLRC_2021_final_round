#include <webots/Emitter.hpp>
#include <iostream>
#include <math.h>
#include <webots/Robot.hpp>
#include <webots/LED.hpp>
#include <string>
#include <vector>
#include <cstring>

#define TIME_STEP 64

using namespace std;
using namespace webots;

Robot *robot = new Robot();

Emitter *emitter;
char rec_name[10] = {"emitter"};

vector<vector<int>> var{{0}, {1}, {0, 5}, {2}, {0, 3}};
string TxMessage;

int juncType = 1;
vector<int> pathState{0, 1, 2, 3}; // down path is always keep as notaccesible since robot coming from that side
vector<int> boxType{0,5};

void initEmitter()
{
  emitter = robot->getEmitter(rec_name);
}

int main(int argc, char **argv)
{
  initEmitter();

  // for (int i = 0; i < 5; i++)
  // {
  //   TxMessage = TxMessage + to_string(var[i][0]);
  //   if (i == 2 or i == 4)
  //     TxMessage = TxMessage + to_string(var[i][1]);
  // }
  // cout << "txmsg: " << TxMessage << endl;


  TxMessage = to_string(juncType);

  for (int i = 0; i < 4; i++)
  {
    TxMessage = TxMessage + to_string(pathState[i]);
  }
  TxMessage = TxMessage + to_string(boxType[0]);
  TxMessage = TxMessage + to_string(boxType[1]);

  

  while (robot->step(TIME_STEP) != -1)
  {
    cout << "txmsg: " << TxMessage << endl;
    emitter->send(TxMessage.c_str(), (int)strlen(TxMessage.c_str()) + 1);
  }
  delete robot;
  return 0; // EXIT_SUCCESS
}