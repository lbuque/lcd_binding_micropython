from machine import Pin, SPI, SoftSPI
import time
import lcd

TFA = const(1)
BFA = const(1)

# waveshare
# cmd(int) args(bytes) delay(ms)
init_waveshare = (
    (0x11, b'', 120),
    (0x21, b'', 0),
    (0xB1, b'\x05\x3A\x3A', 0),
    (0xB2, b'\x05\x3A\x3A', 0),
    (0xB3, b'\x05\x3A\x3A\x05\x3A\x3A', 0),
    (0xB4, b'\x03', 0),
    (0xC0, b'\x62\x02\x04', 0),
    (0xC1, b'\xC0', 0),
    (0xC2, b'\x0D\x00', 0),
    (0xC3, b'\x8D\x6A', 0),
    (0xC4, b'\x8D\xEE', 0),
    (0xC5, b'\x0E', 0),
    (0xE0, b'\x10\x0E\x02\x03\x0E\x07\x02\x07\x0A\x12\x27\x37\x00\x0D\x0E\x10', 0),
    (0xE1, b'\x10\x0E\x03\x03\x0F\x06\x02\x08\x0A\x13\x26\x36\x00\x0D\x0E\x10', 0),
    (0x3A, b'\x05', 0),
    (0x36, b'\xA8', 0),
    (0x29, b'', 0)
)


# tft_espi 1
# cmd(int) args(bytes) delay(ms)
init_tft_espi_1 = (
    (0x11, b'', 500),
    (0xB1, b'\x01\x2C\x2D', 0),
    (0xB2, b'\x01\x2C\x2D', 0),
    (0xB3, b'\x01\x2C\x2D\x01\x2C\x2D', 0),
    (0xB4, b'\x07', 0),
    (0xC0, b'\xA2\x02\x84', 0),
    (0xC1, b'\xC5', 0),
    (0xC2, b'\x0A\x00', 0),
    (0xC3, b'\x8A\x2A', 0),
    (0xC4, b'\x8A\xEE', 0),
    (0xC5, b'\x0E', 0),
    (0x20, b'', 0),
    (0x36, b'\xC8', 0),
    (0x3A, b'\x05', 0),
    (0x2A, b'\x00\x02\x00\x81', 0),
    (0x2B, b'\x00\x01\x00\xA0', 0),
    (0xE0, b'\x02\x1c\x07\x12\x37\x32\x29\x2d\x29\x25\x2B\x39\x00\x01\x03\x10', 0),
    (0xE1, b'\x03\x1d\x07\x06\x2E\x2C\x29\x2D\x2E\x2E\x37\x3F\x00\x00\x02\x10', 0),
    (0x13, b'', 100),
    (0x29, b'', 100)
)

# tft_espi 2
# cmd(int) args(bytes) delay(ms)
init_tft_espi_2 = (
    (0x11, b'', 500),
    (0xB1, b'\x01\x2C\x2D', 0),
    (0xB2, b'\x01\x2C\x2D', 0),
    (0xB3, b'\x01\x2C\x2D\x01\x2C\x2D', 0),
    (0xB4, b'\x07', 0),
    (0xC0, b'\xA2\x02\x84', 0),
    (0xC1, b'\xC5', 0),
    (0xC2, b'\x0A\x00', 0),
    (0xC3, b'\x8A\x2A', 0),
    (0xC4, b'\x8A\xEE', 0),
    (0xC5, b'\x0E', 0),
    (0x20, b'', 0),
    (0x36, b'\xC8', 0),
    (0x3A, b'\x05', 0),
    (0x2A, b'\x00\x02\x00\x81', 0),
    (0x2B, b'\x00\x01\x00\xA0', 0),
    (0xE0, b'\x02\x1c\x07\x12\x37\x32\x29\x2d\x29\x25\x2B\x39\x00\x01\x03\x10', 0),
    (0xE1, b'\x03\x1d\x07\x06\x2E\x2C\x29\x2D\x2E\x2E\x37\x3F\x00\x00\x02\x10', 0),
    (0x13, b'', 100),
    (0x29, b'', 100)
)

def config():
    hspi = SPI(2, sck=Pin(18), mosi=Pin(19), miso=None)
    # hspi = SoftSPI(baudrate=80 * 1000* 1000, polarity=0, phase=0, sck=Pin(18), mosi=Pin(19), miso=Pin(4))
    panel = lcd.SPIPanel(spi=hspi, dc=Pin(23), cs=Pin(5), pclk=60000000, width=80, height=160)
    st = lcd.ST7735(panel, reset=Pin(26), backlight=Pin(27))
    st.backlight_on()
    st.reset()
    st.custom_init(init_tft_espi_2)
    st.invert_color(True)
    st.rotation(3)
    return st


def color565(r, g, b):
    c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
    return c