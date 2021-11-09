#include <webots/DistanceSensor.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Camera.hpp>
#include <webots/Compass.hpp>
#include "SensorGroup.h"
#include "Navigator.h"
#include <math.h>
#include <bits/stdc++.h>

#define TIME_STEP 32
using namespace webots;
using namespace std;

void SensorGroup::initialize(Navigator *follower)
{
    follower = follower;
    init_distance_sensor(follower);
    init_qtr_sensor(follower);
    init_encoders(follower);
    init_camera(follower);
    stabilize_encoder(follower);
    //stabilize_ir_and_distance_sensors(follower);
    init_compass(follower);
}

void SensorGroup::init_distance_sensor(Navigator *follower)
{
    for (int i = 0; i < ds_count; i++)
    {
        ds[i] = follower->getDistanceSensor(dsNames[i]);
        ds[i]->enable(TIME_STEP);
    }
}

void SensorGroup::init_qtr_sensor(Navigator *follower)
{
    for (int i = 0; i < qtr_count; i++)
    {
        qtr[i] = follower->getDistanceSensor(qtrNames[i]);
        qtr[i]->enable(TIME_STEP);
    }
}

void SensorGroup::init_encoders(Navigator *follower)
{
    for (int i = 0; i < encoder_count; i++)
    {
        encoder[i] = follower->getPositionSensor(encoder_name[i]);
        encoder[i]->enable(TIME_STEP);
    }
}

float SensorGroup::get_ir_value(int index)
{
    float val = qtr[index]->getValue();
    return val;
}

void SensorGroup::init_camera(Navigator *follower)
{
    for (int i = 0; i < cs_count; i++)
    {
    camera[i] = follower->getCamera(camera_name[i]);
    camera[i]->enable(TIME_STEP);

    }
}

void SensorGroup::init_compass(Navigator *follower)
{
  compass = follower->getCompass("compass");
  compass->enable(TIME_STEP);
}

const double* SensorGroup::get_compass_value()
{
    return compass->getValues();;
}

// float SensorGroup::get_ir_value_distance_sensors(int index)     //only for stabilization
// {
//     float val = ds[index]->getValue();
//     return val;
// }

float SensorGroup::get_distance_value(int index)
{
    float val = ds[index]->getValue();
    //return round(val * REFLECTION_FACTOR);
    return val;
}

double SensorGroup::get_encoder_val(int index)
{
    double val = encoder[index]->getValue();
    return val;
}

int SensorGroup::get_digital_value(int index)
{
    if (get_ir_value(index) > BLACK_WHITE_THRESHOLD)
        return BLACK;
    else
        return WHITE;
}

// bool SensorGroup::is_junction_detected()
// {
//     if (((get_digital_value(LINE_DETECT_LEFT) == WHITE) and (get_digital_value(QTR_0) == WHITE) and (get_digital_value(LINE_DETECT_RIGHT) == WHITE) 
//     and(get_digital_value(QTR_7) == WHITE)) and((get_digital_value(QTR_3) == BLACK) or (get_digital_value(QTR_4) == BLACK)))
//     {
//         //inverted patch detected
//         cout<<"inverted"<<endl;
//         return true;
//     }
//     else if (((get_digital_value(LINE_DETECT_LEFT) == WHITE) and (get_digital_value(QTR_0) == WHITE) and (get_digital_value(QTR_1) == WHITE) 
//     and (get_digital_value(QTR_2) == WHITE) and (get_digital_value(QTR_3) == WHITE)) or 
//     ((get_digital_value(LINE_DETECT_RIGHT) == WHITE) and (get_digital_value(QTR_4) == WHITE) and (get_digital_value(QTR_5) == WHITE) 
//     and (get_digital_value(QTR_6) == WHITE) and (get_digital_value(QTR_7) == WHITE)))
//     {
//         //normal juction or a white patch
//         //detct the line colors left and right
//         //gor forward specific distance
//         cout<<"second"<<endl;
//         if ((get_digital_value(LINE_DETECT_LEFT) == WHITE) and (get_digital_value(QTR_0) == WHITE) 
//         and (get_digital_value(QTR_1) == WHITE) and (get_digital_value(QTR_2) == WHITE) 
//         and (get_digital_value(QTR_3) == WHITE) and (get_digital_value(LINE_DETECT_RIGHT) == WHITE) 
//         and (get_digital_value(QTR_4) == WHITE) and (get_digital_value(QTR_5) == WHITE) 
//         and (get_digital_value(QTR_6) == WHITE) and (get_digital_value(QTR_7) == WHITE))
//         {
//             //white patch detected
//         }
//         else
//         {
//             //a normal junction
//         }
//         return true;
//     }
//     else
//         return false;
// }

