
# Create an INTERFACE library for our C module.
add_library(usermod_lcd INTERFACE)

# Add our source files to the lib
set(SRC
    ${CMAKE_CURRENT_LIST_DIR}/modlcd.c
    ${CMAKE_CURRENT_LIST_DIR}/st7789.c
    ${CMAKE_CURRENT_LIST_DIR}/i80_panel.c
)

# LIST(APPEND SRC
#             ${CMAKE_CURRENT_LIST_DIR}/hal/soft8080.c
# )

if (CONFIG_IDF_TARGET_ESP32S3)
    LIST(APPEND SRC
                ${CMAKE_CURRENT_LIST_DIR}/hal/esp32.c
    )
    set(USE_ESP_LCD 1)
else()
    LIST(APPEND SRC
                ${CMAKE_CURRENT_LIST_DIR}/hal/soft8080.c
    )
endif()

target_sources(usermod_lcd INTERFACE ${SRC})

# Add the current directory as an include directory.
target_include_directories(usermod_lcd INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_lcd)
