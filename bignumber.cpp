// bignumber.cpp
#include "bignumber.h"

bool BigNumber::initialized = false;
image_t BigNumber::number_imgs[10];

BigNumber::BigNumber()
{
    if (!initialized)
    {
        load_images();
        initialized = true;
    }
}

void BigNumber::draw(int n)
{
    n = n % 100;
    int y_pos = 0;
    
    // draw first number
    int x_pos = (EV3_LCD_WIDTH - get_total_number_width(n)) / 2;
    ev3_lcd_draw_image(&number_imgs[n / 10], x_pos, y_pos);
    
    // draw second number
    x_pos += get_number_width(n / 10) + DISTANCE_BETWEEN_NUMBERS;
    ev3_lcd_draw_image(&number_imgs[n % 10], x_pos, y_pos);
}

void BigNumber::load_images()
{
    const char *inputPath = "/number_imgs";
    
    char path[50];
    for(int n = 0; n < 10; n++)
    {
        static memfile_t memfile = { .buffer = NULL };
        if (memfile.buffer != NULL) ev3_memfile_free(&memfile);
        sprintf(path, "%s/%d.bmp", inputPath, n);
        ev3_memfile_load(path, &memfile);
        ev3_image_load(&memfile, &number_imgs[n]);
    }
}

int BigNumber::get_number_width(int n)
{
    n = n % 10;
    return number_imgs[n].width;
}

int BigNumber::get_total_number_width(int n)
{
    n = n % 100;
    return get_number_width(n / 10) + DISTANCE_BETWEEN_NUMBERS + get_number_width(n % 10);
}
