#include "esp32.h"

#include "spi_panel.h"
#include "qspi_panel.h"
#include "i80_panel.h"
#if DPI_LCD_SUPPORTED
#include "dpi_panel.h"
#endif

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"

#include "machine_hw_spi.c"
#include "py/runtime.h"

#define DEBUG_printf(...) // mp_printf(&mp_plat_print, __VA_ARGS__);

void hal_lcd_spi_panel_construct(mp_obj_base_t *self) {
    lcd_spi_panel_obj_t *spi_panel_obj = (lcd_spi_panel_obj_t *)self;
    machine_hw_spi_obj_t *spi_obj = ((machine_hw_spi_obj_t *)spi_panel_obj->spi_obj);
    machine_hw_spi_obj_t old_self = *spi_obj;
    if (spi_obj->state == MACHINE_HW_SPI_STATE_INIT) {
        spi_obj->state = MACHINE_HW_SPI_STATE_DEINIT;
        machine_hw_spi_deinit_internal(&old_self);
    }
    
    spi_bus_config_t buscfg = {
        .sclk_io_num = spi_obj->sck,
        .mosi_io_num = spi_obj->mosi,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        // .max_transfer_sz = spi_panel_obj->width * spi_panel_obj->height * 2 + 8,
        .max_transfer_sz = 0x4000 * 2 + 8,
    };
    esp_err_t ret = spi_bus_initialize(spi_obj->host, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(spi_bus_initialize)", ret);
    }
    spi_obj->state = MACHINE_HW_SPI_STATE_INIT;

    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = mp_hal_get_pin_obj(spi_panel_obj->dc),
        .cs_gpio_num = mp_hal_get_pin_obj(spi_panel_obj->cs),
        .pclk_hz = spi_panel_obj->pclk,
        .lcd_cmd_bits = spi_panel_obj->cmd_bits,
        .lcd_param_bits = spi_panel_obj->param_bits,
        .spi_mode = spi_obj->phase | (spi_obj->polarity << 1),
        .flags.lsb_first = spi_obj->firstbit == MICROPY_PY_MACHINE_SPI_LSB ? SPI_DEVICE_TXBIT_LSBFIRST | SPI_DEVICE_RXBIT_LSBFIRST : 0,
        .trans_queue_depth = 10,
    };

    // Attach the LCD to the SPI bus
    ret = esp_lcd_new_panel_io_spi(
        (esp_lcd_spi_bus_handle_t)spi_obj->host,
        &io_config,
        &spi_panel_obj->io_handle
    );
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_new_panel_io_spi)", ret);
    }
    spi_panel_obj->state = LCD_SPI_PANEL_STATE_INIT;
}


inline void hal_lcd_spi_panel_tx_param(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *param,
    size_t param_size
) {
    DEBUG_printf("tx_param cmd: %x, param_size: %u\n", lcd_cmd, param_size);

    lcd_spi_panel_obj_t *spi_panel_obj = (lcd_spi_panel_obj_t *)self;
    esp_lcd_panel_io_tx_param(spi_panel_obj->io_handle, lcd_cmd, param, param_size);
}


inline void hal_lcd_spi_panel_tx_color(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *color,
    size_t color_size
) {
    DEBUG_printf("tx_color cmd: %x, color_size: %u\n", lcd_cmd, color_size);
    lcd_spi_panel_obj_t *spi_panel_obj = (lcd_spi_panel_obj_t *)self;

#if 1
    esp_lcd_panel_io_tx_color(spi_panel_obj->io_handle, lcd_cmd, color, color_size);
#else
    size_t i = 0;
    esp_lcd_panel_io_tx_param(spi_panel_obj->io_handle, lcd_cmd, NULL, 0);

    for (i = 0; i < (color_size / (spi_panel_obj->width * 2)); i++) {
        esp_lcd_panel_io_tx_color(
            spi_panel_obj->io_handle,
            -1,
            (const void *)&((const uint8_t *)color)[i * spi_panel_obj->width * 2],
            spi_panel_obj->width * 2
        );
    }
    esp_lcd_panel_io_tx_color(
        spi_panel_obj->io_handle,
        -1,
        (const void *)&((const uint8_t *)color)[i * spi_panel_obj->width * 2],
        color_size - (i * spi_panel_obj->width * 2)
    );
#endif
}


