#include "i80_panel.h"
#include "st7789.h"

#include "py/obj.h"
#include "py/runtime.h"
// #include "py/mphal.h"
#include "mphalport.h"
#include "py/gc.h"

#include <string.h>


#define COLOR_SPACE_RGB        (0)
#define COLOR_SPACE_BGR        (1)
#define COLOR_SPACE_MONOCHROME (2)


// this is the actual C-structure for our new object
typedef struct lcd_st7789_obj_t {
    mp_obj_base_t base;
    // mp_obj_t bus_obj;
    mp_obj_base_t *bus_obj;
    mp_obj_t reset;
    mp_obj_t backlight;
    bool reset_level;
    uint8_t color_space;

    mp_obj_t tx_param_method[2];
    mp_obj_t tx_color_method[2];

    int x_gap;
    int y_gap;
    uint32_t bits_per_pixel;
    uint8_t madctl_val; // save current value of LCD_CMD_MADCTL register
    uint8_t colmod_cal; // save surrent value of LCD_CMD_COLMOD register
} lcd_st7789_obj_t;


const char* color_space_string[] = {
    "RGB",
    "BGR",
    "MONOCHROME"
};


STATIC void lcd_st7789_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    (void) kind;
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ST7789 bus=%p, reset=%p, color_space=%s, bits_per_pixel=%u>",
              self->bus_obj,
              self->reset,
              color_space_string[self->color_space],
              self->bits_per_pixel);
}


mp_obj_t lcd_st7789_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t * all_args)
{
    enum {
        ARG_bus,
        ARG_reset,
        ARG_backlight,
        ARG_reset_level,
        ARG_color_space,
        ARG_bits_per_pixel
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bus,            MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL}     },
        { MP_QSTR_reset,          MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL}     },
        { MP_QSTR_backlight,      MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL}     },
        { MP_QSTR_reset_level,    MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false}          },
        { MP_QSTR_color_space,    MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = COLOR_SPACE_RGB} },
        { MP_QSTR_bits_per_pixel, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 16}              },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // create new object
    lcd_st7789_obj_t *self = m_new_obj(lcd_st7789_obj_t);
    self->base.type           = &lcd_st7789_type;

    self->bus_obj = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
    // self->bus_obj        = MP_OBJ_FROM_PTR(args[ARG_bus].u_obj);
    self->reset          = args[ARG_reset].u_obj;
    self->backlight      = args[ARG_backlight].u_obj;
    self->reset_level    = args[ARG_reset_level].u_bool;
    self->color_space    = args[ARG_color_space].u_int;
    self->bits_per_pixel = args[ARG_bits_per_pixel].u_int;

    // reset
    if (self->reset != MP_OBJ_NULL) {
        mp_hal_pin_obj_t reset_pin = mp_hal_get_pin_obj(self->reset);
        mp_hal_pin_output(reset_pin);
    }

    // init backlight
    if (self->backlight != MP_OBJ_NULL) {
        mp_hal_pin_obj_t backlight_pin = mp_hal_get_pin_obj(self->backlight);
        mp_hal_pin_output(backlight_pin);
    }

    mp_load_method_maybe(self->bus_obj, MP_QSTR_tx_param, self->tx_param_method);
    mp_load_method_maybe(self->bus_obj, MP_QSTR_tx_color, self->tx_color_method);

    switch (self->color_space)
    {
        case COLOR_SPACE_RGB:
            self->madctl_val = 0;
        break;

        case COLOR_SPACE_BGR:
            self->madctl_val |= (1 << 3);
        break;

        default:
            mp_raise_msg_varg(&mp_type_ValueError,
                              MP_ERROR_TEXT("unsupported color space"));
        break;
    }

    switch (self->bits_per_pixel)
    {
        case 16:
            self->colmod_cal = 0x55;
        break;

        case 18:
            self->colmod_cal = 0x66;
        break;

        default:
            mp_raise_msg_varg(&mp_type_ValueError,
                              MP_ERROR_TEXT("unsupported pixel width"));
        break;
    }

    return MP_OBJ_FROM_PTR(self);
}


