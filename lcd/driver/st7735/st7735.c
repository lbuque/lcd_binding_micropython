#include "st7735.h"
#include "spi_panel.h"
#include "i80_panel.h"
#include "lcd_panel.h"
#include "lcd_panel_commands.h"
#include "lcd_panel_types.h"
#include "st7735_rotation.h"

#include "py/obj.h"
#include "py/runtime.h"
#include "mphalport.h"
#include "py/gc.h"

#include <string.h>

// this is the actual C-structure for our new object
typedef struct lcd_st7735_obj_t {
    mp_obj_base_t base;
    mp_obj_base_t *bus_obj;
    mp_lcd_panel_p_t *lcd_panel_p;
    mp_obj_t reset;
    mp_obj_t backlight;
    bool reset_level;
    uint8_t color_space;

    uint16_t width;
    uint16_t height;
    uint8_t rotation;
    lcd_panel_rotation_t rotations[4];   // list of rotation tuples
    int x_gap;
    int y_gap;
    uint32_t bpp;
    uint8_t fb_bpp;
    uint8_t madctl_val; // save current value of LCD_CMD_MADCTL register
    uint8_t colmod_cal; // save surrent value of LCD_CMD_COLMOD register
} lcd_st7735_obj_t;


STATIC void set_rotation(lcd_st7735_obj_t *self, uint8_t rotation)
{
    self->madctl_val |= self->rotations[rotation].madctl;

    // tx param
    if (self->lcd_panel_p) {
        self->lcd_panel_p->tx_param(
            self->bus_obj,
            0x36,
            (uint8_t[]) { self->madctl_val, },
            1
        );
    }

    self->width = self->rotations[rotation].width;
    self->height = self->rotations[rotation].height;
    self->x_gap = self->rotations[rotation].colstart;
    self->y_gap = self->rotations[rotation].rowstart;
}


STATIC void lcd_st7735_print(const mp_print_t *print,
                             mp_obj_t          self_in,
                             mp_print_kind_t   kind)
{
    (void) kind;
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(
        print,
        "<ST7735 bus=%p, reset=%p, color_space=%s, bpp=%u>",
        self->bus_obj,
        self->reset,
        color_space_desc[self->color_space],
        self->bpp
    );
}


mp_obj_t lcd_st7735_make_new(const mp_obj_type_t *type,
                             size_t               n_args,
                             size_t               n_kw,
                             const mp_obj_t      *all_args)
{
    enum {
        ARG_bus,
        ARG_reset,
        ARG_backlight,
        ARG_reset_level,
        ARG_color_space,
        ARG_bpp
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_bus,            MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_obj = MP_OBJ_NULL}     },
        { MP_QSTR_reset,          MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL}     },
        { MP_QSTR_backlight,      MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL}     },
        { MP_QSTR_reset_level,    MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false}          },
        { MP_QSTR_color_space,    MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = COLOR_SPACE_RGB} },
        { MP_QSTR_bpp,            MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 16}              },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(
        n_args,
        n_kw,
        all_args,
        MP_ARRAY_SIZE(allowed_args),
        allowed_args,
        args
    );

    // create new object
    lcd_st7735_obj_t *self = m_new_obj(lcd_st7735_obj_t);
    self->base.type = &lcd_st7735_type;

    self->bus_obj = (mp_obj_base_t *)MP_OBJ_TO_PTR(args[ARG_bus].u_obj);
    self->lcd_panel_p = (mp_lcd_panel_p_t *)self->bus_obj->type->protocol;

    if (mp_obj_is_type(self->bus_obj, &lcd_spi_panel_type)) {
        self->width = ((lcd_spi_panel_obj_t *)self->bus_obj)->width;
        self->height = ((lcd_spi_panel_obj_t *)self->bus_obj)->height;
    } else if (mp_obj_is_type(self->bus_obj, &lcd_i80_type)) {
        self->width = ((lcd_i80_obj_t *)self->bus_obj)->width;
        self->height = ((lcd_i80_obj_t *)self->bus_obj)->height;
    }

    self->reset       = args[ARG_reset].u_obj;
    self->backlight   = args[ARG_backlight].u_obj;
    self->reset_level = args[ARG_reset_level].u_bool;
    self->color_space = args[ARG_color_space].u_int;
    self->bpp         = args[ARG_bpp].u_int;

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

    switch (self->color_space) {
        case COLOR_SPACE_RGB:
            self->madctl_val = 0;
        break;

        case COLOR_SPACE_BGR:
            self->madctl_val |= (1 << 3);
        break;

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported color space"));
        break;
    }

    switch (self->bpp) {
        case 16:
            self->colmod_cal = 0x55;
            self->fb_bpp = 16;
        break;

        case 18:
            self->colmod_cal = 0x66;
            self->fb_bpp = 24;
        break;

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported pixel width"));
        break;
    }

    bzero(&self->rotations, sizeof(self->rotations));
    if ((self->width == 160 && self->height == 80) || \
        (self->width == 80 && self->height == 160)) {
        memcpy(&self->rotations, ORIENTATIONS_160x80, sizeof(ORIENTATIONS_160x80));
    } else {
        mp_warning(NULL, "rotation parameter not detected");
        mp_warning(NULL, "use default rotation parameters");
        memcpy(&self->rotations, ORIENTATIONS_GENERAL, sizeof(ORIENTATIONS_GENERAL));
        self->rotations[0].width = self->width;
        self->rotations[0].height = self->height;
        self->rotations[1].width = self->height;
        self->rotations[1].height = self->width;
        self->rotations[2].width = self->width;
        self->rotations[2].height = self->height;
        self->rotations[3].width = self->height;
        self->rotations[3].height = self->width;
    }
    set_rotation(self, 0);

    return MP_OBJ_FROM_PTR(self);
}