inline void hal_lcd_spi_panel_deinit(mp_obj_base_t *self) {
    lcd_spi_panel_obj_t *spi_panel_obj = (lcd_spi_panel_obj_t *)self;
    if (spi_panel_obj->state == LCD_SPI_PANEL_STATE_INIT) {
        spi_panel_obj->state = LCD_SPI_PANEL_STATE_DEINIT;
        esp_lcd_panel_io_del(spi_panel_obj->io_handle);

        // esp_lcd_panel_io_del has already executed the operation of spi_bus_remove_device,
        // so only need to execute spi_bus_free here.
        machine_hw_spi_obj_t *spi_obj = ((machine_hw_spi_obj_t *)spi_panel_obj->spi_obj);
        if (spi_obj->state == MACHINE_HW_SPI_STATE_INIT) {
            spi_obj->state = MACHINE_HW_SPI_STATE_DEINIT;

            switch (spi_bus_free(spi_obj->host)) {
                case ESP_ERR_INVALID_ARG:
                    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid configuration"));
                return;

                case ESP_ERR_INVALID_STATE:
                    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SPI bus already freed"));
                return;
            }

            int8_t pins[3] = {spi_obj->miso, spi_obj->mosi, spi_obj->sck};

            for (int i = 0; i < 3; i++) {
                if (pins[i] != -1) {
                    gpio_pad_select_gpio(pins[i]);
                    gpio_matrix_out(pins[i], SIG_GPIO_OUT_IDX, false, false);
                    gpio_set_direction(pins[i], GPIO_MODE_INPUT);
                }
            }
        }
    }
}


// qspi
void hal_lcd_qspi_panel_construct(mp_obj_base_t *self) {
    lcd_qspi_panel_obj_t *qspi_panel_obj = (lcd_qspi_panel_obj_t *)self;
    machine_hw_spi_obj_t *spi_obj = ((machine_hw_spi_obj_t *)qspi_panel_obj->spi_obj);
    machine_hw_spi_obj_t old_spi_obj = *spi_obj;
    if (spi_obj->state == MACHINE_HW_SPI_STATE_INIT) {
        spi_obj->state = MACHINE_HW_SPI_STATE_DEINIT;
        machine_hw_spi_deinit_internal(&old_spi_obj);
    }

    mp_hal_pin_output(qspi_panel_obj->cs_pin);
    mp_hal_pin_od_high(qspi_panel_obj->cs_pin);

    spi_bus_config_t buscfg = {
        .data0_io_num = qspi_panel_obj->databus_pins[0],
        .data1_io_num = qspi_panel_obj->databus_pins[1],
        .sclk_io_num = spi_obj->sck,
        .data2_io_num = qspi_panel_obj->databus_pins[2],
        .data3_io_num = qspi_panel_obj->databus_pins[3],
        .max_transfer_sz = (0x4000 * 16) + 8,
        .flags = SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_GPIO_PINS,
    };
    esp_err_t ret = spi_bus_initialize(spi_obj->host, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(spi_bus_initialize)", ret);
    }
    spi_obj->state = MACHINE_HW_SPI_STATE_INIT;

    spi_device_interface_config_t devcfg = {
        .command_bits = qspi_panel_obj->cmd_bits,
        .address_bits = 24,
        .mode = spi_obj->phase | (spi_obj->polarity << 1),
        .clock_speed_hz = qspi_panel_obj->pclk,
        .spics_io_num = -1,
        .flags = SPI_DEVICE_HALFDUPLEX,
        .queue_size = 10,
    };

    ret = spi_bus_add_device(spi_obj->host, &devcfg, &qspi_panel_obj->io_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(spi_bus_add_device)", ret);
    }
}


inline void hal_lcd_qspi_panel_tx_param(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *param,
    size_t param_size
) {
    DEBUG_printf("hal_lcd_qspi_panel_tx_param cmd: %x, param_size: %u\n", lcd_cmd, param_size);

    lcd_qspi_panel_obj_t *qspi_panel_obj = (lcd_qspi_panel_obj_t *)self;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.flags = (SPI_TRANS_MULTILINE_CMD | SPI_TRANS_MULTILINE_ADDR);
    t.cmd = 0x02;
    t.addr = lcd_cmd << 8;
    if (param_size != 0) {
        t.tx_buffer = param;
        t.length = qspi_panel_obj->cmd_bits * param_size;
    } else {
        t.tx_buffer = NULL;
        t.length = 0;
    }
    mp_hal_pin_od_low(qspi_panel_obj->cs_pin);
    spi_device_polling_transmit(qspi_panel_obj->io_handle, &t);
    mp_hal_pin_od_high(qspi_panel_obj->cs_pin);
}


