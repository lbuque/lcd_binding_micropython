#ifndef LCD_QSPI_PANEL_H_
#define LCD_QSPI_PANEL_H_

#include "mphalport.h"
#include "py/obj.h"
#if USE_ESP_LCD
#include "esp_lcd_panel_io.h"
#include "driver/spi_master.h"
#endif

typedef struct lcd_qspi_panel_obj_t {
    mp_obj_base_t base;
    mp_obj_base_t *spi_obj;
    uint16_t width;
    uint16_t height;

    mp_hal_pin_obj_t databus_pins[4];
    mp_hal_pin_obj_t dc_pin;
    mp_hal_pin_obj_t wr_pin;
    mp_hal_pin_obj_t cs_pin;

    mp_obj_t databus[4];
    mp_obj_t dc;
    mp_obj_t wr;
    mp_obj_t cs;

    uint32_t pclk;
    int cmd_bits;
    int param_bits;
    bool swap_color_bytes;
#if USE_ESP_LCD
    spi_device_handle_t io_handle;
#else
    void (*write_color)(mp_hal_pin_obj_t *databus, mp_hal_pin_obj_t wr, const uint8_t *buf, int len);
#endif
} lcd_qspi_panel_obj_t;

extern const mp_obj_type_t lcd_qspi_panel_type;

#endif