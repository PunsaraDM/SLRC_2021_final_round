#include <webots/Receiver.hpp>
#include <iostream>
#include <math.h>
#include <webots/Robot.hpp>
#include <webots/LED.hpp>
#include <string>
#include <vector>

#define TIME_STEP 64

using namespace std;
using namespace webots;

Robot *robot = new Robot();

Receiver *receiver;
char rec_name[10] = {"receiver"};

string tempStr = "";
vector<vector<int>> varRx{{0}, {0}, {0, 0}, {0}, {0, 0}};

int juncTypeRx = 0;
vector<int> pathStateRx{0, 0, 0, 0}; // down path is always keep as notaccesible since robot coming from that side
vector<int> boxTypeRx{0, 0};

void initReceiver()
{
  receiver = robot->getReceiver(rec_name);
  receiver->enable(TIME_STEP);
}


void GetReceiver()
{
  if (receiver->getQueueLength() > 0)
  {
    string TxMessage((const char *)receiver->getData());
    // const int *number = receiver->getData();
    // const char *number((const char *)receiver->getData());

    cout <<"received msg "<< TxMessage << endl;
    receiver->nextPacket();
    
    // for (int i = 0; i < 7; i++)
    // {
    //   tempStr = TxMessage[i];
    //   if (i == 0)
    //     varRx[0][0] = stoi(tempStr);
    //   else if (i == 1)
    //     varRx[1][0] = stoi(tempStr);
    //   else if (i == 2)
    //     varRx[2][0] = stoi(tempStr);
    //   else if (i == 3)
    //     varRx[2][1] = stoi(tempStr);
    //   else if (i == 4)
    //     varRx[3][0] = stoi(tempStr);
    //   else if (i == 5)
    //     varRx[4][0] = stoi(tempStr);
    //   else if (i == 6)
    //     varRx[4][1] = stoi(tempStr);
    // }

    //cout << "rxmsg: " << varRx[0][0] << varRx[1][0] << varRx[2][0] << varRx[2][1] << varRx[3][0] << varRx[4][0] << varRx[4][1] << endl;

    for (int i = 0; i < 7; i++)
    {
      tempStr = TxMessage[i];
      if (i == 0)
        juncTypeRx = stoi(tempStr);
      else if (i == 1)
        pathStateRx[0] = stoi(tempStr);
      else if (i == 2)
        pathStateRx[1] = stoi(tempStr);
      else if (i == 3)
        pathStateRx[2] = stoi(tempStr);
      else if (i == 4)
        pathStateRx[3] = stoi(tempStr);
      else if (i == 5)
        boxTypeRx[0] = stoi(tempStr);
      else if (i == 6)
        boxTypeRx[1] = stoi(tempStr);
    }

    cout << "juncType: " << juncTypeRx << endl;
    cout << "pathState: " << pathStateRx[0] << pathStateRx[1] << pathStateRx[2] << pathStateRx[3] << endl;
    cout << "boxType: " << boxTypeRx[0] << boxTypeRx[1] << endl;

  }
}

int main(int argc, char **argv)
{
  initReceiver();

  while (robot->step(TIME_STEP) != -1)
  {
    GetReceiver();
  }
  delete robot;
  return 0; // EXIT_SUCCESS
}