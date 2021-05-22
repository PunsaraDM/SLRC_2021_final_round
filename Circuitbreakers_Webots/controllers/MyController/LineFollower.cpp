#include <webots/Motor.hpp>
#include <webots/Robot.hpp>
#include "LineFollower.h"
#include "SensorPanel.h"
#include "MotorGroup.h"
#include <math.h>
#include <bits/stdc++.h>

#define TIME_STEP 32
#define UNKNOWN 99999.99

#define KP 0.25
#define KI 0.006
#define KD 2

// Size of the yellow line angle filter
#define FILTER_SIZE 3

using namespace webots;
using namespace std;

LineFollower::LineFollower()
{
    sensorPanel = new SensorPanel();
    motorGroup = new MotorGroup();
    sensorPanel->initialize(this);
    motorGroup->initialize(this);
}

////////////////////////////////////////////////////////// delay /////////////////////////////////////////////////////////////////////////

void LineFollower::passive_wait(double targetLeft, double targetRight)
{
    const double DELTA = 0.1;
    double dif;
    double effectiveLeft, effectiveRight;
    do
    {
        if (step(TIME_STEP) == -1)
            exit(EXIT_SUCCESS);

        effectiveLeft = sensorPanel->get_encoder_val(LEFT);
        effectiveRight = sensorPanel->get_encoder_val(RIGHT);
        dif = min(fabs(effectiveLeft - targetLeft), fabs(effectiveRight - targetRight));

    } while (dif > DELTA);
}

////////////////////////////////////////////////////////// follow line methods /////////////////////////////////////////////////////////////////////////

void LineFollower::follow_line_striaght()
{
    leftSpeed = 7.5;
    rightSpeed = 7.5;

    leftIRVal = sensorPanel->get_ir_value(0);
    rightIRVal = sensorPanel->get_ir_value(1);

    if (sensorPanel->get_digital_value(0) == WHITE)
    {
        leftSpeed = 2.5;
        rightSpeed = 3.5;
    }
    else if (sensorPanel->get_digital_value(1) == WHITE)
    {
        leftSpeed = 3.5;
        rightSpeed = 2.5;
    }

    motorGroup->set_velocity(leftSpeed, rightSpeed);
}

void LineFollower::follow_line_initial_phase()
{
    follow_line_striaght();

    int color = sensorPanel->detect_color_patch();

    if (((colorPatch != -1 && color == 4) || sensorPanel->get_encoder_val(0) > 155) && !colorPrinted)
    {
        sensorPanel->print_color_patch();
        colorPrinted = true;
    }

    if (color < 4)
    {
        colorPatch = color;
        if (colorPatch != 1)
        {
            currentInst = 3;
        }
    }

    bool junctionAhead = sensorPanel->is_junction_detected();

    if (junctionAhead)
    {
        if (!colorPrinted)
        {
            sensorPanel->print_color_patch();
            colorPrinted = true;
        }
        update_state();
    }
}

void LineFollower::follow_line_middle_phase()
{
    follow_line_striaght();

    bool wallAhead = sensorPanel->is_wall_entrance();
    bool junctionAhead = sensorPanel->is_junction_detected();

    if (wallAhead || junctionAhead)
    {
        update_state();
        if (junctionAhead)
        {
            go_forward_specific_distance(3.0);
        }
    }
}

void LineFollower::follow_line_end_phase()
{
    follow_line_striaght();

    bool junctionAhead = sensorPanel->is_junction_detected();

    if (junctionAhead || sensorPanel->detect_color_patch() != colorPatch)
    {
        if (junctionAhead)
        {
            go_forward_specific_distance(6);
        }
        else
        {
            go_forward_specific_distance(0.4);
        }
        motorGroup->set_velocity(0, 0);
        update_state();
    }
}

////////////////////////////////////////////////////////// follow line camera /////////////////////////////////////////////////////////////
void LineFollower::follow_line()
{
    leftSpeed = 5;
    rightSpeed = 5;

    const unsigned char *camera_image = NULL;
    camera_image = sensorPanel->get_image();

    double white_line_angle = filter_angle(process_camera_image(camera_image));

    // cout << white_line_angle << "\n";

    if (white_line_angle != UNKNOWN)
    {
        // // wbu_driver_set_brake_intensity(0.0);
        // cout << "left speed: " << leftSpeed + white_line_angle * leftSpeed << "\n";
        // cout << "right speed: " << rightSpeed - white_line_angle * rightSpeed << "\n";

        // motorGroup->set_velocity(leftSpeed + white_line_angle * leftSpeed, rightSpeed + +white_line_angle * rightSpeed);

        if (white_line_angle < 0)
        {
            cout << "Turn left" << "\n";
            leftSpeed = 2.5;
            rightSpeed = 5.5;
        }
        else if (white_line_angle > 0)
        {
            cout << "Turn right" << "\n";
            leftSpeed = 5.5;
            rightSpeed = 2.5;
        }
        

        motorGroup->set_velocity(leftSpeed, rightSpeed);
    }
    else
    {
        // no obstacle has been detected but we lost the line => we brake and hope to find the line again
        // wbu_driver_set_brake_intensity(0.4);
        motorGroup->set_velocity(2.5, 2.5);
        PID_need_reset = true;
    }
}

