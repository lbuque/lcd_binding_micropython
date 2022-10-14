#include "esp32.h"
// #include "girl.h"
// #include <string.h>

void hal_parallelbus_i80_construct(parallelbus_i80_obj_t *self) {
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

#if 0
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = 5,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(self->io_handle, &panel_config, &panel_handle));

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);
    esp_lcd_panel_invert_color(panel_handle, true);

    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, false, true);
    esp_lcd_panel_set_gap(panel_handle, 0, 35);

    uint16_t *s_lines = (uint16_t *)heap_caps_malloc(170 * 320 * sizeof(uint16_t), MALLOC_CAP_DMA);
    
    memcpy((uint8_t *)s_lines, gImage_girl, 108800);
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, 320, 170, s_lines);
    mp_hal_delay_ms(1000);
    free(s_lines);
    // uint16_t *s_lines = (uint16_t *)heap_caps_malloc(170 * 320 * sizeof(uint16_t), MALLOC_CAP_DMA);

    // for (size_t i = 0; i < 170 * 320; i++)
    // {
    //     s_lines[i] = 0xF800;
    // }

    // int x_start = 0;
    // int y_start = 0;
    // int x_end   = 200;
    // int y_end   = 100;

    // x_start += 0;
    // x_end += 0;
    // y_start += 35;
    // y_end += 35;

    // esp_lcd_panel_io_tx_param(self->io_handle, 0x2A, (uint8_t[]) {
    //     (x_start >> 8) & 0xFF,
    //     x_start & 0xFF,
    //     ((x_end - 1) >> 8) & 0xFF,
    //     (x_end - 1) & 0xFF,
    // }, 4);

    // esp_lcd_panel_io_tx_param(self->io_handle, 0x2B, (uint8_t[]) {
    //     (y_start >> 8) & 0xFF,
    //     y_start & 0xFF,
    //     ((y_end - 1) >> 8) & 0xFF,
    //     (y_end - 1) & 0xFF,
    // }, 4);

    // esp_lcd_panel_io_tx_color(self->io_handle, 0x2C, s_lines, ((x_end - x_start) * (y_end - y_start) * 16 / 8));
    mp_hal_delay_ms(10000);
    // vTaskDelay(10000 / portTICK_PERIOD_MS);
#endif
}

void hal_parallelbus_i80_tx_param(parallelbus_i80_obj_t *self, int lcd_cmd, const void *param, size_t param_size) {
    mp_printf(&mp_plat_print, "tx_param cmd: %x, param_size: %u\n", lcd_cmd, param_size);
    esp_lcd_panel_io_tx_param(self->io_handle, lcd_cmd, param, param_size);
}

void hal_parallelbus_i80_tx_color(parallelbus_i80_obj_t *self, int lcd_cmd, const void *color, size_t color_size) {
    mp_printf(&mp_plat_print, "tx_color cmd: %x, color_size: %u\n", lcd_cmd, color_size);
    esp_lcd_panel_io_tx_color(self->io_handle, lcd_cmd, color, color_size);
}

void hal_parallelbus_i80_deinit(parallelbus_i80_obj_t *self) {
    esp_lcd_panel_io_del(self->io_handle);
    esp_lcd_del_i80_bus(self->i80_bus);
}