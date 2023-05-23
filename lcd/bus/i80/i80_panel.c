#include "i80_panel.h"
#include "lcd_panel.h"

#if USE_ESP_LCD
#include "hal/esp32/esp32.h"
#else
#include "hal/commom/soft8080.h"
#endif

#include "mphalport.h"

#include "py/obj.h"
#include "py/runtime.h"
#include "py/gc.h"

#include <string.h>


STATIC void lcd_i80_print(
    const mp_print_t *print,
    mp_obj_t self_in,
    mp_print_kind_t kind
) {
    (void) kind;
    lcd_i80_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(
        print,
        "<I8080 data=%p, dc=%p, write=%p, read=%p, cs=%p, width=%u, height=%u, cmd_bits=%u, param_bits=%u>",
        self->databus,
        self->dc,
        self->wr,
        self->rd,
        self->cs,
        self->width,
        self->height,
        self->cmd_bits,
        self->param_bits
    );
}


STATIC mp_obj_t lcd_i80_make_new(
    const mp_obj_type_t *type,
    size_t n_args,
    size_t n_kw,
    const mp_obj_t *all_args
) {
    enum {
        ARG_data,
        ARG_command,
        ARG_write,
        ARG_read,
        ARG_cs,
        ARG_pclk,
        ARG_width,
        ARG_height,
        ARG_swap_color_bytes,
        ARG_cmd_bits,
        ARG_param_bits
    };
    const mp_arg_t make_new_args[] = {
        { MP_QSTR_data,             MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED        },
        { MP_QSTR_command,          MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED        },
        { MP_QSTR_write,            MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED        },
        { MP_QSTR_read,             MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_cs,               MP_ARG_OBJ | MP_ARG_KW_ONLY,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_pclk,             MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 10000000   } },
        { MP_QSTR_width,            MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 240        } },
        { MP_QSTR_height,           MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 240        } },
        { MP_QSTR_swap_color_bytes, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false     } },
        { MP_QSTR_cmd_bits,         MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 8          } },
        { MP_QSTR_param_bits,       MP_ARG_INT | MP_ARG_KW_ONLY,  {.u_int = 8          } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(make_new_args)];
    mp_arg_parse_all_kw_array(
        n_args, n_kw,
        all_args,
        MP_ARRAY_SIZE(make_new_args),
        make_new_args,
        args
    );

    // create new object
    lcd_i80_obj_t *self = m_new_obj(lcd_i80_obj_t);
    self->base.type = &lcd_i80_type;

    // data bus
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(args[ARG_data].u_obj);
    self->bus_width = t->len;
    for (size_t i = 0; i < t->len; i++) {
        self->databus[i] = t->items[i];
    }

    self->dc               = args[ARG_command].u_obj;
    self->wr               = args[ARG_write].u_obj;
    self->rd               = args[ARG_read].u_obj;
    self->cs               = args[ARG_cs].u_obj;
    self->pclk             = args[ARG_pclk].u_int;
    self->width            = args[ARG_width].u_int;
    self->height           = args[ARG_height].u_int;
    self->swap_color_bytes = args[ARG_swap_color_bytes].u_bool;
    self->cmd_bits         = args[ARG_cmd_bits].u_int;
    self->param_bits       = args[ARG_param_bits].u_int;

    if (self->rd != MP_OBJ_NULL) {
        mp_hal_pin_obj_t rd_pin = mp_hal_get_pin_obj(self->rd);
        mp_hal_pin_output(rd_pin);
        mp_hal_pin_write(rd_pin, 1);
    }

    hal_lcd_i80_construct(&self->base);
    return MP_OBJ_FROM_PTR(self);
}


STATIC mp_obj_t lcd_i80_tx_param(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args_in[0]);
    int cmd = mp_obj_get_int(args_in[1]);
    if (n_args == 3) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args_in[2], &bufinfo, MP_BUFFER_READ);
        hal_lcd_i80_tx_param(self, cmd, bufinfo.buf, bufinfo.len);
    } else {
        hal_lcd_i80_tx_param(self, cmd, NULL, 0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_i80_tx_param_obj, 2, 3, lcd_i80_tx_param);


STATIC mp_obj_t lcd_i80_tx_color(size_t n_args, const mp_obj_t *args_in) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(args_in[0]);
    int cmd = mp_obj_get_int(args_in[1]);

    if (n_args == 3) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args_in[2], &bufinfo, MP_BUFFER_READ);
        hal_lcd_i80_tx_color(self, cmd, bufinfo.buf, bufinfo.len);
    } else {
        hal_lcd_i80_tx_color(self, cmd, NULL, 0);
    }

    gc_collect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lcd_i80_tx_color_obj, 2, 3, lcd_i80_tx_color);


STATIC mp_obj_t lcd_i80_deinit(mp_obj_t self_in) {
    mp_obj_base_t *self = (mp_obj_base_t *)MP_OBJ_TO_PTR(self_in);

    hal_lcd_i80_deinit(self);
    // m_del_obj(lcd_i80_obj_t, self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lcd_i80_deinit_obj, lcd_i80_deinit);


STATIC const mp_rom_map_elem_t lcd_i80_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_tx_param), MP_ROM_PTR(&lcd_i80_tx_param_obj) },
    { MP_ROM_QSTR(MP_QSTR_tx_color), MP_ROM_PTR(&lcd_i80_tx_color_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),   MP_ROM_PTR(&lcd_i80_deinit_obj)   },
    { MP_ROM_QSTR(MP_QSTR___del__),  MP_ROM_PTR(&lcd_i80_deinit_obj)   },
};
STATIC MP_DEFINE_CONST_DICT(lcd_i80_locals_dict, lcd_i80_locals_dict_table);


STATIC const mp_lcd_panel_p_t mp_lcd_panel_p = {
    .tx_param = hal_lcd_i80_tx_param,
    .tx_color = hal_lcd_i80_tx_color,
    .deinit = hal_lcd_i80_deinit
};


const mp_obj_type_t lcd_i80_type = {
    { &mp_type_type },
    .name = MP_QSTR_I8080,
    .print = lcd_i80_print,
    .make_new = lcd_i80_make_new,
    .protocol = &mp_lcd_panel_p,
    .locals_dict = (mp_obj_dict_t *)&lcd_i80_locals_dict,
};