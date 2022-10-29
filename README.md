LCD Driver for MicroPython
------------------------------

Contents:

- [Introduction](#introduction)
- [Features](#features)
- [References](#references)
- [Future plans](#future-plans)

## Introduction

This driver is based on [esp_lcd](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html).

Currently only some basic functions are supported.It will be compatible with [st7789_mpy](https://github.com/russhughes/st7789_mpy) in the future.

## Features

The following display driver ICs are supported:

- Support for ST7789 displays
- Support for ST7701 displays (based on [esp_lcd](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html))

Supported boards：

- [LILYGO T-DisplayS3](https://github.com/Xinyuan-LilyGO/T-Display-S3)
- [LILYGO T-RGB](https://github.com/Xinyuan-LilyGO/T-RGB)

## References

- [st7789s3_mpy](https://github.com/russhughes/st7789s3_mpy)

## Future plans

- [ ] ST7735
- [ ] api documentation
