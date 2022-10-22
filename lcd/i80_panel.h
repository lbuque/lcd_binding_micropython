#ifndef LCD_I80_H_
#define LCD_I80_H_

#include "py/obj.h"
#include "py/mphal.h"
#if USE_ESP_LCD
#include "esp_lcd_panel_io.h"
#endif

typedef struct lcd_i80_obj_t {
    mp_obj_base_t base;

    uint16_t width;
    uint16_t height;

    mp_obj_t databus[8];
    mp_obj_t dc;
    mp_obj_t wr;
    mp_obj_t rd;
    mp_obj_t reset;
    mp_obj_t cs;
    // mp_obj_t backlight;

    mp_hal_pin_obj_t databus_pins[8];
    mp_hal_pin_obj_t dc_pin;
    mp_hal_pin_obj_t wr_pin;
    mp_hal_pin_obj_t rd_pin;
    mp_hal_pin_obj_t reset_pin;
    mp_hal_pin_obj_t cs_pin;
    mp_hal_pin_obj_t backlight_pin;

    uint32_t pclk;
    size_t bus_width;
    bool swap_color_bytes;
    int cmd_bits;
    int param_bits;
#if CONFIG_IDF_TARGET_ESP32S3
    esp_lcd_i80_bus_handle_t i80_bus;
    esp_lcd_panel_io_handle_t io_handle;
#endif
} lcd_i80_obj_t;

extern const mp_obj_type_t lcd_i80_type;

#endif