STATIC mp_obj_t lcd_st7789_deinit(mp_obj_t self_in)
{
    // lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // m_del_obj(lcd_st7789_obj_t, self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7789_deinit_obj, lcd_st7789_deinit);


STATIC mp_obj_t lcd_st7789_reset(mp_obj_t self_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->reset != MP_OBJ_NULL) {
        mp_hal_pin_obj_t reset_pin = mp_hal_get_pin_obj(self->reset);
        mp_hal_pin_write(reset_pin, self->reset_level);
        mp_hal_delay_us(10 * 1000);
        mp_hal_pin_write(reset_pin, !self->reset_level);
        mp_hal_delay_us(10 * 1000);
    } else {
        mp_machine_i8080_p_t *i8080_p = (mp_machine_i8080_p_t *)self->bus_obj->type->protocol;
        i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x01, NULL, 0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7789_reset_obj, lcd_st7789_reset);


STATIC mp_obj_t lcd_st7789_init(mp_obj_t self_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i8080_p_t *i8080_p = (mp_machine_i8080_p_t *)self->bus_obj->type->protocol;

    i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x11, NULL, 0);
    mp_hal_delay_us(100 * 1000);

    i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x36, (uint8_t[]) {
        self->madctl_val,
    }, 1);

    i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x3A, (uint8_t[]) {
        self->colmod_cal,
    }, 1);

    // turn on display
    i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x29, NULL, 0);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7789_init_obj, lcd_st7789_init);


STATIC mp_obj_t lcd_st7789_bitmap(size_t n_args, const mp_obj_t *args_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    // mp_obj_t args_out[2];

    int x_start = mp_obj_get_int(args_in[1]);
    int y_start = mp_obj_get_int(args_in[2]);
    int x_end   = mp_obj_get_int(args_in[3]);
    int y_end   = mp_obj_get_int(args_in[4]);

    x_start += self->x_gap;
    x_end += self->x_gap;
    y_start += self->y_gap;
    y_end += self->y_gap;

    mp_machine_i8080_p_t *i8080_p = (mp_machine_i8080_p_t *)self->bus_obj->type->protocol;

    i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x2A, (uint8_t[]) {
        ((x_start >> 8) & 0xFF),
        (x_start & 0xFF),
        (((x_end - 1) >> 8) & 0xFF),
        ((x_end - 1) & 0xFF),
    }, 4);
    i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x2B, (uint8_t[]) {
        ((y_start >> 8) & 0xFF),
        (y_start & 0xFF),
        (((y_end - 1) >> 8) & 0xFF),
        ((y_end - 1) & 0xFF),
    }, 4);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[5], &bufinfo, MP_BUFFER_READ);
    size_t len = ((x_end - x_start) * (y_end - y_start) * self->bits_per_pixel / 8);
    i8080_p->tx_color((lcd_i80_obj_t *)self->bus_obj, 0x2C, bufinfo.buf, len);

    gc_collect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_st7789_bitmap_obj, 6, 6, lcd_st7789_bitmap);


STATIC mp_obj_t lcd_st7789_mirror(mp_obj_t self_in,
                                     mp_obj_t mirror_x_in,
                                     mp_obj_t mirror_y_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i8080_p_t *i8080_p = (mp_machine_i8080_p_t *)self->bus_obj->type->protocol;

    if (mp_obj_is_true(mirror_x_in)) {
        self->madctl_val |= (1 << 6);
    } else {
        self->madctl_val &= ~(1 << 6);
    }
    if (mp_obj_is_true(mirror_y_in)) {
        self->madctl_val |= (1 << 7);
    } else {
        self->madctl_val &= ~(1 << 7);
    }

    i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x36, (uint8_t[]) {
        self->madctl_val
    }, 1);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lcd_st7789_mirror_obj, lcd_st7789_mirror);


STATIC mp_obj_t lcd_st7789_swap_xy(mp_obj_t self_in, mp_obj_t swap_axes_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i8080_p_t *i8080_p = (mp_machine_i8080_p_t *)self->bus_obj->type->protocol;

    if (mp_obj_is_true(swap_axes_in)) {
        self->madctl_val |= 1 << 5;
    } else {
        self->madctl_val &= ~(1 << 5);
    }

    i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x36, (uint8_t[]) {
        self->madctl_val
    }, 1);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_st7789_swap_xy_obj, lcd_st7789_swap_xy);


