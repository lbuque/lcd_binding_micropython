import lcd
from machine import Pin, SPI

rm67162_spi_init = (
    (0xFE, b'\x00', 0),   # PAGE
    (0x36, b'\x00', 0),   # Scan Direction Control
    (0x3A, b'\x75', 0),   # Interface Pixel Format 16bit/pixel
    (0x51, b'\x00', 0),   # Write Display Brightness MAX_VAL=0XFF
    (0x11, b'\x00', 120), # Sleep Out
    (0x29, b'\x00', 120), # Display on
    (0x51, b'\xD0', 0),   # Write Display Brightness MAX_VAL=0XFF
)

rm67162_qspi_init = (
    (0x11, b'\x00', 120), # Sleep Out
    (0x3A, b'\x55', 0),   # Interface Pixel Format 16bit/pixel
    (0x51, b'\x00', 0),   # Write Display Brightness MAX_VAL=0XFF
    (0x29, b'\x00', 120), # Display on
    (0x51, b'\xD0', 0),   # Write Display Brightness MAX_VAL=0XFF
)


def config():
    hspi = SPI(2, sck=Pin(47), mosi=None, miso=None, polarity=0, phase=0)
    panel = lcd.QSPIPanel(
        spi=hspi,
        data=(Pin(18), Pin(7), Pin(48), Pin(5)),
        dc=Pin(7),
        cs=Pin(6),
        pclk=80 * 1000 * 1000,
        width=240,
        height=536
    )
    rm = lcd.RM67162(panel, reset=Pin(17), bpp=16)
    rm.reset()
    rm.init()
    # rm.custom_init(rm67162_qspi_init)
    rm.rotation(0)
    rm.backlight_on()
    return rm


def color565(r, g, b):
    c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
    return (c >> 8) | (c << 8)
