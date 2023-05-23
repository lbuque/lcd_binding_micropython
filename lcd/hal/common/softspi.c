#include "softspi.h"
#include "spi_panel.h"

#include "extmod/machine_spi.h"
#include "py/runtime.h"

#define DEBUG_printf(...) // mp_printf(&mp_plat_print, __VA_ARGS__);

#define CS_LOW()                                    \
{                                                   \
    if (spi_panel_obj->cs_pin) {                    \
        mp_hal_pin_write(spi_panel_obj->cs_pin, 0); \
    }                                               \
}

#define CS_HIGH()                                   \
{                                                   \
    if (spi_panel_obj->cs_pin) {                    \
        mp_hal_pin_write(spi_panel_obj->cs_pin, 1); \
    }                                               \
}

#define DC_LOW()                                    \
{                                                   \
    if (spi_panel_obj->dc_pin) {                    \
        mp_hal_pin_write(spi_panel_obj->dc_pin, 0); \
    }                                               \
}

#define DC_HIGH()                                   \
{                                                   \
    if (spi_panel_obj->dc_pin) {                    \
        mp_hal_pin_write(spi_panel_obj->dc_pin, 1); \
    }                                               \
}

void hal_lcd_softspi_panel_construct(mp_obj_base_t *self)
{
    mp_lcd_spi_panel_obj_t *spi_panel_obj = (mp_lcd_spi_panel_obj_t *)self;
    if (spi_panel_obj->dc != MP_OBJ_NULL) {
        spi_panel_obj->dc_pin = mp_hal_get_pin_obj(spi_panel_obj->dc);
        mp_hal_pin_output(spi_panel_obj->dc_pin);
    } else {
        spi_panel_obj->dc_pin = 0;
    }

    if (spi_panel_obj->cs != MP_OBJ_NULL) {
        spi_panel_obj->cs_pin = mp_hal_get_pin_obj(spi_panel_obj->cs);
        mp_hal_pin_output(spi_panel_obj->cs_pin);
    } else {
        spi_panel_obj->cs_pin = 0;
    }

    DC_HIGH();
    CS_HIGH();
}


void hal_lcd_softspi_panel_tx_param(mp_obj_base_t *self,
                                    int            lcd_cmd,
                                    const void    *param,
                                    size_t         param_size)
{
    DEBUG_printf("hal_lcd_qspi_panel_tx_param cmd: %x, param_size: %u\n", lcd_cmd, param_size);

    mp_lcd_spi_panel_obj_t *spi_panel_obj = (mp_lcd_spi_panel_obj_t *)self;
#ifdef MP_OBJ_TYPE_GET_SLOT
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *) MP_OBJ_TYPE_GET_SLOT(spi_panel_obj->spi_obj->type, protocol);
#else
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *) spi_panel_obj->spi_obj->type->protocol;
#endif

    CS_LOW()
    if (lcd_cmd != -1) {
        DC_LOW();
        spi_p->transfer(spi_panel_obj->spi_obj, 1, (uint8_t []) {lcd_cmd & 0xFF}, NULL);
    }
    if (param_size > 0) {
        DC_HIGH();
        spi_p->transfer(spi_panel_obj->spi_obj, param_size, param, NULL);
    }
    CS_HIGH()
}


void hal_lcd_softspi_panel_tx_color(mp_obj_base_t *self,
                                    int            lcd_cmd,
                                    const void    *color,
                                    size_t         color_size)
{
    DEBUG_printf("hal_lcd_softspi_panel_tx_color cmd: %x, color_size: %u\n", lcd_cmd, color_size);

    mp_lcd_spi_panel_obj_t *spi_panel_obj = (mp_lcd_spi_panel_obj_t *)self;
#ifdef MP_OBJ_TYPE_GET_SLOT
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *) MP_OBJ_TYPE_GET_SLOT(spi_panel_obj->spi_obj->type, protocol);
#else
    mp_machine_spi_p_t *spi_p = (mp_machine_spi_p_t *) spi_panel_obj->spi_obj->type->protocol;
#endif

    CS_LOW()
    if (lcd_cmd != -1) {
        DC_LOW();
        spi_p->transfer(spi_panel_obj->spi_obj, 1, (uint8_t []) {lcd_cmd & 0xFF}, NULL);
    }
    if (color_size > 0) {
        DC_HIGH();
        spi_p->transfer(spi_panel_obj->spi_obj, color_size, color, NULL);
    }
    CS_HIGH()
}


void hal_lcd_softspi_panel_deinit(mp_obj_base_t *self)
{

}
