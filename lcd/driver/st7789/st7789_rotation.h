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

STATIC const lcd_panel_rotation_t ORIENTATIONS_240x320[4] = {
    { 0x00, 240, 320, 0, 0},
    { 0x60, 320, 240, 0, 0},
    { 0xC0, 240, 320, 0, 0},
    { 0xA0, 320, 240, 0, 0}
};

STATIC const lcd_panel_rotation_t ORIENTATIONS_170x320[4] = {
    {0x00, 170, 320, 35,  0},
    {0x60, 320, 170,  0, 35},
    {0xC0, 170, 320, 35,  0},
    {0xA0, 320, 170,  0, 35}
};

STATIC const lcd_panel_rotation_t ORIENTATIONS_240x240[4] = {
    {0x00, 240, 240,  0,  0},
    {0x60, 240, 240,  0,  0},
    {0xC0, 240, 240,  0, 80},
    {0xA0, 240, 240, 80,  0}
};

STATIC const lcd_panel_rotation_t ORIENTATIONS_135x240[4] = {
    {0x00, 135, 240, 52, 40},
    {0x60, 240, 135, 40, 53},
    {0xC0, 135, 240, 53, 40},
    {0xA0, 240, 135, 40, 52}
};

STATIC const lcd_panel_rotation_t ORIENTATIONS_128x160[4] = {
    {0x00, 128, 160, 0, 0},
    {0x60, 160, 128, 0, 0},
    {0xC0, 128, 160, 0, 0},
    {0xA0, 160, 128, 0, 0}
};

STATIC const lcd_panel_rotation_t ORIENTATIONS_128x128[4] = {
    {0x00, 128, 128, 2, 1},
    {0x60, 128, 128, 1, 2},
    {0xC0, 128, 128, 2, 3},
    {0xA0, 128, 128, 3, 2}
};

#endif