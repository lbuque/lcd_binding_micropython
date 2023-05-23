from machine import Pin, SPI
import lcd

TFA = const(0)
BFA = const(0)


# This is the command sequence that initialises the GC9A01 driver
# cmd(int) args(bytes) delay(ms)
init_tft_esp = (
    (0xEF, b'', 0),
    (0xEB, b'\x14', 0),
    (0xFE, b'', 0),
    (0xEF, b'', 0),
    (0xEB, b'\x14', 0),
    (0x84, b'\x40', 0),
    (0x85, b'\xFF', 0),
    (0x86, b'\xFF', 0),
    (0x87, b'\xFF', 0),
    (0x88, b'\x0A', 0),
    (0x89, b'\x21', 0),
    (0x8A, b'\x00', 0),
    (0x8B, b'\x80', 0),
    (0x8C, b'\x01', 0),
    (0x8D, b'\x01', 0),
    (0x8E, b'\xFF', 0),
    (0x8F, b'\xFF', 0),
    (0xB6, b'\x00\x20', 0),
    (0x90, b'\x08\x08\x08\x08', 0),
    (0xBD, b'\x06', 0),
    (0xBC, b'\x00', 0),
    (0xFF, b'\x60\x01\x04', 0),
    (0xC3, b'\x13\xC4\x13', 0),
    (0xC9, b'\x22', 0),
    (0xBE, b'\x11', 0),
    (0xE1, b'\x10\x0E', 0),
    (0xDF, b'\x21\x0c\x02', 0),
    (0xF0, b'\x45\x09\x08\x08\x26\x2A', 0),
    (0xF1, b'\x43\x70\x72\x36\x37\x6F', 0),
    (0xF2, b'\x45\x09\x08\x08\x26\x2A', 0),
    (0xF3, b'\x43\x70\x72\x36\x37\x6F', 0),
    (0xED, b'\x1B\x0B', 0),
    (0xAE, b'\x77', 0),
    (0xCD, b'\x63', 0),
    (0x70, b'\x07\x07\x04\x0E\x0F\x09\x07\x08\x03', 0),
    (0xE8, b'\x34', 0),
    (0x62, b'\x18\x0D\x71\xED\x70\x70\x18\x0F\x71\xEF\x70\x70', 0),
    (0x63, b'\x18\x11\x71\xF1\x70\x70\x18\x13\x71\xF3\x70\x70', 0),
    (0x64, b'\x28\x29\xF1\x01\xF1\x00\x07', 0),
    (0x66, b'\x3C\x00\xCD\x67\x45\x45\x10\x00\x00\x00', 0),
    (0x67, b'\x00\x3C\x00\x00\x00\x01\x54\x10\x32\x98', 0),
    (0x74, b'\x10\x85\x80\x00\x00\x4E\x00', 0),
    (0x98, b'\x3E\x07', 0),
    (0x35, b'', 0),

#     (0x21, b'', 0),
#     (0x36, b'\x08', 0),
#     (0x3A, b'\x05', 0),
#     (0x11, b'', 120),
#     (0x29, b'', 20),
)

def config():
    hspi = SPI(2, sck=Pin(14), mosi=Pin(13), miso=None)
    panel = lcd.SPIPanel(spi=hspi, dc=Pin(16), cs=Pin(15), pclk=60000000, width=240, height=240)
    st = lcd.GC9A01(panel, reset=Pin(17), backlight=Pin(18), color_space=lcd.BGR)
    st.backlight_on()
    st.reset()
    st.custom_init(init_tft_esp)
    st.init()
    st.invert_color(True)
    return st


def color565(r, g, b):
    c = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3)
    return (c >> 8) | (c << 8)
