// cli_main.cpp

#include "ev3api.h"
#include "utils.h"
#include "motor.h"
#include "cli_menu.h"
#include "cli_main.h"
#include "parameters.h"
#include <stdlib.h>

Motor* x_motor;
Motor* y_motor;
Motor* z_motor;
Motor* c_motor;

static const CliMenuEntry entry_tab[] = {
    { .key = '1', .title = "Control",    .handler = control },
    { .key = '2', .title = "Homing",     .handler = homing },
    //{ .key = '3', .title = "Shapes",     .handler = shapes },
    //{ .key = '4', .title = "Text",       .handler = drawText },
    //{ .key = '5', .title = "Print File", .handler = drawFromFile },
    //{ .key = '6', .title = "Tests",      .handler = tests },
};

const CliMenu climenu_main = {
    .title     = APP_NAME,
    .entry_tab = entry_tab,
    .entry_num = sizeof(entry_tab) / sizeof(CliMenuEntry),
};

void control(intptr_t unused)
{
    char buf[100];
    
    clearScreen();
    print(0, APP_NAME);
    
    while (1)
    {
        if (ev3_button_is_pressed(BACK_BUTTON))
        {
            return;
        }
        
        ir_remote_t val = ev3_infrared_sensor_get_remote(IR_SENSOR_PORT);
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
    uint8_t val = ev3_color_sensor_get_reflect(COLOR_SENSOR_PORT);
    float prev_acc = x_motor->acc;
    x_motor->acc = 0.0;
    while(val < 25)
    {
        x_motor->backward(false);
        tslp_tsk(10);
        val = ev3_color_sensor_get_reflect(COLOR_SENSOR_PORT);
    }
    motor(X_MOTOR_PORT, 0);
    x_motor->stop();
    x_motor->acc = prev_acc;
    x_motor->reset_pos();
}

void home_y()
{
    colorid_t val = ev3_color_sensor_get_color(COLOR_SENSOR_PORT);
    float prev_acc = y_motor->acc;
    y_motor->acc = 0.0;
    while(val != COLOR_BLUE && val != COLOR_GREEN)
    {
        y_motor->backward(false);
        tslp_tsk(10);
        val = ev3_color_sensor_get_color(COLOR_SENSOR_PORT);
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
    colorid_t val = ev3_color_sensor_get_color(COLOR_SENSOR_PORT);
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
        val = ev3_color_sensor_get_color(COLOR_SENSOR_PORT);
    }
    z_motor->stop();

    // find the limit between the blue and the green bricks
    while(val != COLOR_GREEN)
    {
        z_motor->forward(false); // go down
        tslp_tsk(10);
        val = ev3_color_sensor_get_color(COLOR_SENSOR_PORT);
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

void homing(intptr_t unused)
{
    
    clearScreen();
    print(0, APP_NAME);
    print(1, "Homing X");
    home_x();
    print(2, "Homing Y");
    home_y();
    print(3, "Homing Z");
    home_z();
    print(4, "Homing C");
    home_c();
    print(5, "Finished homing");
    ev3_color_sensor_get_reflect(COLOR_SENSOR_PORT); // this mode uses less energy
}

