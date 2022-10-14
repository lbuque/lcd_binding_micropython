#include "py/obj.h"

#include "st7789.h"
#include "i80_panel.h"

STATIC const mp_map_elem_t mp_module_st7789_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_st7789) },
    { MP_ROM_QSTR(MP_QSTR_ST7789),   MP_ROM_PTR(&st7789_ST7789_type) },
    { MP_ROM_QSTR(MP_QSTR_I8080),    MP_ROM_PTR(&parallelbus_i80_type) },
};
STATIC MP_DEFINE_CONST_DICT(mp_module_st7789_globals, mp_module_st7789_globals_table);

const mp_obj_module_t mp_module_st7789 = {
    .base    = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&mp_module_st7789_globals,
};

#if MICROPY_VERSION >= 0x011300 // MicroPython 1.19 or later
MP_REGISTER_MODULE(MP_QSTR_st7789, mp_module_st7789);
#else
MP_REGISTER_MODULE(MP_QSTR_st7789, mp_module_st7789, MODULE_ST7789_ENABLE);
#endif
