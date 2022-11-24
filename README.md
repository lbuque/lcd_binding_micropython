LCD Driver for MicroPython
------------------------------

Contents:

- [LCD Driver for MicroPython](#lcd-driver-for-micropython)
- [Introduction](#introduction)
- [Features](#features)
- [Documentation](#documentation)
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
- [LILYGO T-Displat](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)

## Documentation

API documentation for this library can be found on [Read the Docs](https://lcd-binding-micropython.readthedocs.io/en/latest/).

## References

- [st7789s3_mpy](https://github.com/russhughes/st7789s3_mpy)

## Future plans

- [ ] ST7735
- [x] api documentation (2022/11/13)
