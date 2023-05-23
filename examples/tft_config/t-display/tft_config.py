from machine import Pin, SPI
import lcd

TFA = const(40)
BFA = const(40)

def config():
    hspi = SPI(2, sck=Pin(18), mosi=Pin(19), miso=None)
    panel = lcd.SPIPanel(spi=hspi, dc=Pin(16), cs=Pin(5), pclk=60000000, width=135, height=240)
    st = lcd.ST7789(panel, reset=Pin(23), backlight=Pin(4))
    st.backlight_on()
    st.reset()
    st.init()
    st.invert_color(True)
    return st


def color565(r, g, b):
    c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
    return c