bool SensorGroup::is_deadend()
{
    if ((get_digital_value(QTR_0) == BLACK) and (get_digital_value(QTR_1) == BLACK) and (get_digital_value(QTR_2) == BLACK) 
    and (get_digital_value(QTR_3) == BLACK)and (get_digital_value(QTR_4) == BLACK) and (get_digital_value(QTR_5) == BLACK) 
    and (get_digital_value(QTR_6) == BLACK) and (get_digital_value(QTR_7) == BLACK))
    {
        return true;
    }
    else
        return false;
}

bool SensorGroup::is_line_segment_detected()
{
    if ((get_digital_value(QTR_0) == WHITE) and (get_digital_value(QTR_1) == WHITE) and (get_digital_value(QTR_2) == WHITE) and (get_digital_value(QTR_3) == WHITE) 
    and (get_digital_value(QTR_4) == WHITE) and (get_digital_value(QTR_5) == WHITE) and (get_digital_value(QTR_6) == WHITE) and (get_digital_value(QTR_7) == WHITE))
        return true;
    else
        return false;
}

int SensorGroup::qtr_read_line()
{
    int irValues[8];
    int numerator = 0;
    int denominator = 0;

    for (int i=0; i<8 ; i++){
        irValues[i] = get_digital_value(i);
    }
    for (int i=0; i<8 ; i++){
        numerator += i*1000*irValues[i];
        denominator += irValues[i];
    }
    //cout<<irValues[7]<<"  "<<irValues[6]<<"  "<<irValues[1]<<"  "<<irValues[0]<<endl;
    if ((numerator == 0) and (denominator == 0))    // to prevent 0/0 division form happening
    {
        if (previousQTR_7 == 1)
            return 7000;
        else if (previousQTR_0 == 1)
            return 0;
        else{
            //cout<<"undefined value at qtr read line"<<endl;
            return 3500;
        }
    }

    previousQTR_7 = irValues[QTR_7];
    previousQTR_0 = irValues[QTR_0];

    return (numerator/denominator);
}


// bool SensorGroup::is_pillar_detected(int side)
// {
//     float distance;

//     if (side == LEFT)
//         distance = get_generic_value(TOF_LEFT); 
//     else 
//         distance = get_generic_value(TOF_RIGHT); 
    

//     if (distance < 15 ) 
//         return 1;
//     else
//         return 0;
// }

// bool SensorGroup::is_gate_detected(int sensor)
// {
//     if (get_generic_value(sensor) < GATE_RANGE ) 
//         return 1;
//     else
//         return 0;
// }

void SensorGroup::stabilize_encoder(Navigator *follower)
{
    if (isnan(get_encoder_val(EN_LEFT)) || isnan(get_encoder_val(EN_RIGH)) || isnan(get_encoder_val(2)) || isnan(get_encoder_val(3)))    //2,3 for arm and box servos
    {
        while (follower->step(TIME_STEP) != -1)
        {
            if (!(isnan(get_encoder_val(EN_LEFT)) && isnan(get_encoder_val(EN_RIGH)) && isnan(get_encoder_val(2)) && isnan(get_encoder_val(3))))
                break; //to make sure that encoder dosent return NaN
        }
    }
}

// void SensorGroup::stabilize_ir_and_distance_sensors(Navigator *follower)
// {
//     while (follower->step(TIME_STEP) != -1)
//     {
//         if (!(isnan(get_ir_value_distance_sensors(DS_SENSOR_FRONT)) && isnan(get_ir_value_distance_sensors(DS_SENSOR_LEFT)) && isnan(get_ir_value_distance_sensors(DS_SENSOR_RIGHT)) && isnan(get_ir_value_distance_sensors(DS_SENSOR_BOX)) 
//         && isnan(get_ir_value_distance_sensors(TOF_LEFT)) && isnan(get_ir_value_distance_sensors(TOF_RIGHT)) && isnan(get_ir_value(QTR_0)) && isnan(get_ir_value(QTR_1)) && isnan(get_ir_value(QTR_2))
//         && isnan(get_ir_value(QTR_3)) && isnan(get_ir_value(QTR_4)) && isnan(get_ir_value(QTR_5)) && isnan(get_ir_value(QTR_6)) && isnan(get_ir_value(QTR_7)) && isnan(get_ir_value(LINE_DETECT_LEFT))
//         && isnan(get_ir_value(LINE_DETECT_RIGHT))))
//             break; //to make sure that ir and distance sensors dosent return NaN
//     }
// }

