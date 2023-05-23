#ifndef _ILI9488_ROTATION_H_
#define _ILI9488_ROTATION_H_

#include "lcd_panel_rotation.h"

//
// Default ili9488 display orientation tables
// can be overridden during init(), madctl values
// will be combined with color_mode
//
STATIC const lcd_panel_rotation_t ORIENTATIONS_GENERAL[4] = {
    { 0x40, 0, 0, 0, 0},
    { 0x20, 0, 0, 0, 0},
    { 0x80, 0, 0, 0, 0},
    { 0xE0, 0, 0, 0, 0}
};


#endif