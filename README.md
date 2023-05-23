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
- Support for ST7735 displays
- Support for ILI9488 displays
- Support for RM67162 displays

Supported boards：

- [LILYGO T-DisplayS3](https://github.com/Xinyuan-LilyGO/T-Display-S3)
- [LILYGO T-RGB](https://github.com/Xinyuan-LilyGO/T-RGB)
- [LILYGO T-Display](https://github.com/Xinyuan-LilyGO/TTGO-T-Display)
- [LILYGO T-Wristband](https://github.com/Xinyuan-LilyGO/T-Wristband)
- [LILYGO T-DisplayS3-AMOLED](https://github.com/Xinyuan-LilyGO/T-Display-S3-AMOLED)

| Driver IC | Hardware SPI     | Software SPI     | Hardware QSPI    | I8080            | DPI(RGB)         |
| --------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| ESP32     | ![alt text][1]   | ![alt text][4]   | ![alt text][1]   | ![alt text][1]   | ![alt text][2]   |
| ESP32-C3  | ![alt text][3]   | ![alt text][4]   | ![alt text][3]   | ![alt text][2]   | ![alt text][2]   |
| ESP32-S2  | ![alt text][3]   | ![alt text][4]   | ![alt text][3]   | ![alt text][3]   | ![alt text][2]   |
| ESP32-S3  | ![alt text][1]   | ![alt text][4]   | ![alt text][1]   | ![alt text][1]   | ![alt text][1]   |

[1]: https://camo.githubusercontent.com/bd5f5f82b920744ff961517942e99a46699fee58737cd9b31bf56e5ca41b781b/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f2d737570706f727465642d677265656e
[2]: https://img.shields.io/badge/-not%20support-lightgrey
[3]: https://img.shields.io/badge/-untested-red
[4]: https://img.shields.io/badge/-todo-blue

## Documentation

API documentation for this library can be found on [Read the Docs](https://lcd-binding-micropython.readthedocs.io/en/latest/).

## More

If you need to support more screens, please provide corresponding hardware.

# Related Repositories

- [framebuf-plus](https://github.com/lbuque/framebuf-plus)

## References

- [st7789s3_mpy](https://github.com/russhughes/st7789s3_mpy)
