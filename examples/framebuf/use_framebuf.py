import lcd
from machine import Pin
import time

i8080 = lcd.I8080(data=(Pin(39), Pin(40), Pin(41), Pin(42), Pin(45), Pin(46), Pin(47), Pin(48)),
                     command=Pin(7),
                     write=Pin(8),
                     read=Pin(9),
                     cs=Pin(6),
                     pclk=4 * 1000 * 1000,
                     width=320,
                     height=170,
                     # swap_color_bytes=True,
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

import framebuf
buf = bytearray(320 * 170 * 2)
fbuf = framebuf.FrameBuffer(buf, 320, 170, framebuf.RGB565)

while True:
    fbuf.fill(st.color565(255, 0, 0))
    st.bitmap(0, 0, 320, 170, buf)
    time.sleep(1)

    fbuf.fill(st.color565(0, 255, 0))
    st.bitmap(0, 0, 320, 170, buf)
    time.sleep(1)

    fbuf.fill(st.color565(0, 0, 255))
    st.bitmap(0, 0, 320, 170, buf)
    time.sleep(1)