// returns approximate angle of yellow road line
// or UNKNOWN if no pixel of yellow line visible
double LineFollower::process_camera_image(const unsigned char *image)
{

    int num_pixels = sensorPanel->get_pixels(); // number of pixels in the image
    int sumx = 0;                               // summed x position of pixels
    int pixel_count = 0;

    int x;

    for (x = 0; x < num_pixels; x++)
    {
        if (sensorPanel->color_diff(image, x) < 150)
        {
            sumx += x % sensorPanel->get_width();
            pixel_count++; // count white pixels
        }
    }

    cout << "pixel count: " << pixel_count << "\n";
    // if no pixels was detected...

    if (pixel_count == 0)
        return UNKNOWN;
    double align = ((double)sumx / pixel_count / sensorPanel->get_width());
    cout << "align: " << align << "\n";
    return ((double)sumx / pixel_count / sensorPanel->get_width() - 0.5);
}

// filter angle of the yellow line (simple average)
double LineFollower::filter_angle(double new_value)
{
    static bool first_call = true;
    static double old_value[FILTER_SIZE];
    int i;

    if (first_call || new_value == UNKNOWN)
    { // reset all the old values to 0.0
        first_call = false;
        for (i = 0; i < FILTER_SIZE; ++i)
            old_value[i] = 0.0;
    }
    else
    { // shift old values
        for (i = 0; i < FILTER_SIZE - 1; ++i)
            old_value[i] = old_value[i + 1];
    }

    if (new_value == UNKNOWN)
        return UNKNOWN;
    else
    {
        old_value[FILTER_SIZE - 1] = new_value;
        double sum = 0.0;
        for (i = 0; i < FILTER_SIZE; ++i)
            sum += old_value[i];
        return (double)sum / FILTER_SIZE;
    }
}


////////////////////////////////////////////////////////// turns /////////////////////////////////////////////////////////////

void LineFollower::complete_turn(int dir)
{
    sensorPanel->stabilize_encoder(this);

    int sign = 1;
    if (dir == LEFT)
        sign = -1;

    double initialLeftENcount = sensorPanel->get_encoder_val(LEFT);
    double initialRightENcount = sensorPanel->get_encoder_val(RIGHT);

    double leftCount = initialLeftENcount + (sign * TURN90_EN_COUNT);
    double rightCount = initialRightENcount - (sign * TURN90_EN_COUNT);

    if (dir == BACK)
    {
        leftCount = initialLeftENcount + TURN180_EN_COUNT;
        rightCount = initialRightENcount - TURN180_EN_COUNT;
    }

    motorGroup->set_control_pid(4.5, 0, 0);
    motorGroup->set_velocity(7.5, 7.5);

    motorGroup->set_position(leftCount, rightCount);

    passive_wait(leftCount, rightCount);

    motorGroup->enable_motor_velocity_control();
}

void LineFollower::go_forward_specific_distance(double distance)
{

    sensorPanel->stabilize_encoder(this);

    double initialLeftENcount = sensorPanel->get_encoder_val(LEFT);
    double initialRightENcount = sensorPanel->get_encoder_val(RIGHT);

    motorGroup->set_control_pid(4.5, 0, 0);
    motorGroup->set_velocity(7.5, 7.5);

    motorGroup->set_position(distance + initialLeftENcount, distance + initialRightENcount);
    passive_wait(distance + initialLeftENcount, distance + initialRightENcount);
    motorGroup->enable_motor_velocity_control();
}

////////////////////////////////////////////////////////// wall follow /////////////////////////////////////////////////////////////

void LineFollower::navigate_wall_maze()
{
    bool skip = false;
    if (sensorPanel->is_wall_exit() == true)
    {
        skip = true;
        update_state();
    }
    if (!skip)
    {
        if ((sensorPanel->get_distance_value(DS_SENSOR_FRONT) < frontWallThreshold) == true)
        {
            wallJuncCount++;
            if (wallJuncCount % 2 == 1)
            {
                wallFollowErrorThreshold = WALL_FOLLOW_HORIZONTAL_SIDE_THRESHOLD;
                frontWallThreshold = WALL_FOLLOW_HORIZONTAL_FRONT_THRESHOLD;
            }
            else
            {
                wallFollowErrorThreshold = WALL_FOLLOW_VERTICAL_SIDE_THRESHOLD;
                frontWallThreshold = WALL_FOLLOW_VERTICAL_FRONT_THRESHOLD;
            }

            if (sensorPanel->is_wall(RIGHT) == false)
                complete_turn(RIGHT);
            else
                complete_turn(LEFT);
        }

        follow_both_walls(0.02, 1.0, wallFollowErrorThreshold);
    }
}

