import st7789
from machine import Pin
import time
backlight=Pin(38, Pin.OUT)
backlight.value(1)
i8080 = st7789.I8080(data=(Pin(39), Pin(40), Pin(41), Pin(42), Pin(45), Pin(46), Pin(47), Pin(48)),
                     command=Pin(7),
                     write=Pin(8),
                     read=Pin(9),
                     cs=Pin(6),
                     backlight=Pin(38),
                     pclk=2 * 1000 * 1000,
                     width=320,
                     height=170,
                     # swap_color_bytes=True,
                     cmd_bits=8,
                     param_bits=8)
i8080.on()
st = st7789.ST7789(i8080, reset=Pin(5))
st.reset()
st.init()
st.invert_color(True)
st.swap_xy(True)
st.mirror(False, True)
st.set_gap(0, 35)

import logo
st.bitmap(0, 0, logo.WIDTH, logo.HEIGHT, logo.BITMAP)
time.sleep(10)

