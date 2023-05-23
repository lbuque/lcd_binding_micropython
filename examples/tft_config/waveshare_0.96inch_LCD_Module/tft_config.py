from machine import Pin, SPI, SoftSPI
import lcd

TFA = const(1)
BFA = const(1)

# waveshare
# cmd(int) args(bytes) delay(ms)
__init_waveshare = (
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
    (0x13, b'', 0),
    (0x29, b'', 0)
)


def config():
    hspi = SPI(2, sck=Pin(14), mosi=Pin(13), miso=None)
    # hspi = SoftSPI(baudrate=80 * 1000* 1000, polarity=0, phase=0, sck=Pin(14), mosi=Pin(13), miso=Pin(12))
    panel = lcd.SPIPanel(spi=hspi, dc=Pin(16), cs=Pin(15), pclk=60000000, width=80, height=160)
    st = lcd.ST7735(panel, reset=Pin(17), backlight=Pin(18), color_space=lcd.BGR)
    st.backlight_on()
    st.reset()
    # st.custom_init(init_waveshare)
    st.init()
    st.invert_color(True)
    return st


def color565(r, g, b):
    c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
    return c
