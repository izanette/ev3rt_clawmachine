// motor.cpp

#include "motor.h"
#include "utils.h"

#define LIMIT_EPSILON 10


Motor::Motor(motor_port_t p, motor_type_t t)
: port(p),
  state(MOTOR_STOP),
  current_speed(0.0)
{
    ev3_motor_config(port, t);
    init_default_values();
}

void Motor::forward(bool check_limits /* = true*/)
{
    if (state == MOTOR_FWD)
    {
        update(check_limits);
        return;
    }
    
    state = MOTOR_FWD;
    set_speed(speed_min, check_limits);
    motor_start = getTimeMillis();
}

void Motor::backward(bool check_limits /* = true*/)
{
    if (state == MOTOR_BCK)
    {
        update(check_limits);
        return;
    }
    
    state = MOTOR_BCK;
    set_speed(speed_min, check_limits);
    motor_start = getTimeMillis();
}

void Motor::stop(bool brake /* = true*/)
{
    if (state == MOTOR_STOP)
    {
        return;
    }
    
    current_speed = 0;
    ev3_motor_stop(port, brake);
    state = MOTOR_STOP;
}

void Motor::update(bool check_limits /* = true*/)
{
    switch(state)
    {
    case MOTOR_FWD:
    case MOTOR_BCK:
        {
            float delta = getTimeMillis() - motor_start - zero_acc_time;
            if (delta > 0)
            {
                float speed = speed_min + delta * acc / 1000;
                set_speed(speed, check_limits);
            }
            else
            {
                set_speed(speed_min, check_limits);
            }
        }
        break;
        
    default:
        return;
    }
}

void Motor::reset_pos()
{
    ev3_motor_reset_counts(port);
}

int32_t  Motor::get_pos()
{
    return home_pos + direction*ev3_motor_get_counts(port);
}

void Motor::find_home(int power, int hold_time)
{
    raw_motor(power);
    tslp_tsk(hold_time);
    
    int32_t last_pos = ev3_motor_get_counts(port);
    float time_last_pos = getTimeMillis();
    
    while(true)
    {
        int32_t pos = ev3_motor_get_counts(port);
        
        if (pos != last_pos)
        {
            time_last_pos = getTimeMillis();
            last_pos = pos;
        }
        else
        {
            if (getTimeMillis() - time_last_pos > hold_time)
            {
                raw_motor(0);
                return;
            }
        }
    }
}

void Motor::init_default_values()
{
    direction = 1;
    limit = 1000;
    home_pos = 0;
    zero_acc_time = 1000;
    acc = 10;
    speed_min = 10;
    speed_max = 100;
}

int int_power(float power)
{
    if (power >  100.0) return  100;
    if (power < -100.0) return -100;
    
    
    return (int) power;
}

// speed should be always greater than 0
void Motor::set_speed(float speed, bool check_limits /* = true*/)
{
    current_speed = MINVAL(speed, speed_max);
    int power;
    
    switch(state)
    {
    case MOTOR_FWD:
        if (check_limits && (get_pos() > limit-LIMIT_EPSILON))
        {
            stop();
            break;
        }
        power = int_power(current_speed*direction);
        raw_motor(power);
        break;
        
    case MOTOR_BCK:
        if (check_limits && (get_pos() < LIMIT_EPSILON))
        {
            stop();
            break;
        }
        power = -int_power(current_speed*direction);
        raw_motor(power);
        break;
        
    default:
        break;
    }
}

void Motor::raw_motor(int speed)
{
    if (speed)
        ev3_motor_set_power(port, speed);
    else
        ev3_motor_stop(port, false);
}
