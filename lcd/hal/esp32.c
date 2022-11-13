#include "esp32.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"

#include "py/runtime.h"

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
    esp_err_t ret = esp_lcd_new_i80_bus(&bus_config, &self->i80_bus);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_new_i80_bus)", ret);
    }
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
    ret = esp_lcd_new_panel_io_i80(self->i80_bus, &io_config, &self->io_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_new_panel_io_i80)", ret);
    }
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


void hal_lcd_rgb_construct(lcd_rgb_obj_t *self) {
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_PLL160M,
        .timings = {
            .pclk_hz = self->pclk,
            .h_res = self->width,
            .v_res = self->height,
            // The following parameters should refer to LCD spec
            .hsync_pulse_width = self->timings.hsync_pulse_width,
            .hsync_back_porch  = self->timings.hsync_back_porch,
            .hsync_front_porch = self->timings.hsync_front_porch,
            .vsync_pulse_width = self->timings.vsync_pulse_width,
            .vsync_back_porch  = self->timings.vsync_back_porch,
            .vsync_front_porch = self->timings.vsync_front_porch,
            .flags = {
                .pclk_active_neg = 1,
            },
        },
        .data_width = self->bus_width, // RGB565 in parallel mode, thus 16bit in width
        .psram_trans_align = 64,
        .hsync_gpio_num = mp_hal_get_pin_obj(self->hsync_obj),
        .vsync_gpio_num = mp_hal_get_pin_obj(self->vsync_obj),
        .de_gpio_num = mp_hal_get_pin_obj(self->de_obj),
        .pclk_gpio_num  = mp_hal_get_pin_obj(self->pclk_obj),
        .data_gpio_nums = {
            mp_hal_get_pin_obj(self->databus_obj[0]),
            mp_hal_get_pin_obj(self->databus_obj[1]),
            mp_hal_get_pin_obj(self->databus_obj[2]),
            mp_hal_get_pin_obj(self->databus_obj[3]),
            mp_hal_get_pin_obj(self->databus_obj[4]),
            mp_hal_get_pin_obj(self->databus_obj[5]),
            mp_hal_get_pin_obj(self->databus_obj[6]),
            mp_hal_get_pin_obj(self->databus_obj[7]),
            mp_hal_get_pin_obj(self->databus_obj[8]),
            mp_hal_get_pin_obj(self->databus_obj[9]),
            mp_hal_get_pin_obj(self->databus_obj[10]),
            mp_hal_get_pin_obj(self->databus_obj[11]),
            mp_hal_get_pin_obj(self->databus_obj[12]),
            mp_hal_get_pin_obj(self->databus_obj[13]),
            mp_hal_get_pin_obj(self->databus_obj[14]),
            mp_hal_get_pin_obj(self->databus_obj[15]),
        },
        .disp_gpio_num = self->disp_pin,
        .on_frame_trans_done = NULL,
        .user_ctx = NULL,
        .flags = {
            .fb_in_psram = 1, // allocate frame buffer in PSRAM
        },
    };
    esp_err_t ret = esp_lcd_new_rgb_panel(&panel_config, &self->panel_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_new_rgb_panel)", ret);
    }
    ret = esp_lcd_panel_reset(self->panel_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_panel_reset)", ret);
    }
    ret = esp_lcd_panel_init(self->panel_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_panel_init)", ret);
    }
}


inline void hal_lcd_rgb_reset(lcd_rgb_obj_t *self) {
    esp_lcd_panel_reset(self->panel_handle);
}


inline void hal_lcd_rgb_init(lcd_rgb_obj_t *self) {
    esp_lcd_panel_init(self->panel_handle);
}


inline void hal_lcd_rgb_del(lcd_rgb_obj_t *self) {
    esp_lcd_panel_del(self->panel_handle);
}


inline void hal_lcd_rgb_bitmap(lcd_rgb_obj_t *self, int x_start, int y_start, int x_end, int y_end, const void *color_data) {
    esp_lcd_panel_draw_bitmap(self->panel_handle, x_start, y_start, x_end, y_end, color_data);
}


inline void hal_lcd_rgb_mirror(lcd_rgb_obj_t *self, bool mirror_x, bool mirror_y) {
    esp_lcd_panel_mirror(self->panel_handle, mirror_x, mirror_y);
}


inline void hal_lcd_rgb_swap_xy(lcd_rgb_obj_t *self, bool swap_axes) {
    esp_lcd_panel_swap_xy(self->panel_handle, swap_axes);
}


inline void hal_lcd_rgb_set_gap(lcd_rgb_obj_t *self, int x_gap, int y_gap) {
    esp_lcd_panel_set_gap(self->panel_handle, x_gap, y_gap);
}


inline void hal_lcd_rgb_invert_color(lcd_rgb_obj_t *self, bool invert_color_data) {
    esp_lcd_panel_invert_color(self->panel_handle, invert_color_data);
}


inline void hal_lcd_rgb_disp_off(lcd_rgb_obj_t *self, bool off) {
    esp_lcd_panel_disp_off(self->panel_handle, off);
}
