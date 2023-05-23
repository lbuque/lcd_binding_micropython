#ifndef LCD_DPI_H_
#define LCD_DPI_H_

#include "py/obj.h"
#include "mphalport.h"
#if USE_ESP_LCD
#include "esp_lcd_panel_io.h"
#endif

typedef struct _mp_lcd_dpi_obj_t {
    mp_obj_base_t base;

    uint16_t width;
    uint16_t height;

    mp_obj_t databus_obj[18];
    mp_obj_t hsync_obj;
    mp_obj_t vsync_obj;
    mp_obj_t de_obj;
    mp_obj_t pclk_obj;
    mp_obj_t disp_obj;
    mp_obj_t backlight;

    mp_hal_pin_obj_t databus_pins[16];
    mp_hal_pin_obj_t hsync_pin;
    mp_hal_pin_obj_t vsync_pin;
    mp_hal_pin_obj_t de_pin;
    mp_hal_pin_obj_t pclk_pin;
    mp_hal_pin_obj_t disp_pin;

    uint32_t pclk;
    size_t bus_width;

    struct {
        uint32_t pclk_hz;           /*!< Frequency of pixel clock */
        uint32_t h_res;             /*!< Horizontal resolution, i.e. the number of pixels in a line */
        uint32_t v_res;             /*!< Vertical resolution, i.e. the number of lines in the frame  */
        uint32_t hsync_pulse_width; /*!< Horizontal sync width, unit: PCLK period */
        uint32_t hsync_back_porch;  /*!< Horizontal back porch, number of PCLK between hsync and start of line active data */
        uint32_t hsync_front_porch; /*!< Horizontal front porch, number of PCLK between the end of active data and the next hsync */
        uint32_t vsync_pulse_width; /*!< Vertical sync width, unit: number of lines */
        uint32_t vsync_back_porch;  /*!< Vertical back porch, number of invalid lines between vsync and start of frame */
        uint32_t vsync_front_porch; /*!< Vertical front porch, number of invalid lines between the end of frame and the next vsync */
        struct {
            uint32_t hsync_idle_low: 1;  /*!< The hsync signal is low in IDLE state */
            uint32_t vsync_idle_low: 1;  /*!< The vsync signal is low in IDLE state */
            uint32_t de_idle_high: 1;    /*!< The de signal is high in IDLE state */
            uint32_t pclk_active_neg: 1; /*!< Whether the display data is clocked out at the falling edge of PCLK */
            uint32_t pclk_idle_high: 1;  /*!< The PCLK stays at high level in IDLE phase */
        } flags;                             /*!< LCD RGB timing flags */
    } timings;

#if USE_ESP_LCD
    esp_lcd_panel_handle_t panel_handle;
#endif
} mp_lcd_dpi_obj_t;

extern const mp_obj_type_t mp_lcd_dpi_type;

#endif