#ifndef _ST7735_ROTATION_H_
#define _ST7735_ROTATION_H_

#include "lcd_panel_rotation.h"

//
// Default st7735 display orientation tables
// can be overridden during init(), madctl values
// will be combined with color_mode
//

STATIC const lcd_panel_rotation_t ORIENTATIONS_GENERAL[4] = {
    { 0x00, 0, 0, 0, 0}, // { madctl, width, height, colstart, rowstart }
    { 0x60, 0, 0, 0, 0},
    { 0xC0, 0, 0, 0, 0},
    { 0xA0, 0, 0, 0, 0}
};

STATIC const lcd_panel_rotation_t ORIENTATIONS_160x80[4] = {
    { 0x00,  80, 160, 26,  1 }, // { madctl, width, height, colstart, rowstart }
    { 0x60, 160,  80,  1, 26 },
    { 0xC0,  80, 160, 26,  1 },
    { 0xA0, 160,  80,  1, 26 }
};

#endif