#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/gc.h"

#if CONFIG_IDF_TARGET_ESP32S3
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#endif

#include "i80_panel.h"
#include "st7789.h"

#include "string.h"

#define COLOR_SPACE_RGB        (0)
#define COLOR_SPACE_BGR        (1)
#define COLOR_SPACE_MONOCHROME (2)

// this is the actual C-structure for our new object
typedef struct st7789_ST7789_obj_t {
    mp_obj_base_t base;
    // mp_obj_base_t *spi_obj;

    mp_obj_t bus_obj;
    mp_obj_t reset;
    bool reset_level;
    uint8_t color_space;

    mp_obj_t tx_param_method[2];
    mp_obj_t tx_color_method[2];

#if CONFIG_IDF_TARGET_ESP32S3
    esp_lcd_panel_handle_t panel_handle;
#endif

    int x_gap;
    int y_gap;
    uint32_t bits_per_pixel;
    uint8_t madctl_val; // save current value of LCD_CMD_MADCTL register
    uint8_t colmod_cal; // save surrent value of LCD_CMD_COLMOD register
} st7789_ST7789_obj_t;


const char* color_space_string[] = {
    "RGB",
    "BGR",
    "MONOCHROME"
};

STATIC void st7789_ST7789_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    (void) kind;
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ST7789 bus=%p, reset=%p, color_space=%s, bits_per_pixel=%u>",
              self->bus_obj,
              self->reset,
              color_space_string[self->color_space],
              self->bits_per_pixel);
}

mp_obj_t st7789_ST7789_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t * all_args)
{
    enum {
        ARG_bus,
        ARG_reset,
        ARG_reset_level,
        ARG_color_space,
        ARG_bits_per_pixel
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bus,            MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL}     },
        { MP_QSTR_reset,          MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL}     },
        { MP_QSTR_reset_level,    MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false}          },
        { MP_QSTR_color_space,    MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = COLOR_SPACE_RGB} },
        { MP_QSTR_bits_per_pixel, MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 16}              },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // create new object
    st7789_ST7789_obj_t *self = m_new_obj(st7789_ST7789_obj_t);
    self->base.type           = &st7789_ST7789_type;

    self->bus_obj        = MP_OBJ_FROM_PTR(args[ARG_bus].u_obj);
    self->reset          = args[ARG_reset].u_obj;
    self->reset_level    = args[ARG_reset_level].u_bool;
    self->color_space    = args[ARG_color_space].u_int;
    self->bits_per_pixel = args[ARG_bits_per_pixel].u_int;

    // reset
    if (self->reset != MP_OBJ_NULL) {
        mp_hal_pin_obj_t reset_pin = mp_hal_get_pin_obj(self->reset);
        mp_hal_pin_output(reset_pin);
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

#if 0
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = 5,
        .color_space = self->color_space,
        .bits_per_pixel = 16,
    };
    parallelbus_i80_obj_t *i8080_in = MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(i8080_in->io_handle, &panel_config, &self->panel_handle));
    esp_lcd_panel_reset(self->panel_handle);
    esp_lcd_panel_init(self->panel_handle);
    esp_lcd_panel_invert_color(self->panel_handle, true);

    esp_lcd_panel_swap_xy(self->panel_handle, true);
    esp_lcd_panel_mirror(self->panel_handle, false, true);
    esp_lcd_panel_set_gap(self->panel_handle, 0, 35);

    self->x_gap = 0;
    self->y_gap = 0;
#endif
    return MP_OBJ_FROM_PTR(self);
}


STATIC mp_obj_t st7789_ST7789_deinit(mp_obj_t self_in)
{
    // st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    // m_del_obj(st7789_ST7789_obj_t, self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(st7789_ST7789_deinit_obj, st7789_ST7789_deinit);


STATIC mp_obj_t st7789_ST7789_reset(mp_obj_t self_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->reset != MP_OBJ_NULL) {
        mp_hal_pin_obj_t reset_pin = mp_hal_get_pin_obj(self->reset);
        mp_hal_pin_write(reset_pin, self->reset_level);
        mp_hal_delay_ms(10);
        mp_hal_pin_write(reset_pin, !self->reset_level);
        mp_hal_delay_ms(10);
    } else {
        mp_obj_t args_out[1];
        args_out[0] = mp_obj_new_int(0x01);
        mp_call_method_self_n_kw(self->tx_param_method[0],
                                 self->tx_param_method[1],
                                 1,
                                 0,
                                 args_out);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(st7789_ST7789_reset_obj, st7789_ST7789_reset);


STATIC mp_obj_t st7789_ST7789_init(mp_obj_t self_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t args_out[2];

    args_out[0] = mp_obj_new_int(0x11);
    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             1,
                             0,
                             args_out);
    mp_hal_delay_ms(100);

    args_out[0] = mp_obj_new_int(0x36);
    args_out[1] = mp_obj_new_bytes((uint8_t[]) {
        self->madctl_val,
    }, 1);
    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             2,
                             0,
                             args_out);
    m_del_obj(mp_obj_array_t, MP_OBJ_TO_PTR(args_out[1]));
    gc_collect();

    args_out[0] = mp_obj_new_int(0x3A);
    args_out[1] = mp_obj_new_bytes((uint8_t[]) {
        self->colmod_cal,
    }, 1);
    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             2,
                             0,
                             args_out);
    m_del_obj(mp_obj_array_t, MP_OBJ_TO_PTR(args_out[1]));
    gc_collect();

    // turn on display
    args_out[0] = mp_obj_new_int(0x29);
    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             1,
                             0,
                             args_out);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(st7789_ST7789_init_obj, st7789_ST7789_init);