// int SensorGroup::get_colour(int cam)
// {
//     const unsigned char *IMAGE = camera[cam]->getImage();

//     WIDTH = camera[cam]->getWidth();
//     HEIGHT = camera[cam]->getHeight();

//     int redpix = 0;
//     int greenpix = 0;
//     int bluepix = 0;

//     int i, j;
    
//     for (j = CAM_PIXEL_THRESH ; j < (HEIGHT-CAM_PIXEL_THRESH); j++)
//     {
//         for (i = CAM_PIXEL_THRESH; i < (WIDTH-CAM_PIXEL_THRESH); i++)
//         {

//             redpix += camera[cam]->imageGetRed(IMAGE, WIDTH, i, j);
//             bluepix += camera[cam]->imageGetBlue(IMAGE, WIDTH, i, j);
//             greenpix += camera[cam]->imageGetGreen(IMAGE, WIDTH, i, j);

//             if ((redpix > greenpix) && (redpix >  bluepix))
//             {
//                 recentColor = RED;
//                 return RED;
//             }
//             else if ((greenpix >  redpix) && (greenpix >  bluepix))
//             {
//                 recentColor = GREEN;
//                 return GREEN;
//             }
//             else if ((bluepix >  redpix) && (bluepix >  greenpix))
//             {
//                 recentColor = BLUE;
//                 return BLUE;
//             }
//         }
//     }
//     //cout<<"no colour"<<endl;
//     return NO_COLOR;
// }

int SensorGroup::get_colour(int cam)
{
    const unsigned char *IMAGE = camera[cam]->getImage();

    WIDTH = camera[cam]->getWidth();
    HEIGHT = camera[cam]->getHeight();

    int redpix = 0;
    int greenpix = 0;
    int bluepix = 0;

    int i, j;

    int whiteCount = 0;
    int redCount = 0;
    int blueCount = 0;
    int greenCount = 0;

    int mainThresh[4] = {90,180,180,180};
    int lowerThresh[4] = {35,70,70,70};

    for (j = CAM_PIXEL_THRESH ; j < (HEIGHT-CAM_PIXEL_THRESH); j++)
    {
        for (i = CAM_PIXEL_THRESH; i < (WIDTH-CAM_PIXEL_THRESH); i++)
        {
            redpix = camera[cam]->imageGetRed(IMAGE, WIDTH, i, j);
            bluepix = camera[cam]->imageGetBlue(IMAGE, WIDTH, i, j);
            greenpix = camera[cam]->imageGetGreen(IMAGE, WIDTH, i, j);

            // if (cam ==0)
            //     cout<<redpix<<"  "<<greenpix<<"  "<<bluepix<<"  "<<endl;

            if ((redpix > mainThresh[cam]) && (bluepix > mainThresh[cam]) && (greenpix > mainThresh[cam]))
            {
                whiteCount += 1;
            }
            else if ((redpix > mainThresh[cam]) && (bluepix < lowerThresh[cam]) && (greenpix < lowerThresh[cam]))
            {
                redCount += 1;
            }
            else if ((redpix < lowerThresh[cam]) && (bluepix > mainThresh[cam]) && (greenpix < lowerThresh[cam]))
            {
                blueCount += 1;
            }
            else if ((redpix < lowerThresh[cam]) && (bluepix < lowerThresh[cam]) && (greenpix > mainThresh[cam]))
            {
                greenCount += 1;
            }

            if ((whiteCount>20)&&(redCount==0)&&(blueCount==0)&&(greenCount==0)){
                recentColor = WHITE_CLR;
                //cout<<"w"<<endl;
                return WHITE_CLR;
            }
            else if ((whiteCount==0)&&(redCount>20)&&(blueCount==0)&&(greenCount==0)){
                recentColor = RED;
                //cout<<"r"<<endl;
                return RED;
            }
            else if ((whiteCount==0)&&(redCount==0)&&(blueCount>20)&&(greenCount==0)){
                recentColor = BLUE;
                //cout<<"b"<<endl;
                return BLUE;
            }
            else if ((whiteCount==0)&&(redCount==0)&&(blueCount==0)&&(greenCount>20)){
                recentColor = GREEN;
                //cout<<"g"<<endl;
                return GREEN;
            }
        }
    }
    return NO_COLOR;
}