/**
 * This sample program runs motors A, B, C & D using a IR Remote control on channels 1 & 2.
 *
 */

#include "ev3api.h"
#include "app.h"
#include "utils.h"
#include "motor.h"
#include "cli_menu.h"
#include "cli_main.h"
#include "parameters.h"
#include <stdlib.h>

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif

void main_task(intptr_t unused)
{
    char buf[100];
    
    print(0, APP_NAME);
    sprintf(buf, "Port%c:X motor", 'A' + X_MOTOR_PORT);
    print(1, buf);
    sprintf(buf, "Port%c:Y motor", 'A' + Y_MOTOR_PORT);
    print(2, buf);
    sprintf(buf, "Port%c:Z motor", 'A' + Z_MOTOR_PORT);
    print(3, buf);
    sprintf(buf, "Port%c:Claw motor", 'A' + C_MOTOR_PORT);
    print(4, buf);
    sprintf(buf, "Port%c:IR sensor",  '1' + IR_SENSOR_PORT);
    print(6, buf);
    sprintf(buf, "Port%c:Color sensor",  '1' + COLOR_SENSOR_PORT);
    print(7, buf);

    // Configure motors
    x_motor = new Motor(X_MOTOR_PORT, LARGE_MOTOR);
    y_motor = new Motor(Y_MOTOR_PORT, LARGE_MOTOR);
    z_motor = new Motor(Z_MOTOR_PORT, LARGE_MOTOR);
    c_motor = new Motor(C_MOTOR_PORT, MEDIUM_MOTOR);
    
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
    ev3_sensor_config(IR_SENSOR_PORT, INFRARED_SENSOR);
    ev3_sensor_config(COLOR_SENSOR_PORT, COLOR_SENSOR);
    
    waitEnterButtonPressed();
    
    while(1) {
        show_cli_menu(&climenu_main, 0, 0, MENU_FONT);
        const CliMenuEntry *cme = select_menu_entry(&climenu_main, 0, MENU_FONT_HEIGHT, MENU_FONT);
        if(cme != NULL) {
            assert(cme->handler != NULL);
            cme->handler(cme->exinf);
        }
    }
}
