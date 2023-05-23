#ifndef _LCD_PANEL_H_
#define _LCD_PANEL_H_

#include "py/obj.h"

typedef struct _mp_lcd_panel_p_t {
    void (*tx_param)(mp_obj_base_t *self, int lcd_cmd, const void *param, size_t param_size);
    void (*tx_color)(mp_obj_base_t *self, int lcd_cmd, const void *color, size_t color_size);
    void (*deinit)(mp_obj_base_t *self);
} mp_lcd_panel_p_t;

#endif