STATIC mp_obj_t lcd_st7735_deinit(mp_obj_t self_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->lcd_panel_p) {
        self->lcd_panel_p->deinit(self->bus_obj);
    }

    // m_del_obj(lcd_st7735_obj_t, self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7735_deinit_obj, lcd_st7735_deinit);


STATIC mp_obj_t lcd_st7735_reset(mp_obj_t self_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->reset != MP_OBJ_NULL) {
        mp_hal_pin_obj_t reset_pin = mp_hal_get_pin_obj(self->reset);
        mp_hal_pin_write(reset_pin, self->reset_level);
        mp_hal_delay_us(10 * 1000);
        mp_hal_pin_write(reset_pin, !self->reset_level);
        mp_hal_delay_us(10 * 1000);
    } else {
        if (self->lcd_panel_p) {
            self->lcd_panel_p->tx_param(self->bus_obj, 0x01, NULL, 0);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7735_reset_obj, lcd_st7735_reset);


STATIC mp_obj_t lcd_st7735_init(mp_obj_t self_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->lcd_panel_p) {
        self->lcd_panel_p->tx_param(self->bus_obj, 0x11, NULL, 0);
        mp_hal_delay_us(100 * 1000);

        self->lcd_panel_p->tx_param(self->bus_obj, 0x36, (uint8_t[]) {
            self->madctl_val,
        }, 1);

        self->lcd_panel_p->tx_param(self->bus_obj, 0x3A, (uint8_t[]) {
            self->colmod_cal,
        }, 1);

        // turn on display
        self->lcd_panel_p->tx_param(self->bus_obj, 0x29, NULL, 0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7735_init_obj, lcd_st7735_init);


STATIC mp_obj_t lcd_st7735_custom_init(mp_obj_t self_in, mp_obj_t cmd_list_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t id = 0;
    mp_buffer_info_t args;
    mp_int_t delay = 0;

    mp_obj_tuple_t *cmd_list = MP_OBJ_TO_PTR(cmd_list_in);
    for (size_t i = 0; i < cmd_list->len; i++) {
        mp_obj_tuple_t *cmd = MP_OBJ_TO_PTR(cmd_list->items[i]);
        id = mp_obj_get_int(cmd->items[0]);
        mp_get_buffer_raise(cmd->items[1], &args, MP_BUFFER_READ);
        delay = mp_obj_get_int(cmd->items[2]);

        self->lcd_panel_p->tx_param(self->bus_obj, id, args.buf, args.len);
        if (delay != 0) {
            mp_hal_delay_us(delay * 1000);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_st7735_custom_init_obj, lcd_st7735_custom_init);


STATIC mp_obj_t lcd_st7735_bitmap(size_t n_args, const mp_obj_t *args_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);

    int x_start = mp_obj_get_int(args_in[1]);
    int y_start = mp_obj_get_int(args_in[2]);
    int x_end   = mp_obj_get_int(args_in[3]);
    int y_end   = mp_obj_get_int(args_in[4]);

    x_start += self->x_gap;
    x_end += self->x_gap;
    y_start += self->y_gap;
    y_end += self->y_gap;

    if (self->lcd_panel_p) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args_in[5], &bufinfo, MP_BUFFER_READ);
        self->lcd_panel_p->tx_param(self->bus_obj, 0x2A, (uint8_t[]) {
            ((x_start >> 8) & 0xFF),
            (x_start & 0xFF),
            (((x_end - 1) >> 8) & 0xFF),
            ((x_end - 1) & 0xFF),
        }, 4);
        self->lcd_panel_p->tx_param(self->bus_obj, 0x2B, (uint8_t[]) {
            ((y_start >> 8) & 0xFF),
            (y_start & 0xFF),
            (((y_end - 1) >> 8) & 0xFF),
            ((y_end - 1) & 0xFF),
        }, 4);
        size_t len = ((x_end - x_start) * (y_end - y_start) * self->fb_bpp / 8);
        self->lcd_panel_p->tx_color(self->bus_obj, 0x2C, bufinfo.buf, len);
    }

    gc_collect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_st7735_bitmap_obj, 6, 6, lcd_st7735_bitmap);


STATIC mp_obj_t lcd_st7735_mirror(mp_obj_t self_in,
                                  mp_obj_t mirror_x_in,
                                  mp_obj_t mirror_y_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

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

    if (self->lcd_panel_p) {
        self->lcd_panel_p->tx_param(self->bus_obj, 0x36, (uint8_t[]) {
            self->madctl_val
        }, 1);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lcd_st7735_mirror_obj, lcd_st7735_mirror);


STATIC mp_obj_t lcd_st7735_swap_xy(mp_obj_t self_in, mp_obj_t swap_axes_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (mp_obj_is_true(swap_axes_in)) {
        self->madctl_val |= 1 << 5;
    } else {
        self->madctl_val &= ~(1 << 5);
    }

    if (self->lcd_panel_p) {
        self->lcd_panel_p->tx_param(self->bus_obj, 0x36, (uint8_t[]) {
            self->madctl_val
        }, 1);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_st7735_swap_xy_obj, lcd_st7735_swap_xy);


STATIC mp_obj_t lcd_st7735_set_gap(mp_obj_t self_in,
                                   mp_obj_t x_gap_in,
                                   mp_obj_t y_gap_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    self->x_gap = mp_obj_get_int(x_gap_in);
    self->y_gap = mp_obj_get_int(y_gap_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lcd_st7735_set_gap_obj, lcd_st7735_set_gap);


STATIC mp_obj_t lcd_st7735_invert_color(mp_obj_t self_in, mp_obj_t invert_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (mp_obj_is_true(invert_in)) {
        if (self->lcd_panel_p) {
            self->lcd_panel_p->tx_param(self->bus_obj, 0x21, NULL, 0);
        }
    } else {
        if (self->lcd_panel_p) {
            self->lcd_panel_p->tx_param(self->bus_obj, 0x20, NULL, 0);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_st7735_invert_color_obj, lcd_st7735_invert_color);


STATIC mp_obj_t lcd_st7735_disp_off(mp_obj_t self_in, mp_obj_t off_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (mp_obj_is_true(off_in)) {
        if (self->lcd_panel_p) {
            self->lcd_panel_p->tx_param(self->bus_obj, 0x28, NULL, 0);
        }
    } else {
        if (self->lcd_panel_p) {
            self->lcd_panel_p->tx_param(self->bus_obj, 0x29, NULL, 0);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lcd_st7735_disp_off_obj, lcd_st7735_disp_off);


STATIC mp_obj_t lcd_st7735_backlight_on(mp_obj_t self_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->backlight != MP_OBJ_NULL) {
        mp_hal_pin_obj_t backlight_pin = mp_hal_get_pin_obj(self->backlight);
        mp_hal_pin_write(backlight_pin, 1);
        mp_hal_delay_us(10 * 1000);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7735_backlight_on_obj, lcd_st7735_backlight_on);


STATIC mp_obj_t lcd_st7735_backlight_off(mp_obj_t self_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->backlight != MP_OBJ_NULL) {
        mp_hal_pin_obj_t backlight_pin = mp_hal_get_pin_obj(self->backlight);
        mp_hal_pin_write(backlight_pin, 0);
        mp_hal_delay_us(10 * 1000);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7735_backlight_off_obj, lcd_st7735_backlight_off);


STATIC mp_obj_t lcd_st7735_color565(size_t n_args, const mp_obj_t *args_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_st7735_color565_obj, 4, 4, lcd_st7735_color565);


STATIC mp_obj_t lcd_st7735_width(mp_obj_t self_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->width);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7735_width_obj, lcd_st7735_width);


STATIC mp_obj_t lcd_st7735_height(mp_obj_t self_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->height);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_st7735_height_obj, lcd_st7735_height);


STATIC mp_obj_t lcd_st7735_rotation(size_t n_args, const mp_obj_t *args_in)
{
    lcd_st7735_obj_t *self = MP_OBJ_TO_PTR(args_in[0]);
    self->rotation = mp_obj_get_int(args_in[1]) % 4;
    if (n_args > 2) {
        mp_obj_tuple_t *rotations_in = MP_OBJ_TO_PTR(args_in[2]);
        for (size_t i = 0; i < rotations_in->len; i++) {
            if (i < 4) {
                mp_obj_tuple_t *item = MP_OBJ_TO_PTR(rotations_in->items[i]);
                self->rotations[i].madctl   = mp_obj_get_int(item->items[0]);
                self->rotations[i].width    = mp_obj_get_int(item->items[1]);
                self->rotations[i].height   = mp_obj_get_int(item->items[2]);
                self->rotations[i].colstart = mp_obj_get_int(item->items[3]);
                self->rotations[i].rowstart = mp_obj_get_int(item->items[4]);
            }
        }
    }
    set_rotation(self, self->rotation);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_st7735_rotation_obj, 2, 3, lcd_st7735_rotation);


STATIC const mp_rom_map_elem_t lcd_st7735_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),        MP_ROM_PTR(&lcd_st7735_deinit_obj)        },
    { MP_ROM_QSTR(MP_QSTR_reset),         MP_ROM_PTR(&lcd_st7735_reset_obj)         },
    { MP_ROM_QSTR(MP_QSTR_init),          MP_ROM_PTR(&lcd_st7735_init_obj)          },
    { MP_ROM_QSTR(MP_QSTR_custom_init),   MP_ROM_PTR(&lcd_st7735_custom_init_obj)   },
    { MP_ROM_QSTR(MP_QSTR_bitmap),        MP_ROM_PTR(&lcd_st7735_bitmap_obj)        },
    { MP_ROM_QSTR(MP_QSTR_mirror),        MP_ROM_PTR(&lcd_st7735_mirror_obj)        },
    { MP_ROM_QSTR(MP_QSTR_swap_xy),       MP_ROM_PTR(&lcd_st7735_swap_xy_obj)       },
    { MP_ROM_QSTR(MP_QSTR_set_gap),       MP_ROM_PTR(&lcd_st7735_set_gap_obj)       },
    { MP_ROM_QSTR(MP_QSTR_invert_color),  MP_ROM_PTR(&lcd_st7735_invert_color_obj)  },
    { MP_ROM_QSTR(MP_QSTR_disp_off),      MP_ROM_PTR(&lcd_st7735_disp_off_obj)      },
    { MP_ROM_QSTR(MP_QSTR_backlight_on),  MP_ROM_PTR(&lcd_st7735_backlight_on_obj)  },
    { MP_ROM_QSTR(MP_QSTR_backlight_off), MP_ROM_PTR(&lcd_st7735_backlight_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_color565),      MP_ROM_PTR(&lcd_st7735_color565_obj)      },
    { MP_ROM_QSTR(MP_QSTR_height),        MP_ROM_PTR(&lcd_st7735_height_obj)        },
    { MP_ROM_QSTR(MP_QSTR_width),         MP_ROM_PTR(&lcd_st7735_width_obj)         },
    { MP_ROM_QSTR(MP_QSTR_rotation),      MP_ROM_PTR(&lcd_st7735_rotation_obj)      },
    { MP_ROM_QSTR(MP_QSTR___del__),       MP_ROM_PTR(&lcd_st7735_deinit_obj)        },

    { MP_ROM_QSTR(MP_QSTR_RGB),           MP_ROM_INT(COLOR_SPACE_RGB)               },
    { MP_ROM_QSTR(MP_QSTR_BGR),           MP_ROM_INT(COLOR_SPACE_BGR)               },
    { MP_ROM_QSTR(MP_QSTR_MONOCHROME),    MP_ROM_INT(COLOR_SPACE_MONOCHROME)        },
};
STATIC MP_DEFINE_CONST_DICT(lcd_st7735_locals_dict, lcd_st7735_locals_dict_table);


const mp_obj_type_t lcd_st7735_type = {
    { &mp_type_type },
    .name        = MP_QSTR_ST7735,
    .print       = lcd_st7735_print,
    .make_new    = lcd_st7735_make_new,
    .locals_dict = (mp_obj_dict_t *)&lcd_st7735_locals_dict,
};
