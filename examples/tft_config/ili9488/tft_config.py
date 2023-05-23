from machine import Pin, SPI
import lcd

TFA = const(0)
BFA = const(0)

def config():
    hspi = SPI(2, sck=Pin(19), mosi=Pin(23), miso=None)
    panel = lcd.SPIPanel(spi=hspi, dc=Pin(21), cs=Pin(22), pclk=60 * 1000 * 1000, width=320, height=480)
    ili = lcd.ILI9488(panel, reset=Pin(18), backlight=Pin(5), color_space=lcd.ILI9488.BGR, bpp=18)
    ili.backlight_on()
    ili.reset()
    ili.init()
    ili.invert_color(False)
    return ili


def color565(r, g, b):
    c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
    return c
