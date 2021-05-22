#include <webots/DistanceSensor.hpp>
#include <webots/PositionSensor.hpp>
#include <webots/Camera.hpp>
#include "SensorPanel.h"
#include "LineFollower.h"
#include <math.h>
#include <bits/stdc++.h>

#define TIME_STEP 32
using namespace webots;
using namespace std;

void SensorPanel::initialize(LineFollower *follower)
{
    follower = follower;
    init_distance_sensor(follower);
    init_encoders(follower);
    init_camera(follower);
    stabilize_ir_and_distance_sensors(follower);
}

void SensorPanel::init_distance_sensor(LineFollower *follower)
{
    for (int i = 0; i < 7; i++)
    {
        ds[i] = follower->getDistanceSensor(dsNames[i]);
        ds[i]->enable(TIME_STEP);
    }
}

void SensorPanel::init_encoders(LineFollower *follower)
{
    for (int i = 0; i < 2; i++)
    {
        encoder[i] = follower->getPositionSensor(encoder_name[i]);
        encoder[i]->enable(TIME_STEP);
    }
}

float SensorPanel::get_ir_value(int index)
{
    float val = ds[index]->getValue();
    return val;
}

void SensorPanel::init_camera(LineFollower *follower)
{
    camera = follower->getCamera(camera_name);
    camera->enable(TIME_STEP);
    WIDTH = camera->getWidth();
    HEIGHT = camera->getHeight();
}

float SensorPanel::get_distance_value(int index)
{
    float val = ds[index]->getValue();
    return val * REFLECTION_FACTOR;
}

double SensorPanel::get_encoder_val(int index)
{
    double val = encoder[index]->getValue();
    return val;
}

int SensorPanel::get_digital_value(int index)
{
    if (get_ir_value(index) > BLACK_WHITE_THRESHOLD)
        return BLACK;
    else
        return WHITE;
}

bool SensorPanel::is_junction_detected()
{
    if ((get_digital_value(IR_LEFT_1) == WHITE) or (get_digital_value(IR_RIGHT_1) == WHITE))
        return true;
    else
        return false;
}

void SensorPanel::enable_wall_follow()
{
    enableWallFollow = true;
}
bool SensorPanel::is_wall(int side)
{
    if ((side == RIGHT) and (get_distance_value(DS_SENSOR_RIGHT) < SIDE_WALL_THRESHOLD))
        return true && enableWallFollow;
    else if ((side == LEFT) and (get_distance_value(DS_SENSOR_LEFT) < SIDE_WALL_THRESHOLD))
        return true && enableWallFollow;
    else if ((side == FRONT) and (get_distance_value(DS_SENSOR_FRONT) < FRONT_WALL_THRESHOLD))
        return true && enableWallFollow;
    else
        return false && enableWallFollow;
}

bool SensorPanel::is_wall_entrance()
{
    if (is_wall(LEFT) == true or is_wall(RIGHT) == true)
        return true;
    else
        return false;
}

bool SensorPanel::is_wall_exit()
{
    if (is_wall(LEFT) == false and is_wall(RIGHT) == false)
        return true;
    else
        return false;
}

void SensorPanel::stabilize_encoder(LineFollower *follower)
{
    if (isnan(get_encoder_val(LEFT)) || isnan(get_encoder_val(RIGHT)))
    {
        while (follower->step(TIME_STEP) != -1)
        {
            if (!(isnan(get_encoder_val(LEFT)) && isnan(get_encoder_val(RIGHT))))
                break; //to make sure that encoder dosent return NaN
        }
    }
}

void SensorPanel::stabilize_ir_and_distance_sensors(LineFollower *follower)
{
    while (follower->step(TIME_STEP) != -1)
    {
        if (!(isnan(get_ir_value(IR_LEFT_0)) && isnan(get_ir_value(IR_RIGHT_0)) && isnan(get_ir_value(IR_LEFT_1)) && isnan(get_ir_value(IR_RIGHT_1)) && isnan(get_ir_value(DS_SENSOR_FRONT)) && isnan(get_ir_value(DS_SENSOR_LEFT)) && isnan(get_ir_value(DS_SENSOR_RIGHT))))
            break; //to make sure that ir and distance sensors dosent return NaN
    }
}

//follow line using camera
const unsigned char *SensorPanel::get_image()
{
    return camera->getImage();
}

int SensorPanel::get_pixels()
{
    return camera->getWidth() * camera->getHeight();
}

int SensorPanel::get_width()
{
    return camera->getWidth();
}

