#ifndef _ST7789_ROTATION_H_
#define _ST7789_ROTATION_H_

#include "lcd_panel_rotation.h"


//
// Default st7789 and st7735 display orientation tables
// can be overridden during init(), madctl values
// will be combined with color_mode
//
//{ madctl, width, height, colstart, rowstart }
//
STATIC const lcd_panel_rotation_t ORIENTATIONS_GENERAL[4] = {
    { 0x00, 0, 0, 0, 0},
    { 0x60, 0, 0, 0, 0},
    { 0xC0, 0, 0, 0, 0},
    { 0xA0, 0, 0, 0, 0}
};


STATIC const lcd_panel_rotation_t ORIENTATIONS_240x240[4] = {
    {0x00, 240, 240,  0,  0},
    {0x60, 240, 240,  0,  0},
    {0xC0, 240, 240,  0,  0},
    {0xA0, 240, 240,  0,  0}
};

#endif