*********
快速开始
*********

概况
====

此驱动程序基于 `esp_lcd`_。

目前只支持部分基本功能，未来会兼容 `st7789_mpy`_。

.. _esp_lcd: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html
.. _st7789_mpy: https://github.com/russhughes/st7789_mpy

已支持的LCD类型
===============

- Intel 8080并行LCD
- DPI(RGB) LCD
- SPI LCD
- QSPI LCD

已支持的驱动IC
==============

- ST7789
- ST7701s
- ST7735
- ILI9488
- RM67162

编译指南
========

获取源码
---------

.. code-block:: shell

    cd micropython
    git clone https://github.com/lbuque/lcd_binding_micropython.git extmod/lcd_binding_micropython

esp32
------

micropython <= v1.20.0
^^^^^^^^^^^^^^^^^^^^^^

.. note::

    编译前请准备好`esp-idf release/v4.4 <https://github.com/espressif/esp-idf/tree/release/v4.4>`_

    关于esp32更详细的编译说明，请参考 https://github.com/micropython/micropython/tree/master/ports/esp32

修改 ``micropython/ports/esp32/main/CMakeLists.txt`` :

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

编译:

.. code-block:: shell

    cd ports/esp32/
    make USER_C_MODULES=../../../extmod/lcd_binding_micropython/lcd/micropython.cmake

micropython == master
^^^^^^^^^^^^^^^^^^^^^

.. note::

    编译前请准备好`esp-idf v5.0.2 <https://github.com/espressif/esp-idf/tree/v5.0.2>`_

    关于esp32更详细的编译说明，请参考 https://github.com/micropython/micropython/tree/master/ports/esp32

修改 ``lcd_binding_micropython/lcd/hal/esp32/esp32.c`` :

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

修改 ``micropython/ports/esp32/esp32_common.cmake`` :

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

编译:

.. code-block:: shell

    cd ports/esp32/
    make USER_C_MODULES=../../../extmod/lcd_binding_micropython/lcd/micropython.cmake

.. toctree::
    :maxdepth: 1
    :hidden:

    self
    API 参考 <api-reference/lcd>
    屏幕 WiKi <screen/index>
    版权 <COPYRIGHT>
