#include "rgb_panel.h"

#if USE_ESP_LCD
#include "hal/esp32.h"
#endif

#include "py/obj.h"
#include "py/runtime.h"

#include "py/mphal.h"
#include "py/gc.h"

#include <string.h>

STATIC void lcd_rgb_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    (void) kind;
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<RGB pclk=%u, width=%u, height=%u>",
              self->pclk,
              self->width,
              self->height);
}


mp_obj_t lcd_rgb_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t * all_args)
{
    enum {
        ARG_data,
        ARG_hsync,
        ARG_vsync,
        ARG_de,
        ARG_pclk_pin,
        ARG_timings,
        ARG_disp,
        ARG_backlight,
        ARG_pclk,
        ARG_width,
        ARG_height,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data,      MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_hsync,     MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_vsync,     MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_de,        MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_pclk_pin,  MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timings,   MP_ARG_OBJ | MP_ARG_REQUIRED | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_disp,      MP_ARG_OBJ | MP_ARG_KW_ONLY,                   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_backlight, MP_ARG_OBJ | MP_ARG_KW_ONLY,                   {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_pclk,      MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 8000000    } },
        { MP_QSTR_width,     MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 480        } },
        { MP_QSTR_height,    MP_ARG_INT | MP_ARG_KW_ONLY,                   {.u_int = 480        } },
       
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // create new object
    lcd_rgb_obj_t *self = m_new_obj(lcd_rgb_obj_t);
    self->base.type = &lcd_rgb_type;

    // data bus
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(args[ARG_data].u_obj);
    self->bus_width = t->len;
    for (size_t i = 0; i < t->len; i++)
    {
        self->databus_obj[i] = t->items[i];
    }

    self->hsync_obj = args[ARG_hsync].u_obj;
    self->vsync_obj = args[ARG_vsync].u_obj;
    self->de_obj    = args[ARG_de].u_obj;
    self->pclk_obj  = args[ARG_pclk_pin].u_obj;

    // timings
    t = MP_OBJ_TO_PTR(args[ARG_timings].u_obj);
    self->timings.hsync_pulse_width = mp_obj_get_int(t->items[0]);
    self->timings.hsync_back_porch  = mp_obj_get_int(t->items[1]);
    self->timings.hsync_front_porch = mp_obj_get_int(t->items[2]);
    self->timings.vsync_pulse_width = mp_obj_get_int(t->items[3]);
    self->timings.vsync_back_porch  = mp_obj_get_int(t->items[4]);
    self->timings.vsync_front_porch = mp_obj_get_int(t->items[5]);

    // disp gpio
    self->disp_obj = args[ARG_disp].u_obj;
    if (self->disp_obj != MP_OBJ_NULL) {
        self->disp_pin = mp_hal_get_pin_obj(self->disp_obj);
    } else {
        self->disp_pin = -1;
    }

    // init backlight
    self->backlight      = args[ARG_backlight].u_obj;
    if (self->backlight != MP_OBJ_NULL) {
        mp_hal_pin_obj_t backlight_pin = mp_hal_get_pin_obj(self->backlight);
        mp_hal_pin_output(backlight_pin);
    }

    self->pclk = args[ARG_pclk].u_int;
    self->width = args[ARG_width].u_int;
    self->height = args[ARG_height].u_int;

    hal_lcd_rgb_construct(self);
    return MP_OBJ_FROM_PTR(self);
}