inline void hal_lcd_qspi_panel_tx_color(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *color,
    size_t color_size
) {
    DEBUG_printf("hal_lcd_qspi_panel_tx_color cmd: %x, color_size: %u\n", lcd_cmd, color_size);

    lcd_qspi_panel_obj_t *qspi_panel_obj = (lcd_qspi_panel_obj_t *)self;
    spi_transaction_ext_t t;

    mp_hal_pin_od_low(qspi_panel_obj->cs_pin);
    memset(&t, 0, sizeof(t));
    t.base.flags = SPI_TRANS_MODE_QIO;
    t.base.cmd = 0x32;
    t.base.addr = 0x002C00;
    spi_device_polling_transmit(qspi_panel_obj->io_handle, (spi_transaction_t *)&t);

    uint8_t *p_color = (uint8_t *)color;
    size_t chunk_size;
    size_t len = color_size;
    memset(&t, 0, sizeof(t));
    t.base.flags = SPI_TRANS_MODE_QIO | \
                       SPI_TRANS_VARIABLE_CMD | \
                       SPI_TRANS_VARIABLE_ADDR | \
                       SPI_TRANS_VARIABLE_DUMMY;
    t.command_bits = 0;
    t.address_bits = 0;
    t.dummy_bits = 0;
    do {
        if (len > 0x8000) {
            chunk_size = 0x8000;
        } else {
            chunk_size = len;
        }
        t.base.tx_buffer = p_color;
        t.base.length = chunk_size * 8;
        spi_device_polling_transmit(qspi_panel_obj->io_handle, (spi_transaction_t *)&t);
        len -= chunk_size;
        p_color += chunk_size;
    } while (len > 0);

    mp_hal_pin_od_high(qspi_panel_obj->cs_pin);
}


inline void hal_lcd_qspi_panel_deinit(mp_obj_base_t *self) {
    // lcd_qspi_panel_obj_t *qspi_panel_obj = (lcd_qspi_panel_obj_t *)self;
    // esp_lcd_panel_io_del(qspi_panel_obj->io_handle);
}


void hal_lcd_i80_construct(mp_obj_base_t *self) {
    lcd_i80_obj_t *i80_obj = (lcd_i80_obj_t *)self;
    esp_lcd_i80_bus_config_t bus_config = {
        .dc_gpio_num = mp_hal_get_pin_obj(i80_obj->dc),
        .wr_gpio_num = mp_hal_get_pin_obj(i80_obj->wr),
        .clk_src = LCD_CLK_SRC_PLL160M,
        .data_gpio_nums = {
            mp_hal_get_pin_obj(i80_obj->databus[0]),
            mp_hal_get_pin_obj(i80_obj->databus[1]),
            mp_hal_get_pin_obj(i80_obj->databus[2]),
            mp_hal_get_pin_obj(i80_obj->databus[3]),
            mp_hal_get_pin_obj(i80_obj->databus[4]),
            mp_hal_get_pin_obj(i80_obj->databus[5]),
            mp_hal_get_pin_obj(i80_obj->databus[6]),
            mp_hal_get_pin_obj(i80_obj->databus[7]),
        },
        .bus_width = i80_obj->bus_width,
        .max_transfer_bytes = i80_obj->width * i80_obj->height * sizeof(uint16_t)
    };
    esp_err_t ret = esp_lcd_new_i80_bus(&bus_config, &i80_obj->i80_bus);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_new_i80_bus)", ret);
    }
    esp_lcd_panel_io_i80_config_t io_config = {
        .pclk_hz = i80_obj->pclk,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .flags = {
            .swap_color_bytes = i80_obj->swap_color_bytes,
        },
        .lcd_cmd_bits = i80_obj->cmd_bits,
        .lcd_param_bits = i80_obj->param_bits,
    };
    if (i80_obj->cs != MP_OBJ_NULL) {
        io_config.cs_gpio_num = mp_hal_get_pin_obj(i80_obj->cs);
    } else {
        io_config.cs_gpio_num = -1;
    }
    ret = esp_lcd_new_panel_io_i80(i80_obj->i80_bus, &io_config, &i80_obj->io_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_new_panel_io_i80)", ret);
    }
}


inline void hal_lcd_i80_tx_param(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *param,
    size_t param_size
) {
    DEBUG_printf("tx_param cmd: %x, param_size: %u\n", lcd_cmd, param_size);

    lcd_i80_obj_t *i80_obj = (lcd_i80_obj_t *)self;
    esp_lcd_panel_io_tx_param(i80_obj->io_handle, lcd_cmd, param, param_size);
}


inline void hal_lcd_i80_tx_color(
    mp_obj_base_t *self,
    int lcd_cmd,
    const void *color,
    size_t color_size
) {
    DEBUG_printf("tx_color cmd: %x, color_size: %u\n", lcd_cmd, color_size);

    lcd_i80_obj_t *i80_obj = (lcd_i80_obj_t *)self;
    esp_lcd_panel_io_tx_color(i80_obj->io_handle, lcd_cmd, color, color_size);
}


