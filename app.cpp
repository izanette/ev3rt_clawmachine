/**
 * This sample program runs motors A, B, C & D using a IR Remote control on channels 1 & 2.
 *
 */

#include "ev3api.h"
#include "app.h"
#include "utils.h"
#include "motor.h"
#include <stdlib.h>

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

const motor_port_t x_motor_port   = EV3_PORT_A;
const motor_port_t y_motor_port   = EV3_PORT_D;
const motor_port_t z_motor_port   = EV3_PORT_C;
const motor_port_t c_motor_port   = EV3_PORT_B;

const sensor_port_t ir_sensor = EV3_PORT_1;
const sensor_port_t color_sensor = EV3_PORT_4;

Motor* x_motor;
Motor* y_motor;
Motor* z_motor;
Motor* c_motor;


void control()
{
    char buf[100];
    
    clearScreen();
    print(0, "Claw Machine v01");
    
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
                    x_motor->backward();
                }
                else if (reddown)
                {
                    x_motor->forward();
                }
                else
                {
                    x_motor->stop();
                }
                
                // y motor
                if (blueup)
                {
                    y_motor->backward();
                }
                else if (bluedown)
                {
                    y_motor->forward();
                }
                else
                {
                    y_motor->stop();
                }
            }
            else // z and claw motors
            {
                // z motor
                if (redup)
                {
                    z_motor->backward();
                }
                else if (reddown)
                {
                    z_motor->forward();
                }
                else
                {
                    z_motor->stop();
                }
                
                // claw motor
                if (blueup)
                {
                    c_motor->forward();
                }
                else if (bluedown)
                {
                    c_motor->backward();
                }
                else
                {
                    c_motor->stop();
                }
            }
        }
        // wait 10 mili-seconds
        tslp_tsk(10);
        
        sprintf(buf, "X pos: %ld", x_motor->get_pos());
        print(1, buf);
        sprintf(buf, "Y pos: %ld", y_motor->get_pos());
        print(2, buf);
        sprintf(buf, "Z pos: %ld", z_motor->get_pos());
        print(3, buf);
        sprintf(buf, "C pos: %ld", c_motor->get_pos());
        print(4, buf);
    }
}

void home_x()
{
    uint8_t val = ev3_color_sensor_get_reflect(color_sensor);
    float prev_acc = x_motor->acc;
    x_motor->acc = 0.0;
    while(val < 25)
    {
        x_motor->backward(false);
        tslp_tsk(10);
        val = ev3_color_sensor_get_reflect(color_sensor);
    }
    motor(x_motor_port, 0);
    x_motor->stop();
    x_motor->acc = prev_acc;
    x_motor->reset_pos();
}

void home_y()
{
    colorid_t val = ev3_color_sensor_get_color(color_sensor);
    float prev_acc = y_motor->acc;
    y_motor->acc = 0.0;
    while(val != COLOR_BLUE && val != COLOR_GREEN)
    {
        y_motor->backward(false);
        tslp_tsk(10);
        val = ev3_color_sensor_get_color(color_sensor);
    }
    
    // run a little bit more so it points towards 
    // the center of the blue/green brick
    tslp_tsk(500); // todo: substitute by a count movement instead of a timed one
    y_motor->stop();
    y_motor->acc = prev_acc;
    y_motor->reset_pos();
}

void home_z()
{
    // assume home_y was done before. so either the color sensor is sensing BLUE or GREEN
    colorid_t val = ev3_color_sensor_get_color(color_sensor);
    float prev_acc = z_motor->acc;
    z_motor->acc = 0.0;
    if (val != COLOR_BLUE && val != COLOR_GREEN)
    {
        //error
        return;
    }
    
    while(val == COLOR_GREEN)
    {
        z_motor->backward(false); // go up
        tslp_tsk(10);
        val = ev3_color_sensor_get_color(color_sensor);
    }
    z_motor->stop();

    // find the limit between the blue and the green bricks
    while(val != COLOR_GREEN)
    {
        z_motor->forward(false); // go down
        tslp_tsk(10);
        val = ev3_color_sensor_get_color(color_sensor);
    }
    z_motor->stop();
    z_motor->acc = prev_acc;
    z_motor->reset_pos();

}

void home_c()
{
    c_motor->find_home(-7, 80);
    c_motor->reset_pos();
}

void home()
{
    
    clearScreen();
    print(0, "Claw Machine v01");
    print(1, "Homing X");
    home_x();
    print(2, "Homing Y");
    home_y();
    print(3, "Homing Z");
    home_z();
    print(4, "Homing C");
    home_c();
    print(5, "Finished homing");
    ev3_color_sensor_get_reflect(color_sensor); // this mode uses less energy
}

void main_task(intptr_t unused)
{
    char buf[100];
    
    print(0, "Claw Machine v01");
    sprintf(buf, "Port%c:X motor", 'A' + x_motor_port);
    print(1, buf);
    sprintf(buf, "Port%c:Y motor", 'A' + y_motor_port);
    print(2, buf);
    sprintf(buf, "Port%c:Z motor", 'A' + z_motor_port);
    print(3, buf);
    sprintf(buf, "Port%c:Claw motor", 'A' + c_motor_port);
    print(4, buf);
    sprintf(buf, "Port%c:IR sensor",  '1' + ir_sensor);
    print(6, buf);
    sprintf(buf, "Port%c:Color sensor",  '1' + color_sensor);
    print(7, buf);

    // Configure motors
    x_motor = new Motor(x_motor_port, LARGE_MOTOR);
    y_motor = new Motor(y_motor_port, LARGE_MOTOR);
    z_motor = new Motor(z_motor_port, LARGE_MOTOR);
    c_motor = new Motor(c_motor_port, MEDIUM_MOTOR);
    
    x_motor->direction = -1;
    y_motor->direction = -1;
    z_motor->direction =  1;
    c_motor->direction = -1;
    
    x_motor->limit = 1400;
    y_motor->limit = 2000;
    z_motor->limit = 1850;
    c_motor->limit = 2100;
    
    z_motor->home_pos = 1850;
    c_motor->home_pos = 2100;
    
    // Configure sensors
    ev3_sensor_config(ir_sensor, INFRARED_SENSOR);
    ev3_sensor_config(color_sensor, COLOR_SENSOR);
    
    waitEnterButtonPressed();
    
    home();
    
    control();
}
