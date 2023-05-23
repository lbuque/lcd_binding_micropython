#ifndef _LCD_SOFTSPI_H_
#define _LCD_SOFTSPI_H_

#include "py/obj.h"

void hal_lcd_softspi_panel_construct(mp_obj_base_t *self);

void hal_lcd_softspi_panel_tx_param(mp_obj_base_t *self,
                                    int            lcd_cmd,
                                    const void    *param,
                                    size_t         param_size);

void hal_lcd_softspi_panel_tx_color(mp_obj_base_t *self,
                                    int            lcd_cmd,
                                    const void    *color,
                                    size_t         color_size);

void hal_lcd_softspi_panel_deinit(mp_obj_base_t *self);

#endif
