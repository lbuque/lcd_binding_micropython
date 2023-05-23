import tft_config
import time
import framebuf
try:
    from tft_config import color565
except:
    def color565(r, g, b):
        c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
        return c


def render(tft, x, y, w, h, buf):
    for i in range(0, h, 10):
        tft.bitmap(0, i, w, i + 10, buf)
    if h % 10 != 0:
        tft.bitmap(0, h - h % 10, w, h, buf)


def main():
    tft = tft_config.config()
    width = tft.width()
    height = tft.height()
    buf = bytearray(width * 10 * 2)
    fbuf = framebuf.FrameBuffer(buf, width, 10, framebuf.RGB565)

    while True:
        fbuf.fill(color565(255, 0, 0))
        render(tft, 0, 0,width, height, buf)
        time.sleep(0.5)

        fbuf.fill(color565(0, 255, 0))
        render(tft, 0, 0, width, height, buf)
        time.sleep(0.5)

        fbuf.fill(color565(0, 0, 255))
        render(tft, 0, 0, width, height, buf)
        time.sleep(0.5)


main()

