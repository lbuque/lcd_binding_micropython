:mod:`lcd` --- LCD Driver library
==================================

.. module:: lcd
    :synopsis: LCD Driver library

该模块包含市面上常见的 LCD 驱动，如 SPI LCD、I80 LCD（又名 Intel 8080并行LCD）、RGB LCD等。

Classes
--------

.. toctree::
    :maxdepth: 1

    lcd.I8080.rst
    lcd.RGB.rst
    lcd.ST7789.rst
    lcd.ST7735.rst
    lcd.ILI9488.rst
    lcd.RM67162.rst

Constants
---------

.. data:: RGB

    色彩空间顺序: RGB

.. data:: BGR

    色彩空间顺序: BGR

.. data:: MONOCHROME

    色彩空间顺序: monochrome
