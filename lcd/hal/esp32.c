#include "esp32.h"

#define DEBUG_printf(...)

void hal_lcd_i80_construct(lcd_i80_obj_t *self) {
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = mp_hal_get_pin_obj(self->dc),
        .wr_gpio_num = mp_hal_get_pin_obj(self->wr),
        .clk_src = LCD_CLK_SRC_PLL160M,
        .data_gpio_nums = {
            mp_hal_get_pin_obj(self->databus[0]),
            mp_hal_get_pin_obj(self->databus[1]),
            mp_hal_get_pin_obj(self->databus[2]),
            mp_hal_get_pin_obj(self->databus[3]),
            mp_hal_get_pin_obj(self->databus[4]),
            mp_hal_get_pin_obj(self->databus[5]),
            mp_hal_get_pin_obj(self->databus[6]),
            mp_hal_get_pin_obj(self->databus[7]),
        },
        .bus_width = self->bus_width,
        .max_transfer_bytes = self->width * self->height * sizeof(uint16_t)
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &self->i80_bus));

    esp_lcd_panel_io_i80_config_t io_config = {
        .pclk_hz = self->pclk,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .flags = {
            .swap_color_bytes = self->swap_color_bytes,
        },
        .lcd_cmd_bits = self->cmd_bits,
        .lcd_param_bits = self->param_bits,
    };
    if (self->cs != MP_OBJ_NULL) {
        io_config.cs_gpio_num = mp_hal_get_pin_obj(self->cs);
    } else {
        io_config.cs_gpio_num = -1;
    }
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(self->i80_bus, &io_config, &self->io_handle));
}


void hal_lcd_i80_tx_param(lcd_i80_obj_t *self, int lcd_cmd, const void *param, size_t param_size) {
    DEBUG_printf("tx_param cmd: %x, param_size: %u\n", lcd_cmd, param_size);
    esp_lcd_panel_io_tx_param(self->io_handle, lcd_cmd, param, param_size);
}


void hal_lcd_i80_tx_color(lcd_i80_obj_t *self, int lcd_cmd, const void *color, size_t color_size) {
    DEBUG_printf("tx_color cmd: %x, color_size: %u\n", lcd_cmd, color_size);
    esp_lcd_panel_io_tx_color(self->io_handle, lcd_cmd, color, color_size);
}


void hal_lcd_i80_deinit(lcd_i80_obj_t *self) {
    esp_lcd_panel_io_del(self->io_handle);
    esp_lcd_del_i80_bus(self->i80_bus);
}