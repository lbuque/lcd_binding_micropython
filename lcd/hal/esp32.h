#ifndef _ESP32_H_
#define _ESP32_H_
#include "i80_panel.h"
#include "rgb_panel.h"

// i8080
void hal_lcd_i80_construct(lcd_i80_obj_t *self);

void hal_lcd_i80_tx_param(lcd_i80_obj_t *self, int lcd_cmd, const void *param, size_t param_size);

void hal_lcd_i80_tx_color(lcd_i80_obj_t *self, int lcd_cmd, const void *color, size_t color_size);

void hal_lcd_i80_deinit(lcd_i80_obj_t *self);

// rgb
void hal_lcd_rgb_construct(lcd_rgb_obj_t *self);

void hal_lcd_rgb_reset(lcd_rgb_obj_t *self);

void hal_lcd_rgb_init(lcd_rgb_obj_t *self);

void hal_lcd_rgb_del(lcd_rgb_obj_t *self);

void hal_lcd_rgb_bitmap(lcd_rgb_obj_t *self, int x_start, int y_start, int x_end, int y_end, const void *color_data);

void hal_lcd_rgb_mirror(lcd_rgb_obj_t *self, bool mirror_x, bool mirror_y);

void hal_lcd_rgb_swap_xy(lcd_rgb_obj_t *self, bool swap_axes);

void hal_lcd_rgb_set_gap(lcd_rgb_obj_t *self, int x_gap, int y_gap);

void hal_lcd_rgb_invert_color(lcd_rgb_obj_t *self, bool invert_color_data);

void hal_lcd_rgb_disp_off(lcd_rgb_obj_t *self, bool off);

#endif