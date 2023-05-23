import tft_config
import framebuf
try:
    from tft_config import color565
except:
    def color565(r, g, b):
        c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
        return c


def hsv2rgb(hue, sat, val):
    '''The conversion algorithm comes from https://blog.csdn.net/lly_3485390095/article/details/104570885'''
    C = 0.0
    X = 0.0
    Y = 0.0
    Z = 0.0
    i = 0
    H = float(hue)
    S = sat / 100.0
    V = val / 100.0
    if int(S) == 0:
        return int(V*255), int(V*255), int(V*255)
    else:
        H = H / 60
        i = int(H)
        C = H - i

        X = V * (1 - S)
        Y = V * (1 - S * C)
        Z = V * (1 -S * (1 - C))
        if i == 0:
            return int(V * 255), int(Z * 255), int(X * 255)
        elif i == 1:
            return int(Y * 255), int(V * 255), int(X * 255)
        elif i == 2:
            return int(X * 255), int(V * 255), int(Z * 255)
        elif i == 3:
            return int(X * 255), int(Y * 255), int(V * 255)
        elif i == 4:
            return int(Z * 255), int(X * 255), int(V * 255)
        elif i == 5:
            return int(V * 255), int(X * 255), int(Y * 255)


def hsv_wheel():
    while True:
        for i in range(0, 360):
            yield hsv2rgb(i, 255, 100)


def main():
    tft = tft_config.config()
    width = tft.width()
    height = tft.height()
    buf = bytearray(tft.width() * 10 * 2)
    fbuf = framebuf.FrameBuffer(buf, tft.width(), 10, framebuf.RGB565)
    color = hsv_wheel()
    while True:
        r, g, b = next(color)
        fbuf.fill(color565(r, g, b))
        for j in range(0, height, 10):
            tft.bitmap(0, j, width, j + 10, buf)
        if height % 10 != 0:
            tft.bitmap(
                0,
                height - height % 10,
                width,
                height,
                buf
            )

main()