STATIC mp_obj_t st7789_ST7789_bitmap(size_t n_args, const mp_obj_t *args_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    mp_obj_t args_out[2];

    int x_start = mp_obj_get_int(args_in[1]);
    int y_start = mp_obj_get_int(args_in[2]);
    int x_end   = mp_obj_get_int(args_in[3]);
    int y_end   = mp_obj_get_int(args_in[4]);

    x_start += self->x_gap;
    x_end += self->x_gap;
    y_start += self->y_gap;
    y_end += self->y_gap;

    args_out[0] = mp_obj_new_int(0x2A);
    args_out[1] = mp_obj_new_bytes((uint8_t[]) {
        ((x_start >> 8) & 0xFF),
        (x_start & 0xFF),
        (((x_end - 1) >> 8) & 0xFF),
        ((x_end - 1) & 0xFF),
    }, 4);
    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             2,
                             0,
                             args_out);
    m_del_obj(mp_obj_array_t, MP_OBJ_TO_PTR(args_out[1]));
    gc_collect();

    args_out[0] = mp_obj_new_int(0x2B);
    args_out[1] = mp_obj_new_bytes((uint8_t[]) {
        ((y_start >> 8) & 0xFF),
        (y_start & 0xFF),
        (((y_end - 1) >> 8) & 0xFF),
        ((y_end - 1) & 0xFF),
    }, 4);
    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             2,
                             0,
                             args_out);
    m_del_obj(mp_obj_array_t, MP_OBJ_TO_PTR(args_out[1]));
    gc_collect();

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args_in[5], &bufinfo, MP_BUFFER_READ);

    args_out[0] = mp_obj_new_int(0x2C);
    // args_out[1] = mp_obj_new_bytearray_by_ref(((x_end - x_start) * (y_end - y_start) * self->bits_per_pixel / 8), bufinfo.buf);
    args_out[1] = mp_obj_new_bytes(bufinfo.buf, ((x_end - x_start) * (y_end - y_start) * self->bits_per_pixel / 8));
    mp_call_method_self_n_kw(self->tx_color_method[0],
                             self->tx_color_method[1],
                             2,
                             0,
                             args_out);
    m_del_obj(mp_obj_array_t, MP_OBJ_TO_PTR(args_out[1]));
    gc_collect();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(st7789_ST7789_bitmap_obj, 6, 6, st7789_ST7789_bitmap);


STATIC mp_obj_t st7789_ST7789_mirror(mp_obj_t self_in,
                                     mp_obj_t mirror_x_in,
                                     mp_obj_t mirror_y_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t args_out[2];

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

    args_out[0] = mp_obj_new_int(0x36);
    args_out[1] = mp_obj_new_bytes((uint8_t[]) {
        self->madctl_val
    }, 1);
    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             2,
                             0,
                             args_out);
    m_del_obj(mp_obj_array_t, MP_OBJ_TO_PTR(args_out[1]));
    gc_collect();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(st7789_ST7789_mirror_obj, st7789_ST7789_mirror);


STATIC mp_obj_t st7789_ST7789_swap_xy(mp_obj_t self_in, mp_obj_t swap_axes_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t args_out[2];

    if (mp_obj_is_true(swap_axes_in)) {
        self->madctl_val |= 1 << 5;
    } else {
        self->madctl_val &= ~(1 << 5);
    }

    args_out[0] = mp_obj_new_int(0x36);
    args_out[1] = mp_obj_new_bytes((uint8_t[]) {
        self->madctl_val
    }, 1);
    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             2,
                             0,
                             args_out);
    m_del_obj(mp_obj_array_t, MP_OBJ_TO_PTR(args_out[1]));
    gc_collect();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(st7789_ST7789_swap_xy_obj, st7789_ST7789_swap_xy);