void LineFollower::follow_both_walls(float Kp, float Kd, float threshold)
{
    double error = 0;

    if (sensorPanel->is_wall(LEFT))
    {
        error = threshold - round(sensorPanel->get_distance_value(DS_SENSOR_LEFT));
    }
    else if (sensorPanel->is_wall(RIGHT))
    {
        error = round(sensorPanel->get_distance_value(DS_SENSOR_RIGHT)) - threshold;
    }

    double controlValue = (error * Kp) + (error - wallFollowPreviousError) * Kd;

    wallFollowPreviousError = error;

    leftSpeed = MAX_VELOCITY;
    rightSpeed = MAX_VELOCITY;

    rightSpeed = WALL_FOLLOW_BASE_SPEED - controlValue;
    leftSpeed = WALL_FOLLOW_BASE_SPEED + controlValue;

    if (rightSpeed < MIN_VELOCITY)
        rightSpeed = MIN_VELOCITY;
    else if (rightSpeed > MAX_VELOCITY)
        rightSpeed = MAX_VELOCITY;

    if (leftSpeed < MIN_VELOCITY)
        leftSpeed = MIN_VELOCITY;
    else if (leftSpeed > MAX_VELOCITY)
        leftSpeed = MAX_VELOCITY;

    motorGroup->set_velocity(leftSpeed, rightSpeed);
}

////////////////////////////////////////////////////////// final stage methods /////////////////////////////////////////////////////////////////////////

void LineFollower::find_destination()
{
    sensorPanel->detect_color_patches();
    finalPosition = distance(sensorPanel->COLORS, find(sensorPanel->COLORS, sensorPanel->COLORS + 3, colorPatch));

    if (finalPosition % 2 == 0)
    {
        order[8 + finalPosition / 2] = TURN_LEFT;
        order[9 - finalPosition / 2] = TURN_RIGHT;
        order[10] = CAL_FACTORS;
        finalPhase = currentInst + 3;
        ++currentInst;
    }
    else
    {
        order[8] = CAL_FACTORS;
        finalPhase = currentInst;
    }

    update_state();
}

void LineFollower::find_factors(int n)
{
    if (!factorsFound)
    {
        printf("N: %d\n", n);

        vector<int> factors;
        int i = 1;
        while (i * i <= n)
        {
            if (n % i == 0)
            {
                factors.push_back(i);
                if (n / i != i)
                {
                    factors.push_back(n / i);
                }
            }
            i++;
        }
        sort(factors.begin(), factors.end());

        cout << "Factors: " << endl;

        for (auto i = factors.begin(); i != factors.end(); ++i)
        {
            std::cout << *i << endl;
        }
    }
    factorsFound = true;
    exit(0);
}

//////////////////////////////////////////////////////////// main //////////////////////////////////////////////////////////////////////

void LineFollower::update_state()
{
    if (currentInst != 0)
    {
        currentInst++;
        if (currentInst % 2 == 1)
        {
            if (currentInst < 4)
            {
                currentState = LINEFOLLOW_INITIAL;
            }
            else if (currentInst > finalPhase)
            {
                currentState = LINEFOLLOW_END;
            }
            else
            {
                currentState = LINEFOLLOW;
            }
        }
        else
        {
            currentState = order[currentInst / 2 - 1];
            if (currentState == TURN_RIGHT)
                sensorPanel->enable_wall_follow();
        }
    }
    else
    {
        currentInst = 1;
        currentState = LINEFOLLOW_INITIAL;
    }
}

//maze following
void LineFollower::travel_maze()
{
    switch (currentState)
    {
    case LINEFOLLOW:
        follow_line_middle_phase();
        break;

    case INITIALIZE:
        go_forward_specific_distance(3);
        update_state();
        break;

    case LINEFOLLOW_END:
        follow_line_end_phase();
        break;

    case LINEFOLLOW_INITIAL:
        follow_line_initial_phase();
        break;

    case SQUARE:
        go_forward_specific_distance(8.6);
        update_state();
        break;

    case WALLFOLLOW:
        navigate_wall_maze();
        break;

    case TURN_RIGHT:
        complete_turn(RIGHT);
        update_state();
        break;

    case TURN_LEFT:
        complete_turn(LEFT);
        update_state();
        break;

    case FIND_DESTINATION:
        find_destination();
        break;

    case CAL_FACTORS:
        find_factors((finalPosition + 1) * 4);
        break;

    default:
        // motorGroup->set_velocity("Default case");
        break;
    }
}

void LineFollower::test_camera()
{
    follow_line();
}