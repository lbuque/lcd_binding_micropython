import tft_config
import logo
import gc
import time

def main():
    tft = tft_config.config()
    tft.bitmap(0, 0, logo.WIDTH, logo.HEIGHT, logo.BITMAP)
    time.sleep(0.1)
    del tft

gc.enable()
main()
gc.collect()