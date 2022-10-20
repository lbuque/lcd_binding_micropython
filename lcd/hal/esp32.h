#ifndef _ESP32_H_
#define _ESP32_H_
#include "i80_panel.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

void hal_lcd_i80_construct(lcd_i80_obj_t *self);

void hal_lcd_i80_tx_param(lcd_i80_obj_t *self, int lcd_cmd, const void *param, size_t param_size);

void hal_lcd_i80_tx_color(lcd_i80_obj_t *self, int lcd_cmd, const void *color, size_t color_size);

void hal_lcd_i80_deinit(lcd_i80_obj_t *self);


#endif