inline void hal_lcd_i80_deinit(mp_obj_base_t *self) {
    lcd_i80_obj_t *i80_obj = (lcd_i80_obj_t *)self;
    esp_lcd_panel_io_del(i80_obj->io_handle);
    esp_lcd_del_i80_bus(i80_obj->i80_bus);
}


#if DPI_LCD_SUPPORTED
void hal_lcd_dpi_construct(mp_obj_base_t *self)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_PLL160M,
        .timings = {
            .pclk_hz = dpi_obj->pclk,
            .h_res = dpi_obj->width,
            .v_res = dpi_obj->height,
            // The following parameters should refer to LCD spec
            .hsync_pulse_width = dpi_obj->timings.hsync_pulse_width,
            .hsync_back_porch  = dpi_obj->timings.hsync_back_porch,
            .hsync_front_porch = dpi_obj->timings.hsync_front_porch,
            .vsync_pulse_width = dpi_obj->timings.vsync_pulse_width,
            .vsync_back_porch  = dpi_obj->timings.vsync_back_porch,
            .vsync_front_porch = dpi_obj->timings.vsync_front_porch,
            .flags = {
                .pclk_active_neg = 1,
            },
        },
        .data_width = dpi_obj->bus_width, // RGB565 in parallel mode, thus 16bit in width
        .psram_trans_align = 64,
        .hsync_gpio_num = mp_hal_get_pin_obj(dpi_obj->hsync_obj),
        .vsync_gpio_num = mp_hal_get_pin_obj(dpi_obj->vsync_obj),
        .de_gpio_num = mp_hal_get_pin_obj(dpi_obj->de_obj),
        .pclk_gpio_num  = mp_hal_get_pin_obj(dpi_obj->pclk_obj),
        .data_gpio_nums = {
            mp_hal_get_pin_obj(dpi_obj->databus_obj[0]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[1]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[2]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[3]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[4]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[5]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[6]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[7]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[8]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[9]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[10]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[11]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[12]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[13]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[14]),
            mp_hal_get_pin_obj(dpi_obj->databus_obj[15]),
        },
        .disp_gpio_num = dpi_obj->disp_pin,
        .on_frame_trans_done = NULL,
        .user_ctx = NULL,
        .flags = {
            .fb_in_psram = 1, // allocate frame buffer in PSRAM
        },
    };
    esp_err_t ret = esp_lcd_new_rgb_panel(&panel_config, &dpi_obj->panel_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_new_rgb_panel)", ret);
    }
    ret = esp_lcd_panel_reset(dpi_obj->panel_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_panel_reset)", ret);
    }
    ret = esp_lcd_panel_init(dpi_obj->panel_handle);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_OSError, "%d(esp_lcd_panel_init)", ret);
    }
}


inline void hal_lcd_dpi_reset(mp_obj_base_t *self)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_reset(dpi_obj->panel_handle);
}


inline void hal_lcd_dpi_init(mp_obj_base_t *self)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_init(dpi_obj->panel_handle);
}


inline void hal_lcd_dpi_del(mp_obj_base_t *self)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_del(dpi_obj->panel_handle);
}


inline void hal_lcd_dpi_bitmap(mp_obj_base_t *self,
                               int            x_start,
                               int            y_start,
                               int            x_end,
                               int            y_end,
                               const void    *color_data)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_draw_bitmap(
        dpi_obj->panel_handle,
        x_start,
        y_start,
        x_end,
        y_end,
        color_data
    );
}


inline void hal_lcd_dpi_mirror(mp_obj_base_t *self,
                               bool           mirror_x,
                               bool           mirror_y)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_mirror(dpi_obj->panel_handle, mirror_x, mirror_y);
}


inline void hal_lcd_dpi_swap_xy(mp_obj_base_t *self, bool swap_axes)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_swap_xy(dpi_obj->panel_handle, swap_axes);
}


inline void hal_lcd_dpi_set_gap(mp_obj_base_t *self, int x_gap, int y_gap)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_set_gap(dpi_obj->panel_handle, x_gap, y_gap);
}


inline void hal_lcd_dpi_invert_color(mp_obj_base_t *self, bool invert_color_data)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_invert_color(dpi_obj->panel_handle, invert_color_data);
}


inline void hal_lcd_dpi_disp_off(mp_obj_base_t *self, bool off)
{
    lcd_dpi_obj_t *dpi_obj = (lcd_dpi_obj_t *)self;
    esp_lcd_panel_disp_off(dpi_obj->panel_handle, off);
}
#endif