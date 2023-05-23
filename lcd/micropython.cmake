
# Create an INTERFACE library for our C module.
add_library(usermod_lcd INTERFACE)

# user configure
set(set CONFIG_LCD_BUS_SPI 1)
set(set CONFIG_LCD_BUS_I80 1)
set(set CONFIG_LCD_BUS_DPI 1)
set(set CONFIG_LCD_DRIVER_ST7735 1)
set(set CONFIG_LCD_DRIVER_ST7789 1)

# hal layer
set(HAL_DIR ${CMAKE_CURRENT_LIST_DIR}/hal)
set(ESP32_HAL_SRC ${HAL_DIR}/esp32/esp32.c)
set(ESP32_HAL_INC ${HAL_DIR}/esp32)
set(COMMON_HAL_SRC ${HAL_DIR}/commom/soft8080.c)
set(COMMON_HAL_INC ${HAL_DIR}/commom)

# bus layer
set(BUS_DIR ${CMAKE_CURRENT_LIST_DIR}/bus)
set(COMMON_BUS_INC ${BUS_DIR}/commom)
set(SPI_BUS_SRC ${BUS_DIR}/spi/spi_panel.c)
set(SPI_BUS_INC ${BUS_DIR}/spi ${COMMON_BUS_INC})
set(I8080_BUS_SRC ${BUS_DIR}/i80/i80_panel.c)
set(I8080_BUS_INC ${BUS_DIR}/i80 ${COMMON_BUS_INC})
set(DPI_BUS_SRC ${BUS_DIR}/dpi/rgb_panel.c)
set(DPI_BUS_INC ${BUS_DIR}/dpi ${COMMON_BUS_INC})

# driver layer
set(DRIVER_DIR ${CMAKE_CURRENT_LIST_DIR}/driver)
set(ST7735_DRIVER_SRC ${DRIVER_DIR}/st7735/st7735.c)
set(ST7735_DRIVER_INC ${DRIVER_DIR}/st7735)
set(ST7789_DRIVER_SRC ${DRIVER_DIR}/st7789/st7789.c)
set(ST7789_DRIVER_INC ${DRIVER_DIR}/st7789)

# Add our source files to the lib
set(SRC ${CMAKE_CURRENT_LIST_DIR}/modlcd.c)
LIST(APPEND SRC ${ESP32_HAL_SRC})
# LIST(APPEND SRC ${COMMON_HAL_SRC})
LIST(APPEND SRC ${SPI_BUS_SRC})
LIST(APPEND SRC ${I8080_BUS_SRC})
# LIST(APPEND SRC ${DPI_BUS_SRC})
LIST(APPEND SRC ${ST7735_DRIVER_SRC})
LIST(APPEND SRC ${ST7789_DRIVER_SRC})

set(INC ${CMAKE_CURRENT_LIST_DIR} ${HAL_DIR}/esp32)
LIST(APPEND INC ${ESP32_HAL_INC})
# LIST(APPEND INC ${COMMON_HAL_INC})
LIST(APPEND INC ${SPI_BUS_INC})
LIST(APPEND INC ${I8080_BUS_INC})
# LIST(APPEND INC ${DPI_BUS_INC})
LIST(APPEND INC ${ST7735_DRIVER_INC})
LIST(APPEND INC ${ST7789_DRIVER_INC})

target_compile_definitions(usermod_lcd INTERFACE USE_ESP_LCD=1)

# if (CONFIG_IDF_TARGET_ESP32 OR CONFIG_IDF_TARGET_ESP32S3)
#     LIST(APPEND SRC ${CMAKE_CURRENT_LIST_DIR}/hal/esp32.c)
#     target_compile_definitions(usermod_lcd INTERFACE USE_ESP_LCD=1)
#     if (CONFIG_IDF_TARGET_ESP32S3)
#         LIST(APPEND SRC ${CMAKE_CURRENT_LIST_DIR}/rgb_panel.c)
#         target_compile_definitions(usermod_lcd INTERFACE RGB_LCD_SUPPORTED=1)
#     endif()
# else()
#     LIST(APPEND SRC ${CMAKE_CURRENT_LIST_DIR}/hal/soft8080.c)
# endif()

target_sources(usermod_lcd INTERFACE ${SRC})

# Add the current directory as an include directory.
target_include_directories(usermod_lcd INTERFACE ${INC})

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_lcd)
