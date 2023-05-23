#ifndef _ESP32_H_
#define _ESP32_H_

#include "py/obj.h"

// spi
void hal_lcd_spi_panel_construct(mp_obj_base_t *self);

void hal_lcd_spi_panel_tx_param(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *param,
    size_t param_size
);

void hal_lcd_spi_panel_tx_color(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *color,
    size_t color_size
);

void hal_lcd_spi_panel_deinit(mp_obj_base_t *self);

// qspi
void hal_lcd_qspi_panel_construct(mp_obj_base_t *self);

void hal_lcd_qspi_panel_tx_param(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *param,
    size_t param_size
);

void hal_lcd_qspi_panel_tx_color(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *color,
    size_t color_size
);

void hal_lcd_qspi_panel_deinit(mp_obj_base_t *self);

// i8080
void hal_lcd_i80_construct(mp_obj_base_t *self);

void hal_lcd_i80_tx_param(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *param,
    size_t param_size
);

void hal_lcd_i80_tx_color(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *color,
    size_t color_size
);

void hal_lcd_i80_deinit(mp_obj_base_t *self);

// dpi
#if DPI_LCD_SUPPORTED
void hal_lcd_dpi_construct(mp_obj_base_t *self);

void hal_lcd_dpi_reset(mp_obj_base_t *self);

void hal_lcd_dpi_init(mp_obj_base_t *self);

void hal_lcd_dpi_del(mp_obj_base_t *self);

void hal_lcd_dpi_bitmap(
    mp_obj_base_t *self,
    int x_start,
    int y_start,
    int x_end,
    int y_end,
    const void *color_data
);

void hal_lcd_dpi_mirror(mp_obj_base_t *self, bool mirror_x, bool mirror_y);

void hal_lcd_dpi_swap_xy(mp_obj_base_t *self, bool swap_axes);

void hal_lcd_dpi_set_gap(mp_obj_base_t *self, int x_gap, int y_gap);

void hal_lcd_dpi_invert_color(mp_obj_base_t *self, bool invert_color_data);

void hal_lcd_dpi_disp_off(mp_obj_base_t *self, bool off);
#endif

#endif