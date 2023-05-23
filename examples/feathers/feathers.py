import framebuf
import random
import math
import utime
import tft_config

try:
    from tft_config import color565
except:
    def color565(r, g, b):
        c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
        return c

def between(left, right, along):
    """returns a point along the curve from left to right"""
    dist = (1 - math.cos(along * math.pi)) / 2
    return left * (1 - dist) + right * dist

def color_wheel(position):
    """returns a 565 color from the given position of the color wheel"""
    position = (255 - position) % 255

    if position < 85:
        return color565(255 - position * 3, 0, position * 3)

    if position < 170:
        position -= 85
        return color565(0, position * 3, 255 - position * 3)

    position -= 170
    return color565(position * 3, 255 - position * 3, 0)

def main():
    '''
    The big show!
    '''
    tft = tft_config.config()
    tft.rotation(1)
    buf = bytearray(1 * tft.height() * 2)
    fbuf = framebuf.FrameBuffer(buf, 1, tft.height(), framebuf.RGB565)
    height = tft.height()       # height of display in pixels
    width = tft.width()         # width if display in pixels

    tfa = tft_config.TFA    # top free area when scrolling
    bfa = tft_config.BFA    # bottom free area when scrolling

    scroll = 0                  # scroll position
    wheel = 0                   # color wheel position

    tft.vscroll_area(tfa, width, bfa)    # set scroll area
    tft.vscroll_start(scroll + tfa)        # set scroll position
    fbuf.fill(color565(0, 0, 0))
    for i in range(0, tft.width()):
        tft.bitmap(i, 0, i + 1, tft.height(), buf)

    half = (height >> 1) - 1    # half the height of the dislay
    interval = 0                # steps between new points
    increment = 0               # increment per step
    counter = 1                 # step counter, overflow to start
    current_y = 0               # current_y value (right point)
    last_y = 0                  # last_y value (left point)

    # segment offsets
    x_offsets = [x * (width // 8) -1 for x in range(2,9)]

    while True:
        # when the counter exceeds the interval, save current_y to last_y,
        # choose a new random value for current_y between 0 and 1/2 the
        # height of the display, choose a new random interval then reset
        # the counter to 0

        if counter > interval:
            last_y = current_y
            current_y = random.randint(0, half)
            counter = 0
            interval = random.randint(10, 100)
            increment = 1 / interval

        # clear the first column of the display and scroll it
        fbuf.fill(color565(0, 0, 0))
        tft.bitmap(scroll, 0, scroll + 1, height, buf)
        tft.vscroll_start(scroll + tfa, False)

        # get the next point between last_y and current_y
        tween = int(between(last_y, current_y, counter * increment))

        # draw mirrored pixels across the display at the offsets using the color_wheel effect
        for i, x_offset in enumerate(x_offsets):
            c = color_wheel(wheel+(i<<2)).to_bytes(2, 'little')
            tft.bitmap(
                (scroll + x_offset) % width,
                half + tween,
                ((scroll + x_offset) % width) + 1,
                half + tween + 1,
                c
            )
            tft.bitmap(
                (scroll + x_offset) % width,
                half - tween,
                ((scroll + x_offset) % width) + 1,
                half - tween + 1,
                c
            )

        # increment scroll, counter, and wheel
        scroll = (scroll + 1) % width
        wheel = (wheel + 1) % 256
        counter += 1

        # pause to slow down scrolling
        # utime.sleep(0.01)

main()
