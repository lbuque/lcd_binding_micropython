#ifndef SOFT8080_H_
#define SOFT8080_H_

#include "i80_panel.h"

void hal_lcd_i80_construct(mp_lcd_i80_obj_t *self);

void hal_lcd_i80_tx_param(mp_lcd_i80_obj_t *self, int lcd_cmd, const void *param, size_t param_size);

void hal_lcd_i80_tx_color(mp_lcd_i80_obj_t *self, int lcd_cmd, const void *color, size_t color_size);

void hal_lcd_i80_deinit(mp_lcd_i80_obj_t *self);

#endif