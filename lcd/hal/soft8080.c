#include "soft8080.h"

#define DEBUG_printf(...)

#define CS_LOW()                               \
    {                                          \
        if (self->cs_pin != -1) {              \
            mp_hal_pin_write(self->cs_pin, 0); \
        }                                      \
    }

#define CS_HIGH()                              \
    {                                          \
        if (self->cs_pin != -1) {              \
            mp_hal_pin_write(self->cs_pin, 1); \
        }                                      \
    }

#define DC_LOW()  (mp_hal_pin_write(self->dc_pin, 0))
#define DC_HIGH() (mp_hal_pin_write(self->dc_pin, 1))

#define WR_LOW()  (mp_hal_pin_write(self->wr_pin, 0))
#define WR_HIGH() (mp_hal_pin_write(self->wr_pin, 1))

STATIC void write_bus(lcd_i80_obj_t *self, const uint8_t *buf, int len)
{
    static int last = 0;
    uint8_t b;

    for (int i = 0; i < len; i++) {
        b = buf[i];

        if (b != last) {
#if 1
            mp_hal_pin_write(self->databus_pins[7], (b >> 7) & 1);
            mp_hal_pin_write(self->databus_pins[6], (b >> 6) & 1);
            mp_hal_pin_write(self->databus_pins[5], (b >> 5) & 1);
            mp_hal_pin_write(self->databus_pins[4], (b >> 4) & 1);
            mp_hal_pin_write(self->databus_pins[3], (b >> 3) & 1);
            mp_hal_pin_write(self->databus_pins[2], (b >> 2) & 1);
            mp_hal_pin_write(self->databus_pins[1], (b >> 1) & 1);
            mp_hal_pin_write(self->databus_pins[0], b & 1);
#else
            mp_hal_pin_write(self->databus_pins[0], (b >> 7) & 1);
            mp_hal_pin_write(self->databus_pins[1], (b >> 6) & 1);
            mp_hal_pin_write(self->databus_pins[2], (b >> 5) & 1);
            mp_hal_pin_write(self->databus_pins[3], (b >> 4) & 1);
            mp_hal_pin_write(self->databus_pins[4], (b >> 3) & 1);
            mp_hal_pin_write(self->databus_pins[5], (b >> 2) & 1);
            mp_hal_pin_write(self->databus_pins[6], (b >> 1) & 1);
            mp_hal_pin_write(self->databus_pins[7], b & 1);
#endif
            last = b;
        }
        WR_LOW();
        WR_HIGH();
    }
}


STATIC void write_color(lcd_i80_obj_t *self, const uint8_t *buf, int len)
{
    static int last = 0;
    uint8_t b;

    for (int i = 0; i < len / 2; i++) {
        if (self->swap_color_bytes) {
            b = buf[i * 2 + 1];
        } else {
            b = buf[i * 2];
        }

        if (b != last) {
#if 1
            mp_hal_pin_write(self->databus_pins[7], (b >> 7) & 1);
            mp_hal_pin_write(self->databus_pins[6], (b >> 6) & 1);
            mp_hal_pin_write(self->databus_pins[5], (b >> 5) & 1);
            mp_hal_pin_write(self->databus_pins[4], (b >> 4) & 1);
            mp_hal_pin_write(self->databus_pins[3], (b >> 3) & 1);
            mp_hal_pin_write(self->databus_pins[2], (b >> 2) & 1);
            mp_hal_pin_write(self->databus_pins[1], (b >> 1) & 1);
            mp_hal_pin_write(self->databus_pins[0], b & 1);
#else
            mp_hal_pin_write(self->databus_pins[0], (b >> 7) & 1);
            mp_hal_pin_write(self->databus_pins[1], (b >> 6) & 1);
            mp_hal_pin_write(self->databus_pins[2], (b >> 5) & 1);
            mp_hal_pin_write(self->databus_pins[3], (b >> 4) & 1);
            mp_hal_pin_write(self->databus_pins[4], (b >> 3) & 1);
            mp_hal_pin_write(self->databus_pins[5], (b >> 2) & 1);
            mp_hal_pin_write(self->databus_pins[6], (b >> 1) & 1);
            mp_hal_pin_write(self->databus_pins[7], b & 1);
#endif
            last = b;
        }
        WR_LOW();
        WR_HIGH();

        if (self->swap_color_bytes) {
            b = buf[i * 2];
        } else {
            b = buf[i * 2 + 1];
        }

        if (b != last) {
#if 1
            mp_hal_pin_write(self->databus_pins[7], (b >> 7) & 1);
            mp_hal_pin_write(self->databus_pins[6], (b >> 6) & 1);
            mp_hal_pin_write(self->databus_pins[5], (b >> 5) & 1);
            mp_hal_pin_write(self->databus_pins[4], (b >> 4) & 1);
            mp_hal_pin_write(self->databus_pins[3], (b >> 3) & 1);
            mp_hal_pin_write(self->databus_pins[2], (b >> 2) & 1);
            mp_hal_pin_write(self->databus_pins[1], (b >> 1) & 1);
            mp_hal_pin_write(self->databus_pins[0], b & 1);
#else
            mp_hal_pin_write(self->databus_pins[0], (b >> 7) & 1);
            mp_hal_pin_write(self->databus_pins[1], (b >> 6) & 1);
            mp_hal_pin_write(self->databus_pins[2], (b >> 5) & 1);
            mp_hal_pin_write(self->databus_pins[3], (b >> 4) & 1);
            mp_hal_pin_write(self->databus_pins[4], (b >> 3) & 1);
            mp_hal_pin_write(self->databus_pins[5], (b >> 2) & 1);
            mp_hal_pin_write(self->databus_pins[6], (b >> 1) & 1);
            mp_hal_pin_write(self->databus_pins[7], b & 1);
#endif
            last = b;
        }
        WR_LOW();
        WR_HIGH();

    }
}


