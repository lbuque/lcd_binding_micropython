import machine
import tft_config
import gc
import framebuf

def color565(r, g, b):
    return ((r & 0xF8) << 8) | \
           ((g & 0xFC) << 3) | \
           ((b & 0xF8) >> 3)

def hsv2rgb(hue, sat, val):
    '''The conversion algorithm comes from https://blog.csdn.net/lly_3485390095/article/details/104570885'''
    C = 0.0
    X = 0.0
    Y = 0.0
    Z = 0.0
    i = 0
    H = float(hue)
    S = sat/100.0
    V = val/100.0
    if int(S) == 0:
        return int(V*255), int(V*255), int(V*255)
    else:
        H = H/60
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

if __name__ == "__main__":
    gc.enable()
    rgb = tft_config.config()
    rgb.backlight_on()

    buf = bytearray(480 * 480 * 2)
    fbuf = framebuf.FrameBuffer(buf, 480, 480, framebuf.RGB565)
    while True:
        for i in range(0, 360, 1):
            r, g, b = hsv2rgb(i, 255, 100)
            fbuf.fill(color565(r, g, b))
            rgb.bitmap(0, 0, 480, 480, buf)
