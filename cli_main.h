// cli_main.h
#pragma once
#include "cli_menu.h"

extern Motor* x_motor;
extern Motor* y_motor;
extern Motor* z_motor;
extern Motor* c_motor;

extern const CliMenu climenu_main;

void control(intptr_t unused);
void homing(intptr_t unused);
