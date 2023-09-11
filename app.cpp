/**
 * This sample program runs motors A, B, C & D using a IR Remote control on channels 1 & 2.
 *
 */

#include "ev3api.h"
#include "app.h"
#include "utils.h"
#include <stdlib.h>

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

#define MINVAL(X, Y)  ((X) < (Y) ? (X) : (Y))
#define MAXVAL(X, Y)  ((X) > (Y) ? (X) : (Y))

const motor_port_t x_motor   = EV3_PORT_A;
const motor_port_t y_motor   = EV3_PORT_D;
const motor_port_t z_motor   = EV3_PORT_B;
const motor_port_t c_motor   = EV3_PORT_C;
const sensor_port_t ir_sensor = EV3_PORT_1;
const sensor_port_t color_sensor = EV3_PORT_4;

void motor(motor_port_t m, int power)
{
    if (power)
        ev3_motor_set_power(m, power);
    else
        ev3_motor_stop(m, false);
}

void wait_no_button_pressed()
{
    while(1)
    {
        // wait 10 mili-seconds
        tslp_tsk(10);
        
        int stop = 1;
        for(int i = 0; i < 6; i++)
        {
            if (ev3_button_is_pressed((button_t)i))
                stop = 0;
        }
        if (stop) break;
    }
}

void control()
{
    while (1)
    {
        ir_remote_t val = ev3_infrared_sensor_get_remote(ir_sensor);
        for(int c = 0; c < 2; c++)
        {
            int redup    = (val.channel[c] & IR_RED_UP_BUTTON);
            int reddown  = (val.channel[c] & IR_RED_DOWN_BUTTON);
            int blueup   = (val.channel[c] & IR_BLUE_UP_BUTTON);
            int bluedown = (val.channel[c] & IR_BLUE_DOWN_BUTTON);
            
            if (c == 0) // x and y motors
            {
                // x motor
                if (redup)
                {
                    motor(x_motor, 10);
                }
                else if (reddown)
                {
                    motor(x_motor, -10);
                }
                else
                {
                    motor(x_motor, 0);
                }
                
                // y motor
                if (blueup)
                {
                    motor(y_motor, 10);
                }
                else if (bluedown)
                {
                    motor(y_motor, -10);
                }
                else
                {
                    motor(y_motor, 0);
                }
            }
            else // z and c motors
            {
                // z motor
                if (redup)
                {
                    motor(z_motor, -10);
                }
                else if (reddown)
                {
                    motor(z_motor, 10);
                }
                else
                {
                    motor(z_motor, 0);
                }
                
                // c motor
                if (blueup)
                {
                    motor(c_motor, -10);
                }
                else if (bluedown)
                {
                    motor(c_motor, 10);
                }
                else
                {
                    motor(c_motor, 0);
                }
            }
        }
        // wait 10 mili-seconds
        tslp_tsk(10);
    }
}

void home_x()
{
    uint8_t val = ev3_color_sensor_get_reflect(color_sensor);
    while(val < 50)
    {
        motor(x_motor, 10);
        tslp_tsk(10);
        val = ev3_color_sensor_get_reflect(color_sensor);
    }
    motor(x_motor, 0);
}

void home()
{
    home_x();
}

void main_task(intptr_t unused)
{
    char buf[100];
    
    print(0, "App: Claw Machine v01");
    sprintf(buf, "Port%c:X motor", 'A' + x_motor);
    print(1, buf);
    sprintf(buf, "Port%c:Y motor", 'A' + y_motor);
    print(2, buf);
    sprintf(buf, "Port%c:Z motor", 'A' + z_motor);
    print(3, buf);
    sprintf(buf, "Port%c:Claw motor", 'A' + c_motor);
    print(4, buf);
    sprintf(buf, "Port%c:IR sensor",  '1' + ir_sensor);
    print(6, buf);
    sprintf(buf, "Port%c:Color sensor",  '1' + color_sensor);
    print(7, buf);

    // Configure motors
    ev3_motor_config(x_motor, LARGE_MOTOR);
    ev3_motor_config(y_motor, LARGE_MOTOR);
    ev3_motor_config(z_motor, LARGE_MOTOR);
    ev3_motor_config(c_motor, MEDIUM_MOTOR);
    
    // Configure sensors
    ev3_sensor_config(ir_sensor, INFRARED_SENSOR);
    ev3_sensor_config(color_sensor, COLOR_SENSOR);

    home();
    
    control();
}
