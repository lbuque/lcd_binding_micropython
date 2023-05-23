
# Create an INTERFACE library for our C module.
add_library(usermod_lcd INTERFACE)

# Add our source files to the lib
set(SRC ${CMAKE_CURRENT_LIST_DIR}/modlcd.c
        ${CMAKE_CURRENT_LIST_DIR}/st7789.c
        ${CMAKE_CURRENT_LIST_DIR}/i80_panel.c
        ${CMAKE_CURRENT_LIST_DIR}/spi_panel.c
)

if (CONFIG_IDF_TARGET_ESP32 OR CONFIG_IDF_TARGET_ESP32S3)
    LIST(APPEND SRC ${CMAKE_CURRENT_LIST_DIR}/hal/esp32.c)
    target_compile_definitions(usermod_lcd INTERFACE USE_ESP_LCD=1)
    if (CONFIG_IDF_TARGET_ESP32S3)
        LIST(APPEND SRC ${CMAKE_CURRENT_LIST_DIR}/rgb_panel.c)
        target_compile_definitions(usermod_lcd INTERFACE RGB_LCD_SUPPORTED=1)
    endif()
else()
    LIST(APPEND SRC ${CMAKE_CURRENT_LIST_DIR}/hal/soft8080.c)
endif()

target_sources(usermod_lcd INTERFACE ${SRC})

# Add the current directory as an include directory.
target_include_directories(usermod_lcd INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_lcd)
