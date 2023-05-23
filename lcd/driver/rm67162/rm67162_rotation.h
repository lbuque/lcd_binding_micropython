#ifndef _RM67162_ROTATION_H_
#define _RM67162_ROTATION_H_

#include "lcd_panel_rotation.h"

//
// Default rm67162 display orientation tables
// can be overridden during init(), madctl values
// will be combined with color_mode
//

STATIC const lcd_panel_rotation_t ORIENTATIONS_GENERAL[4] = {
    { 0x00, 0, 0, 0, 0}, // { madctl, width, height, colstart, rowstart }
    { 0x60, 0, 0, 0, 0},
    { 0xC0, 0, 0, 0, 0},
    { 0xA0, 0, 0, 0, 0}
};

STATIC const lcd_panel_rotation_t ORIENTATIONS_240x536[4] = {
    { 0x00, 240, 536, 0, 0 },
    { 0x60, 536, 240, 0, 0 },
    { 0xC0, 240, 160, 0, 0 },
    { 0xA0, 536, 240, 0, 0 }
};

#endif