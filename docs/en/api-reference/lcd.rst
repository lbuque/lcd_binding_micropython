:mod:`lcd` --- LCD Driver library
==================================

.. module:: lcd
    :synopsis: LCD Driver library

This module contains common LCD drivers on the market, such as SPI LCD, I80 LCD (aka Intel 8080 Parallel LCD), RGB LCD, etc.

Currently SPI LCD is still in preparation. 

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

    Color space: RGB

.. data:: BGR

    Color space: BGR

.. data:: MONOCHROME

    Color space: monochrome