STATIC mp_obj_t lcd_st7789_set_gap(mp_obj_t self_in, mp_obj_t x_gap_in, mp_obj_t y_gap_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->x_gap = mp_obj_get_int(x_gap_in);
    self->y_gap = mp_obj_get_int(y_gap_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lcd_st7789_set_gap_obj, lcd_st7789_set_gap);


STATIC mp_obj_t lcd_st7789_invert_color(mp_obj_t self_in, mp_obj_t invert_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i8080_p_t *i8080_p = (mp_machine_i8080_p_t *)self->bus_obj->type->protocol;

    if (mp_obj_is_true(invert_in)) {
        i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x21, NULL, 0);
    } else {
        i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x20, NULL, 0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_st7789_invert_color_obj, lcd_st7789_invert_color);


STATIC mp_obj_t lcd_st7789_disp_off(mp_obj_t self_in, mp_obj_t off_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_i8080_p_t *i8080_p = (mp_machine_i8080_p_t *)self->bus_obj->type->protocol;

    if (mp_obj_is_true(off_in)) {
        i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x28, NULL, 0);
    } else {
        i8080_p->tx_param((lcd_i80_obj_t *)self->bus_obj, 0x29, NULL, 0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_st7789_disp_off_obj, lcd_st7789_disp_off);


STATIC mp_obj_t lcd_st7789_backlight_on(mp_obj_t self_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->backlight != MP_OBJ_NULL) {
        mp_hal_pin_obj_t backlight_pin = mp_hal_get_pin_obj(self->backlight);
        mp_hal_pin_write(backlight_pin, 1);
        mp_hal_delay_us(10 * 1000);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7789_backlight_on_obj, lcd_st7789_backlight_on);


STATIC mp_obj_t lcd_st7789_backlight_off(mp_obj_t self_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->backlight != MP_OBJ_NULL) {
        mp_hal_pin_obj_t backlight_pin = mp_hal_get_pin_obj(self->backlight);
        mp_hal_pin_write(backlight_pin, 0);
        mp_hal_delay_us(10 * 1000);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7789_backlight_off_obj, lcd_st7789_backlight_off);


STATIC mp_obj_t lcd_st7789_rgb565(size_t n_args, const mp_obj_t *args_in)
{
    lcd_st7789_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    lcd_i80_obj_t *i8080_obj = MP_OBJ_TO_PTR(self->bus_obj);
    uint16_t color = 0;
    uint16_t a = 0x1234;

    color = ((mp_obj_get_int(args_in[1]) & 0xF8) << 8) | \
            ((mp_obj_get_int(args_in[2]) & 0xFC) << 3) | \
            ((mp_obj_get_int(args_in[3]) & 0xF8) >> 3);

    if ((*(char *)&a) == 0x34) {
        if (!i8080_obj->swap_color_bytes) {
            color = color >> 8 | color << 8;
        }
    } else {
        if (i8080_obj->swap_color_bytes) {
            color = color >> 8 | color << 8;
        }
    }

    return MP_OBJ_NEW_SMALL_INT(color);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_st7789_rgb565_obj, 4, 4, lcd_st7789_rgb565);


STATIC const mp_rom_map_elem_t lcd_st7789_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&lcd_st7789_deinit_obj)        },
    { MP_ROM_QSTR(MP_QSTR_reset),         MP_ROM_PTR(&lcd_st7789_reset_obj)         },
    { MP_ROM_QSTR(MP_QSTR_init),          MP_ROM_PTR(&lcd_st7789_init_obj)          },
    { MP_ROM_QSTR(MP_QSTR_bitmap),        MP_ROM_PTR(&lcd_st7789_bitmap_obj)        },
    { MP_ROM_QSTR(MP_QSTR_mirror),        MP_ROM_PTR(&lcd_st7789_mirror_obj)        },
    { MP_ROM_QSTR(MP_QSTR_swap_xy),       MP_ROM_PTR(&lcd_st7789_swap_xy_obj)       },
    { MP_ROM_QSTR(MP_QSTR_set_gap),       MP_ROM_PTR(&lcd_st7789_set_gap_obj)       },
    { MP_ROM_QSTR(MP_QSTR_invert_color),  MP_ROM_PTR(&lcd_st7789_invert_color_obj)  },
    { MP_ROM_QSTR(MP_QSTR_disp_off),      MP_ROM_PTR(&lcd_st7789_disp_off_obj)      },
    { MP_ROM_QSTR(MP_QSTR_backlight_on),  MP_ROM_PTR(&lcd_st7789_backlight_on_obj)  },
    { MP_ROM_QSTR(MP_QSTR_backlight_off), MP_ROM_PTR(&lcd_st7789_backlight_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_color565),      MP_ROM_PTR(&lcd_st7789_rgb565_obj)        },
    { MP_ROM_QSTR(MP_QSTR___del__),       MP_ROM_PTR(&lcd_st7789_deinit_obj)        },

    { MP_ROM_QSTR(MP_QSTR_RGB),           MP_ROM_INT(COLOR_SPACE_RGB)               },
    { MP_ROM_QSTR(MP_QSTR_BGR),           MP_ROM_INT(COLOR_SPACE_BGR)               },
    { MP_ROM_QSTR(MP_QSTR_MONOCHROME),    MP_ROM_INT(COLOR_SPACE_MONOCHROME)        }
};
STATIC MP_DEFINE_CONST_DICT(lcd_st7789_locals_dict, lcd_st7789_locals_dict_table);


const mp_obj_type_t lcd_st7789_type = {
    { &mp_type_type },
    .name        = MP_QSTR_ST7789,
    .print       = lcd_st7789_print,
    .make_new    = lcd_st7789_make_new,
    .locals_dict = (mp_obj_dict_t *)&lcd_st7789_locals_dict,
};