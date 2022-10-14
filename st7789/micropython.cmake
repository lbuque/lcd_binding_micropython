
# Create an INTERFACE library for our C module.
add_library(usermod_st7789 INTERFACE)

# Add our source files to the lib
target_sources(usermod_st7789 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/modst7789.c
    ${CMAKE_CURRENT_LIST_DIR}/st7789.c
    ${CMAKE_CURRENT_LIST_DIR}/i80_panel.c
    ${CMAKE_CURRENT_LIST_DIR}/parallelbus_hal/esp32.c
    # ${CMAKE_CURRENT_LIST_DIR}/parallelbus_hal/soft8080.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_st7789 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_st7789)
