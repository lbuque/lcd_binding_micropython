#ifndef LCD_SPI_PANEL_H_
#define LCD_SPI_PANEL_H_

#include "mphalport.h"
#include "py/obj.h"
#if USE_ESP_LCD
#include "esp_lcd_panel_io.h"
#endif

typedef struct lcd_spi_panel_obj_t {
    mp_obj_base_t base;

    mp_obj_base_t *spi_obj;
    uint16_t width;
    uint16_t height;

    mp_obj_t dc;
    mp_obj_t wr;
    mp_obj_t cs;

    uint32_t pclk;
    int cmd_bits;
    int param_bits;
    bool swap_color_bytes;
#if USE_ESP_LCD
    esp_lcd_panel_io_handle_t io_handle;
#else
    void (*write_color)(mp_hal_pin_obj_t *databus, mp_hal_pin_obj_t wr, const uint8_t *buf, int len);
#endif
} lcd_spi_panel_obj_t;

// spi panle protocol
typedef struct _mp_machine_spi_panel_p_t {
    void (*tx_param)(lcd_spi_panel_obj_t *self, int lcd_cmd, const void *param, size_t param_size);
    void (*tx_color)(lcd_spi_panel_obj_t *self, int lcd_cmd, const void *color, size_t color_size);
} mp_machine_spi_panel_p_t;

extern const mp_obj_type_t lcd_spi_panel_type;

#endif