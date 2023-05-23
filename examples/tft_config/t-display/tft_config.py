from machine import Pin, SPI
import lcd

def config():
    hspi = SPI(2, sck=Pin(18), mosi=Pin(19), miso=None)
    panel = lcd.SPIPanel(spi=hspi, command=Pin(16), cs=Pin(5), pclk=60000000, width=135, height=240)
    st = lcd.ST7789(panel, reset=Pin(23), backlight=Pin(4))
    st.backlight_on()
    st.reset()
    st.init()
    st.invert_color(True)
    st.rotation(3)
    return st
