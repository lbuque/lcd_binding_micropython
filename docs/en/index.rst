************
Get Started
************

Introduction
=============

This driver is based on `esp_lcd`_.

Currently only some basic functions are supported.It will be compatible with `st7789_mpy`_ in the future.

.. _esp_lcd: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html
.. _st7789_mpy: https://github.com/russhughes/st7789_mpy

Supported LCD Types
====================

- Intel 8080 parallel LCD
- DPI(RGB) LCD
- SPI LCD
- QSPI LCD

Supported driver ICs
=====================

- ST7789
- ST7701s
- ST7735
- ILI9488
- RM67162

Compiling guide
================

Get the source code
--------------------

.. code-block:: shell

    cd micropython
    git clone https://github.com/lbuque/lcd_binding_micropython.git extmod/lcd_binding_micropython

esp32
------

micropython <= v1.20.0
^^^^^^^^^^^^^^^^^^^^^^

.. note::

    Please prepare `esp-idf release/v4.4 <https://github.com/espressif/esp-idf/tree/release/v4.4>`_ before compiling

    For more detailed compilation of esp32, please refer to https://github.com/micropython/micropython/tree/master/ports/esp32

modify ``micropython/ports/esp32/main/CMakeLists.txt``:

.. code-block:: diff

    diff --git a/ports/esp32/main/CMakeLists.txt b/ports/esp32/main/CMakeLists.txt
    index f8acfa905..209547892 100644
    --- a/ports/esp32/main/CMakeLists.txt
    +++ b/ports/esp32/main/CMakeLists.txt
    @@ -124,6 +124,7 @@ set(IDF_COMPONENTS
        ulp
        vfs
        xtensa
    +    esp_lcd
    )
    
    if(IDF_VERSION_MINOR GREATER_EQUAL 1 OR IDF_VERSION_MAJOR GREATER_EQUAL 5)

build:

.. code-block:: shell

    cd ports/esp32/
    make USER_C_MODULES=../../../extmod/lcd_binding_micropython/lcd/micropython.cmake

micropython == master
^^^^^^^^^^^^^^^^^^^^^

.. note::

    Please prepare `esp-idf v5.0.2 <https://github.com/espressif/esp-idf/tree/v5.0.2>`_ before compiling

    For more detailed compilation of esp32, please refer to https://github.com/micropython/micropython/tree/master/ports/esp32

modify ``lcd_binding_micropython/lcd/hal/esp32/esp32.c`` :

.. code-block:: diff

    diff --git a/lcd/hal/esp32/esp32.c b/lcd/hal/esp32/esp32.c
    index b100971..907feeb 100644
    --- a/lcd/hal/esp32/esp32.c
    +++ b/lcd/hal/esp32/esp32.c
    @@ -119,8 +119,8 @@ inline void hal_lcd_spi_panel_deinit(mp_obj_base_t *self)
    
                for (int i = 0; i < 3; i++) {
                    if (pins[i] != -1) {
    -                    gpio_pad_select_gpio(pins[i]);
    -                    gpio_matrix_out(pins[i], SIG_GPIO_OUT_IDX, false, false);
    +                    esp_rom_gpio_pad_select_gpio(pins[i]);
    +                    gpio_iomux_out(pins[i], SIG_GPIO_OUT_IDX, false);
                        gpio_set_direction(pins[i], GPIO_MODE_INPUT);
                    }
                }

modify ``micropython/ports/esp32/esp32_common.cmake`` :

.. code-block:: diff

    diff --git a/ports/esp32/esp32_common.cmake b/ports/esp32/esp32_common.cmake
    index 37a19316b..6be95ddf5 100644
    --- a/ports/esp32/esp32_common.cmake
    +++ b/ports/esp32/esp32_common.cmake
    @@ -136,6 +136,7 @@ list(APPEND IDF_COMPONENTS
        ulp
        vfs
        xtensa
    +    esp_lcd
    )
    
    # Register the main IDF component.

build:

.. code-block:: shell

    cd ports/esp32/
    make USER_C_MODULES=../../../extmod/lcd_binding_micropython/lcd/micropython.cmake

.. toctree::
    :maxdepth: 1
    :hidden:

    self
    API Reference <api-reference/lcd>
    LCD WiKi <screen/index>
    Copyrights <COPYRIGHT>