int SensorPanel::get_height()
{
    return camera->getHeight();
}

double SensorPanel::get_fov()
{
    return camera->getFov();
}

// compute rgb difference
int SensorPanel::color_diff(const unsigned char *image, int x)
{
    int i, diff = 0;
    int position_x = x % get_width();
    int position_y = x % get_height();
    int found_colors[3];
    found_colors[0] = camera->imageGetRed(image, WIDTH, position_x, position_y);
    found_colors[1] = camera->imageGetBlue(image, WIDTH, position_x, position_y);
    found_colors[2] = camera->imageGetGreen(image, WIDTH, position_x, position_y);

    for (i = 0; i < 3; i++)
    {
        // cout << "a: " << a[i] << "\n";
        int d = found_colors[i] - 255;
        diff += d > 0 ? d : -d;
    }
    // cout << "red: " << found_colors[0] << "\n";
    // cout << "blue: " << found_colors[1] << "\n";
    // cout << "green: " << found_colors[2] << "\n";

    return diff;
}

int SensorPanel::detect_color_patch()
{
    const unsigned char *IMAGE = camera->getImage();

    int redpix = 0;
    int greenpix = 0;
    int bluepix = 0;

    int i, j;

    for (j = HEIGHT - 1; j >= 0; j--)
    {
        for (i = WIDTH / 4; i < 3 * WIDTH / 4; i++)
        {

            redpix = camera->imageGetRed(IMAGE, WIDTH, i, j);
            bluepix = camera->imageGetBlue(IMAGE, WIDTH, i, j);
            greenpix = camera->imageGetGreen(IMAGE, WIDTH, i, j);

            if ((redpix > 4 * greenpix) && (redpix > 4 * bluepix))
            {
                recentColor = RED;
                return RED;
            }
            else if ((greenpix > 4 * redpix) && (greenpix > 4 * bluepix))
            {
                recentColor = GREEN;
                return GREEN;
            }
            else if ((bluepix > 4 * redpix) && (bluepix > 4 * greenpix))
            {
                recentColor = BLUE;
                return BLUE;
            }
        }
    }
    return NO_PATCH;
}

void SensorPanel::detect_color_patches()
{
    const unsigned char *IMAGE = camera->getImage();

    int redpix = 0;
    int greenpix = 0;
    int bluepix = 0;

    int i, j;
    int ind = 0;
    bool red_detected = false;
    bool green_detected = false;
    bool blue_detected = false;

    for (i = 0; i < WIDTH; i++)
    {
        for (j = 0; j < HEIGHT; j++)
        {
            redpix = camera->imageGetRed(IMAGE, WIDTH, i, j);
            bluepix = camera->imageGetBlue(IMAGE, WIDTH, i, j);
            greenpix = camera->imageGetGreen(IMAGE, WIDTH, i, j);

            if (!red_detected && (redpix > 4 * greenpix) && (redpix > 4 * bluepix))
            {
                COLORS[ind] = RED;
                red_detected = true;
                ind += 1;
            }
            else if (!green_detected && (greenpix > 4 * redpix) && (greenpix > 4 * bluepix))
            {
                COLORS[ind] = GREEN;
                green_detected = true;
                ind += 1;
            }
            else if (!blue_detected && (bluepix > 4 * redpix) && (bluepix > 4 * greenpix))
            {
                COLORS[ind] = BLUE;
                blue_detected = true;
                ind += 1;
            }
        }
    }
    return;
}

void SensorPanel::print_color_patch()
{
    if (recentColor == RED)
    {
        cout << "RED" << endl;
    }
    else if (recentColor == GREEN)
    {
        cout << "GREEN" << endl;
    }
    else if (recentColor == BLUE)
    {
        cout << "BLUE" << endl;
    }
}

void SensorPanel::detect_white_line()
{
    const unsigned char *IMAGE = camera->getImage();

    int i;
    int j = HEIGHT / 2;
    int pix;
    bool line_detected = false;

    for (i = 0; i < WIDTH; i++)
    {
        pix = camera->imageGetGray(IMAGE, WIDTH, i, j);

        if ((pix > 128) and !line_detected)
        {
            START = i;
            cout << "start: " << i;
            line_detected = true;
        }

        if ((pix < 128) and line_detected)
        {
            END = i - 1;
            cout << ", end: " << i - 1 << endl;
            return;
        }
    }
    cout << "line not detected" << endl;
    return;
}

float SensorPanel::calculate_error()
{
    float error = (START + END) / 2 - (WIDTH / 2 - 1);
    return error;
}