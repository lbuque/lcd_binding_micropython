import lcd
from machine import Pin
import logo
import gc
import time

gc.enable()

i8080 = lcd.I8080(data=(Pin(39), Pin(40), Pin(41), Pin(42), Pin(45), Pin(46), Pin(47), Pin(48)),
                     command=Pin(7),
                     write=Pin(8),
                     read=Pin(9),
                     cs=Pin(6),
                     pclk=4 * 1000 * 1000,
                     width=320,
                     height=170,
                     swap_color_bytes=False,
                     cmd_bits=8,
                     param_bits=8)

st = lcd.ST7789(i8080, reset=Pin(5), backlight=Pin(38))

st.reset()
st.init()
st.invert_color(True)
st.swap_xy(True)
st.mirror(False, True)
st.set_gap(0, 35)
st.backlight_on()

st.bitmap(0, 0, logo.WIDTH, logo.HEIGHT, logo.BITMAP)

time.sleep(0.1)
del st
del i8080
gc.collect()
