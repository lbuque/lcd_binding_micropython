#ifndef _LCD_PANEL_ROTATION_H_
#define _LCD_PANEL_ROTATION_H_

#include <stdint.h>

typedef struct _lcd_panel_rotation_t {
    uint8_t madctl;
    uint16_t width;
    uint16_t height;
    uint16_t colstart;
    uint16_t rowstart;
} lcd_panel_rotation_t;

#endif