void hal_lcd_i80_construct(lcd_i80_obj_t *self) {
    for (size_t i = 0; i < self->bus_width; i++)
    {
        self->databus_pins[i] = mp_hal_get_pin_obj(self->databus[i]);
        mp_hal_pin_output(self->databus_pins[i]);
    }

    self->dc_pin = mp_hal_get_pin_obj(self->dc);
    mp_hal_pin_output(self->dc_pin);

    self->wr_pin = mp_hal_get_pin_obj(self->wr);
    mp_hal_pin_output(self->wr_pin);

    if (self->cs != MP_OBJ_NULL) {
        self->cs_pin = mp_hal_get_pin_obj(self->cs);
        mp_hal_pin_output(self->cs_pin);
    } else {
        self->cs_pin = -1;
    }

    CS_HIGH();
    DC_HIGH();
    WR_HIGH();
    // RD_HIGH();

    mp_hal_pin_write(self->databus_pins[0], 0);
    mp_hal_pin_write(self->databus_pins[1], 0);
    mp_hal_pin_write(self->databus_pins[2], 0);
    mp_hal_pin_write(self->databus_pins[3], 0);
    mp_hal_pin_write(self->databus_pins[4], 0);
    mp_hal_pin_write(self->databus_pins[5], 0);
    mp_hal_pin_write(self->databus_pins[6], 0);
    mp_hal_pin_write(self->databus_pins[7], 0);
}


void hal_lcd_i80_tx_param(lcd_i80_obj_t *self, int lcd_cmd, const void *param, size_t param_size) {
    DEBUG_printf("tx_param cmd: %x, param_size: %u\n", lcd_cmd, param_size);
    CS_LOW()
    if (lcd_cmd) {
        DC_LOW();
        write_bus(self, (uint8_t[]) {
            lcd_cmd & 0xFF
        }, 1);
        DC_HIGH();
    }
    if (param_size > 0) {
        DC_HIGH();
        write_bus(self, param, param_size);
    }
    CS_HIGH()
}


void hal_lcd_i80_tx_color(lcd_i80_obj_t *self, int lcd_cmd, const void *color, size_t color_size) {
    DEBUG_printf("tx_color cmd: %x, color_size: %u\n", lcd_cmd, color_size);
    CS_LOW()
    if (lcd_cmd) {
        DC_LOW();
        write_bus(self, (uint8_t[]) {
            lcd_cmd & 0xFF
        }, 1);
        DC_HIGH();
    }
    if (color_size > 0) {
        DC_HIGH();
        write_color(self, color, color_size);
    }
    CS_HIGH()
}


void hal_lcd_i80_deinit(lcd_i80_obj_t *self) {
    // esp_lcd_panel_io_del(self->io_handle);
    // esp_lcd_del_i80_bus(self->i80_bus);
}