STATIC mp_obj_t lcd_rgb_deinit(mp_obj_t self_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // m_del_obj(lcd_rgb_obj_t, self);
    hal_lcd_rgb_del(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_rgb_deinit_obj, lcd_rgb_deinit);


STATIC mp_obj_t lcd_rgb_reset(mp_obj_t self_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    hal_lcd_rgb_reset(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_rgb_reset_obj, lcd_rgb_reset);


STATIC mp_obj_t lcd_rgb_init(mp_obj_t self_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    hal_lcd_rgb_init(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_rgb_init_obj, lcd_rgb_init);


STATIC mp_obj_t lcd_rgb_bitmap(size_t n_args, const mp_obj_t *args_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[5], &bufinfo, MP_BUFFER_READ);

    hal_lcd_rgb_bitmap(self,
                       mp_obj_get_int(args_in[1]),
                       mp_obj_get_int(args_in[2]),
                       mp_obj_get_int(args_in[3]),
                       mp_obj_get_int(args_in[4]),
                       bufinfo.buf);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_rgb_bitmap_obj, 6, 6, lcd_rgb_bitmap);


STATIC mp_obj_t lcd_rgb_mirror(mp_obj_t self_in,
                                     mp_obj_t mirror_x_in,
                                     mp_obj_t mirror_y_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    hal_lcd_rgb_mirror(self, mp_obj_is_true(mirror_x_in), mp_obj_is_true(mirror_y_in));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lcd_rgb_mirror_obj, lcd_rgb_mirror);


STATIC mp_obj_t lcd_rgb_swap_xy(mp_obj_t self_in, mp_obj_t swap_axes_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    hal_lcd_rgb_swap_xy(self, mp_obj_is_true(swap_axes_in));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_rgb_swap_xy_obj, lcd_rgb_swap_xy);


STATIC mp_obj_t lcd_rgb_set_gap(mp_obj_t self_in, mp_obj_t x_gap_in, mp_obj_t y_gap_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    hal_lcd_rgb_set_gap(self, mp_obj_get_int(x_gap_in), mp_obj_get_int(y_gap_in));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lcd_rgb_set_gap_obj, lcd_rgb_set_gap);


STATIC mp_obj_t lcd_rgb_invert_color(mp_obj_t self_in, mp_obj_t invert_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    hal_lcd_rgb_invert_color(self, mp_obj_is_true(invert_in));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_rgb_invert_color_obj, lcd_rgb_invert_color);


STATIC mp_obj_t lcd_rgb_disp_off(mp_obj_t self_in, mp_obj_t off_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    hal_lcd_rgb_disp_off(self, mp_obj_is_true(off_in));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_rgb_disp_off_obj, lcd_rgb_disp_off);


STATIC mp_obj_t lcd_rgb_backlight_on(mp_obj_t self_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->backlight != MP_OBJ_NULL) {
        mp_hal_pin_obj_t backlight_pin = mp_hal_get_pin_obj(self->backlight);
        mp_hal_pin_write(backlight_pin, 1);
        mp_hal_delay_ms(10);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_rgb_backlight_on_obj, lcd_rgb_backlight_on);


STATIC mp_obj_t lcd_rgb_backlight_off(mp_obj_t self_in)
{
    lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->backlight != MP_OBJ_NULL) {
        mp_hal_pin_obj_t backlight_pin = mp_hal_get_pin_obj(self->backlight);
        mp_hal_pin_write(backlight_pin, 0);
        mp_hal_delay_ms(10);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_rgb_backlight_off_obj, lcd_rgb_backlight_off);


#if 0
// STATIC mp_obj_t lcd_rgb_rgb565(size_t n_args, const mp_obj_t *args_in)
// {
//     lcd_rgb_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
//     lcd_i80_obj_t *i8080_obj = MP_OBJ_TO_PTR(self->bus_obj);
//     uint16_t color = 0;
//     uint16_t a = 0x1234;

//     color = ((mp_obj_get_int(args_in[1]) & 0xF8) << 8) | 
//             ((mp_obj_get_int(args_in[2]) & 0xFC) << 3) | 
//             ((mp_obj_get_int(args_in[3]) & 0xF8) >> 3);

//     if ((*(char *)&a) == 0x34) {
//         if (!i8080_obj->swap_color_bytes) {
//             color = color >> 8 | color << 8;
//         }
//     } else {
//         if (i8080_obj->swap_color_bytes) {
//             color = color >> 8 | color << 8;
//         }
//     }

//     return MP_OBJ_NEW_SMALL_INT(color);
// }
// STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_rgb_rgb565_obj, 4, 4, lcd_rgb_rgb565);
#endif

STATIC const mp_rom_map_elem_t lcd_rgb_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&lcd_rgb_deinit_obj)        },
    { MP_ROM_QSTR(MP_QSTR_reset),         MP_ROM_PTR(&lcd_rgb_reset_obj)         },
    { MP_ROM_QSTR(MP_QSTR_init),          MP_ROM_PTR(&lcd_rgb_init_obj)          },
    { MP_ROM_QSTR(MP_QSTR_bitmap),        MP_ROM_PTR(&lcd_rgb_bitmap_obj)        },
    { MP_ROM_QSTR(MP_QSTR_mirror),        MP_ROM_PTR(&lcd_rgb_mirror_obj)        },
    { MP_ROM_QSTR(MP_QSTR_swap_xy),       MP_ROM_PTR(&lcd_rgb_swap_xy_obj)       },
    { MP_ROM_QSTR(MP_QSTR_set_gap),       MP_ROM_PTR(&lcd_rgb_set_gap_obj)       },
    { MP_ROM_QSTR(MP_QSTR_invert_color),  MP_ROM_PTR(&lcd_rgb_invert_color_obj)  },
    { MP_ROM_QSTR(MP_QSTR_disp_off),      MP_ROM_PTR(&lcd_rgb_disp_off_obj)      },
    { MP_ROM_QSTR(MP_QSTR_backlight_on),  MP_ROM_PTR(&lcd_rgb_backlight_on_obj)  },
    { MP_ROM_QSTR(MP_QSTR_backlight_off), MP_ROM_PTR(&lcd_rgb_backlight_off_obj) },
    // { MP_ROM_QSTR(MP_QSTR_color565),      MP_ROM_PTR(&lcd_rgb_rgb565_obj)        },
    { MP_ROM_QSTR(MP_QSTR___del__),       MP_ROM_PTR(&lcd_rgb_deinit_obj)        },
};
STATIC MP_DEFINE_CONST_DICT(lcd_rgb_locals_dict, lcd_rgb_locals_dict_table);



const mp_obj_type_t lcd_rgb_type = {
    { &mp_type_type },
    .name        = MP_QSTR_RGB,
    .print       = lcd_rgb_print,
    .make_new    = lcd_rgb_make_new,
    .locals_dict = (mp_obj_dict_t *)&lcd_rgb_locals_dict,
};