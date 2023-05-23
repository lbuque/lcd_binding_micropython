"""
scroll.py
    Smoothly scroll all characters of a font up the display.
    Fonts heights must be even multiples of the screen height
    (i.e. 8 or 16 pixels high).

    This example is from the following code:

        https://github.com/russhughes/st7789_mpy/blob/master/examples/scroll.py
"""

import utime
import tft_config
import framebuf

def cycle(p):
    try:
        len(p)
    except TypeError:
        cache = []
        for i in p:
            yield i
            cache.append(i)
        p = cache
    while p:
        yield from p

def main():
    colors = cycle([0xe000, 0xece0, 0xe7e0, 0x5e0, 0x00d3, 0x7030])
    foreground = next(colors)
    background = 0x0000

    tft = tft_config.config()
    tft.rotation(2)

    '''The font height of the framebuf is 8 pixels.'''
    font_height = const(8)

    buf = bytearray(tft.width() * font_height * 2)
    fbuf = framebuf.FrameBuffer(buf, tft.width(), font_height, framebuf.RGB565)

    fbuf.fill(background)
    for i in range(0, tft.height(), font_height):
        tft.bitmap(0, i, tft.width(), i + font_height, buf)

    width = tft.width()
    height = tft.height()
    last_line = height - font_height

    tfa = tft_config.TFA # top free area
    bfa = tft_config.BFA # bottom free area

    tft.vscroll_area(tfa, height, bfa)

    scroll = 0
    character = 0x20

    while True:
        '''clear top line before scrolling off display'''
        # fbuf.fill_rect(0, 0, width, 1, background)
        # tft.bitmap(0, scroll, width, scroll + 1, buf)

        '''Write new line when we have scrolled the height of a character'''
        if scroll % font_height == 0:
            line = (height - scroll) % height
            # line = (scroll + last_line) % height
            fbuf.fill(background)
            '''write character hex value as a string'''
            fbuf.text(
                '0x{:02x} {}'.format(character, chr(character)),
                16,
                0,
                foreground
            )

            '''write character using a integer (could be > 0x7f)'''
            tft.bitmap(0, line, width, line + font_height, buf)

            '''change color for next line'''
            foreground = next(colors)

            '''next character with rollover at 256'''
            character += 1
            if character > 0x7f:
                character = 0x20

        '''scroll the screen up 1 row'''
        tft.vscroll_start(scroll + tfa, False)
        scroll += 1
        scroll %= height

        '''!!! Scrolling too fast can make you lose sight of things.'''
        utime.sleep(0.01)

main()
