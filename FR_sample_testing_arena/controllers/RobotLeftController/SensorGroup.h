#include <webots/DistanceSensor.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Camera.hpp>
#include <webots/Compass.hpp>
#include <webots/Receiver.hpp>
#include <webots/Emitter.hpp>
#include <string>

using namespace webots;
using namespace std;

#ifndef SENSORGROUP_H
#define SENSORGROUP_H

class Navigator;
class SensorGroup
{
public:
    void initialize(Navigator *follower);
    void init_distance_sensor(Navigator *follower);
    void init_qtr_sensor(Navigator *follower);
    void init_encoders(Navigator *follower);
    void init_camera(Navigator *follower);
    void init_compass(Navigator *follower);

    const double *get_compass_value();
    float get_ir_value(int index);
    //float get_ir_value_distance_sensors(int index);
    float get_distance_value(int index);
    double get_encoder_val(int index);
    int get_digital_value(int index);
    void stabilize_encoder(Navigator *follower);
    void init_receiver(Navigator *follower);
    void init_emmiter(Navigator *follower);

    //void stabilize_ir_and_distance_sensors(Navigator *follower);

    int get_colour(int cam);

    bool receiver_ready();
    string receive();
    void emmit(string TxMessage);

    //bool is_junction_detected();
    bool is_line_segment_detected();
    bool is_deadend();

    //bool is_pillar_detected(int side);
    //bool is_gate_detected(int sensor);

    int qtr_read_line();

    int COLORS[3];
    int nextTurn = -1;

private:
    Navigator *follower;
    static const int ds_count = 1;
    DistanceSensor *ds[ds_count];
    char dsNames[ds_count][8] = {"ds_arm"};

    static const int qtr_count = 10;
    DistanceSensor *qtr[qtr_count];
    char qtrNames[qtr_count][18] = {"qtr_0", "qtr_1", "qtr_2", "qtr_3", "qtr_4", "qtr_5", "qtr_6", "qtr_7", "ld_left", "ld_right"};

    static const int encoder_count = 6;
    PositionSensor *encoder[encoder_count];
    char encoder_name[encoder_count][24] = {"encoder_left", "encoder_right", "base_motor_pos", "vertical_motor_pos", "gripper_left_motor_pos", "gripper_right_motor_pos"};

    static const int cs_count = 4;
    Camera *camera[cs_count];
    char camera_name[cs_count][10] = {"cs_arm", "cs_left", "cs_right", "cs_front"};

    Compass *compass;
    char compass_name[8] = "compass";

    Receiver *receiver;
    char rx_name[10] = "receiver";

    Emitter *emmiter;
    char tx_name[10] = "emitter";

    int BLACK = 0;
    int WHITE = 1;

    //int LEFT = 0;
    //int RIGHT = 1;
    int BACK = 2;
    int FRONT = 4;

    int QTR_0 = 0;
    int QTR_1 = 1;
    int QTR_2 = 2;
    int QTR_3 = 3;
    int QTR_4 = 4;
    int QTR_5 = 5;
    int QTR_6 = 6;
    int QTR_7 = 7;
    int LINE_DETECT_LEFT = 8;
    int LINE_DETECT_RIGHT = 9;

    int previousQTR_7 = 0;
    int previousQTR_0 = 0;

    // int DS_SENSOR_BOX = 3;
    // int DS_SENSOR_FRONT = 2;
    // int DS_SENSOR_RIGHT = 1;
    // int DS_SENSOR_LEFT = 0;
    // int TOF_RIGHT = 5;
    // int TOF_LEFT = 4;

    // int CAM_ARM = 0;
    // int CAM_BACK = 1;
    // int CAM_FRONT = 2;
    int CAM_PIXEL_THRESH = 20;

    int WIDTH, HEIGHT;
    int RED = 1;
    int GREEN = 2;
    int BLUE = 3;
    int WHITE_CLR = 4;
    int NO_COLOR = 5;
    int recentColor = -1;

    float IR_BLACK_VALUE = 10;
    float IR_WHITE_VALUE = 5.14;
    float BLACK_WHITE_THRESHOLD = (IR_BLACK_VALUE + IR_WHITE_VALUE) / 2;

    float RED_LEVEL = 0.666667;
    float ROUGHNESS = 0.9;
    float OCCLUSION = 0;
    float REFLECTION_FACTOR = 0.2 + 0.8 * RED_LEVEL * (1 - 0.5 * ROUGHNESS) * (1 - 0.5 * OCCLUSION);

    int FAR_RANGE = 145;
    int NEAR_RANGE = 65;
    int GATE_RANGE = 300;
};

#endif