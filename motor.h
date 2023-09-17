// motor.h
#pragma once

#include "ev3api.h"

class Motor
{
public: // attributes
    int direction;
    int32_t limit;
    int32_t home_pos;
    int zero_acc_time;
    float acc;
    int speed_min;
    int speed_max;

public: // methods
    Motor(motor_port_t p, motor_type_t t);
    void forward(bool check_limits = true);
    void backward(bool check_limits = true);
    void stop(bool brake = true);
    void update(bool check_limits = true);
    int32_t get_pos();
    void reset_pos();
    void raw_motor(int speed);
    void find_home(int power, int hold_time);

private: // types

    typedef enum {
        MOTOR_FWD,
        MOTOR_BCK,
        MOTOR_STOP
    } motor_state_t;

private: // attributes
    motor_port_t port;
    motor_state_t state;
    float current_speed;
    float motor_start;

private: //methods
    void init_default_values();
    void set_speed(float speed, bool check_limits = true);
};