STATIC mp_obj_t st7789_ST7789_set_gap(mp_obj_t self_in, mp_obj_t x_gap_in, mp_obj_t y_gap_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->x_gap = mp_obj_get_int(x_gap_in);
    self->y_gap = mp_obj_get_int(y_gap_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(st7789_ST7789_set_gap_obj, st7789_ST7789_set_gap);


STATIC mp_obj_t st7789_ST7789_invert_color(mp_obj_t self_in, mp_obj_t invert_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t args_out[1];

    if (mp_obj_is_true(invert_in)) {
        args_out[0] = mp_obj_new_int(0x21);
    } else {
        args_out[0] = mp_obj_new_int(0x20);
    }

    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             1,
                             0,
                             args_out);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(st7789_ST7789_invert_color_obj, st7789_ST7789_invert_color);


STATIC mp_obj_t st7789_ST7789_disp_off(mp_obj_t self_in, mp_obj_t off_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t args_out[1];

    if (mp_obj_is_true(off_in)) {
        args_out[0] = mp_obj_new_int(0x28);
    } else {
        args_out[0] = mp_obj_new_int(0x29);
    }

    mp_call_method_self_n_kw(self->tx_param_method[0],
                             self->tx_param_method[1],
                             1,
                             0,
                             args_out);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(st7789_ST7789_disp_off_obj, st7789_ST7789_disp_off);


STATIC mp_obj_t st7789_ST7789_backlight_on(mp_obj_t self_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t dest[2];

    mp_load_method_maybe(self->bus_obj, MP_QSTR_on, dest);
    mp_call_method_self_n_kw(dest[0],
                             dest[1],
                             0,
                             0,
                             NULL);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(st7789_ST7789_backlight_on_obj, st7789_ST7789_backlight_on);


STATIC mp_obj_t st7789_ST7789_backlight_off(mp_obj_t self_in)
{
    st7789_ST7789_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t dest[2];

    mp_load_method_maybe(self->bus_obj, MP_QSTR_off, dest);
    mp_call_method_self_n_kw(dest[0],
                             dest[1],
                             0,
                             0,
                             NULL);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(st7789_ST7789_backlight_off_obj, st7789_ST7789_backlight_off);


STATIC const mp_rom_map_elem_t st7789_ST7789_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&st7789_ST7789_deinit_obj)        },
    { MP_ROM_QSTR(MP_QSTR_reset),         MP_ROM_PTR(&st7789_ST7789_reset_obj)         },
    { MP_ROM_QSTR(MP_QSTR_init),          MP_ROM_PTR(&st7789_ST7789_init_obj)          },
    { MP_ROM_QSTR(MP_QSTR_bitmap),        MP_ROM_PTR(&st7789_ST7789_bitmap_obj)        },
    { MP_ROM_QSTR(MP_QSTR_mirror),        MP_ROM_PTR(&st7789_ST7789_mirror_obj)        },
    { MP_ROM_QSTR(MP_QSTR_swap_xy),       MP_ROM_PTR(&st7789_ST7789_swap_xy_obj)       },
    { MP_ROM_QSTR(MP_QSTR_set_gap),       MP_ROM_PTR(&st7789_ST7789_set_gap_obj)       },
    { MP_ROM_QSTR(MP_QSTR_invert_color),  MP_ROM_PTR(&st7789_ST7789_invert_color_obj)  },
    { MP_ROM_QSTR(MP_QSTR_disp_off),      MP_ROM_PTR(&st7789_ST7789_disp_off_obj)      },
    { MP_ROM_QSTR(MP_QSTR_backlight_on),  MP_ROM_PTR(&st7789_ST7789_backlight_on_obj)  },
    { MP_ROM_QSTR(MP_QSTR_backlight_off), MP_ROM_PTR(&st7789_ST7789_backlight_off_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),       MP_ROM_PTR(&st7789_ST7789_deinit_obj)        },

    { MP_ROM_QSTR(MP_QSTR_RGB),           MP_ROM_INT(COLOR_SPACE_RGB)                  },
    { MP_ROM_QSTR(MP_QSTR_BGR),           MP_ROM_INT(COLOR_SPACE_BGR)                  },
    { MP_ROM_QSTR(MP_QSTR_MONOCHROME),    MP_ROM_INT(COLOR_SPACE_MONOCHROME)           }
};
STATIC MP_DEFINE_CONST_DICT(st7789_ST7789_locals_dict, st7789_ST7789_locals_dict_table);

const mp_obj_type_t st7789_ST7789_type = {
    { &mp_type_type },
    .name        = MP_QSTR_ST7789,
    .print       = st7789_ST7789_print,
    .make_new    = st7789_ST7789_make_new,
    .locals_dict = (mp_obj_dict_t *)&st7789_ST7789_locals_dict,
};