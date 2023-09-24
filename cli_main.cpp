// cli_main.cpp

#include "ev3api.h"
#include "utils.h"
#include "motor.h"
#include "cli_menu.h"
#include "cli_main.h"
#include "seconds_counter.h"
#include "parameters.h"
#include <stdlib.h>

Motor* x_motor;
Motor* y_motor;
Motor* z_motor;
Motor* c_motor;

static const CliMenuEntry entry_tab[] = {
    { .key = '1', .title = "Control",    .handler = control },
    { .key = '2', .title = "Homing",     .handler = homing },
    { .key = '3', .title = "Claw",       .handler = claw_game },
};

const CliMenu climenu_main = {
    .title     = APP_NAME,
    .entry_tab = entry_tab,
    .entry_num = sizeof(entry_tab) / sizeof(CliMenuEntry),
};

void control(intptr_t unused)
{
    char buf[100];
    
    ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
    draw_title("Control", 0, 0, MENU_FONT);
    
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
        print(2, buf);
        sprintf(buf, "Y pos: %ld", y_motor->get_pos());
        print(3, buf);
        sprintf(buf, "Z pos: %ld", z_motor->get_pos());
        print(4, buf);
        sprintf(buf, "C pos: %ld", c_motor->get_pos());
        print(5, buf);
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
    c_motor->find_home(7, 80);
    c_motor->reset_pos();
}

void homing(intptr_t unused)
{
    ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
    draw_title("Homing", 0, 0, MENU_FONT);
    
    clearScreen();
    print(2, "Homing X");
    home_x();
    print(3, "Homing Y");
    home_y();
    print(4, "Homing Z");
    home_z();
    print(5, "Homing C");
    home_c();
    print(6, "Finished homing");
    ev3_color_sensor_get_ambient(COLOR_SENSOR_PORT); // this mode uses less energy
    
    tslp_tsk(1000);
}

void open_claw()
{
    while(c_motor->get_pos() > 300)
    {
        c_motor->backward();
        tslp_tsk(10);
    }
    c_motor->stop();
}

void close_claw()
{
    while(c_motor->get_pos() < (c_motor->limit-200))
    {
        c_motor->forward();
        tslp_tsk(10);
    }
    c_motor->stop();
}

bool equal_pos(int32_t p1, int32_t p2, int32_t epsilon)
{
    return abs(p2-p1) <= epsilon;
}

bool move_to(Motor* motor, int32_t pos)
{
    const int32_t epsilon = 10;
    if (pos == -1 || equal_pos(pos, motor->get_pos(), epsilon))
    {
        motor->stop();
        return false;
    }
    
    if (motor->get_pos() < pos)
    {
        motor->forward();
    }
    else
    {
        motor->backward();
    }
    
    return true;
}

void go_to(int32_t x, int32_t y, int32_t z)
{
    bool loop = true;
    while(loop)
    {
        loop = false;
        if (move_to(x_motor, x)) loop = true;
        if (move_to(y_motor, y)) loop = true;
        if (move_to(z_motor, z)) loop = true;
        tslp_tsk(10);
    }
}

void claw_game(intptr_t unused)
{
    //char buf[100];
    bool claw_is_open;
    
    ev3_lcd_fill_rect(0, 0, EV3_LCD_WIDTH, EV3_LCD_HEIGHT, EV3_LCD_WHITE);
    draw_title("Claw Game", 0, 0, MENU_FONT);

    // move to central position and open the claw
    go_to(-1, -1, 0);
    go_to(x_motor->limit/2, y_motor->limit/2, -1);
    open_claw();
    claw_is_open = true;
    bool running = true;
    
    // start the countdown display
    seconds_counter = 91;
    ev3_sta_cyc(EV3_UPDATE_COUNTER);
    
    while (running)
    {
        if (ev3_button_is_pressed(BACK_BUTTON))
        {
            return;
        }
        
        if (seconds_counter <= 0)
        {
            ev3_stp_cyc(EV3_UPDATE_COUNTER);
            tslp_tsk(3000);
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
                // catch
                if (redup || reddown)
                {
                    if (claw_is_open)
                    {
                        go_to(-1, -1, z_motor->limit-100);
                        close_claw();
                        claw_is_open = false;
                        go_to(-1, -1, 0);
                    }
                    else
                    {
                        open_claw();
                        claw_is_open = true;
                    }
                }
                
                // drop in the bin if it is closed
                if (!claw_is_open && (blueup || bluedown))
                {
                    go_to(x_motor->limit, 0, -1); // move to the bin
                    go_to(-1, -1, x_motor->limit-250); // lower the claw
                    open_claw();
                    claw_is_open = true;
                    go_to(-1, -1, 0); // raise the claw
                }
            }
        }
        // wait 10 mili-seconds
        tslp_tsk(10);
        
        //bignumber.draw((int)90-(getTimeMillis()-start_game)/1000.0);
        /*
        sprintf(buf, "X pos: %ld", x_motor->get_pos());
        print(2, buf);
        sprintf(buf, "Y pos: %ld", y_motor->get_pos());
        print(3, buf);
        sprintf(buf, "Z pos: %ld", z_motor->get_pos());
        print(4, buf);
        sprintf(buf, "C pos: %ld", c_motor->get_pos());
        print(5, buf);
        */
    }
}
