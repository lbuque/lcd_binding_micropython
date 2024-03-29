class ST7789 -- Sitronix ST7789 abstract classes
=================================================

The ST7789 class is the Sitronix ST7789 hardware abstraction class, which implements some common interfaces of the ST7789.

Constructors
------------

.. class:: ST7789(bus: I8080, reset: Pin=None, backlight: Pin=None, reset_level: bool=false, color_space: int=RGB, bpp: int=16)

    Create an ST7789 hardware abstraction class.

    These parameters are:

        - ``bus``: I8080 obj
        - ``reset``: Pin used to reset the LCD panel, set to None if it’s not used
        - ``backlight``: Pin used for backlight signal
        - ``reset_level``: Setting this if the panel reset is high level active
        - ``color_space``: Set the color space used by the LCD panel
        - ``bpp``: Color depth, in bpp

Methods
-------

.. method:: ST7789.reset()

    Reset LCD panel.

.. method:: ST7789.init()

    Initialize LCD panel.

    .. note::

        Before calling this method, make sure the LCD panel has finished the reset stage by ST7789.reset().

.. method:: ST7789.bitmap(x_start: int, y_start: int, x_end: int, y_end: int, color_data: bytes=None)

    Draw bitmap on LCD panel.

    The arguments are:

    - ``x_start`` - Start index on x-axis (x_start included)
    - ``y_start`` - Start index on y-axis (y_start included)
    - ``x_end`` - End index on x-axis (x_end not included)
    - ``y_end`` - End index on y-axis (y_end not included)
    - ``color_data`` - RGB color data that will be dumped to the specific window range

.. method:: ST7789.mirror(mirror_x: bool, mirror_y: bool)

    Mirror the LCD panel on specific axis.

    The arguments are:

    - ``mirror_x`` - Whether the panel will be mirrored about the x axis
    - ``mirror_y`` - Whether the panel will be mirrored about the y axis

    .. note::

        Combined with ST7789.swap_xy(), one can realize screen rotation

.. method:: ST7789.swap_xy(swap_axes: bool)

    Swap/Exchange x and y axis.

    - ``swap_axes`` - Whether to swap the x and y axis

    .. note::

        Combined with ST7789.mirror(), one can realize screen rotation

.. method:: ST7789.set_gap(x_gap: int, y_gap: int)

    Set extra gap in x and y axis.

    The gap is the space (in pixels) between the left/top sides of the LCD panel and the first row/column respectively of the actual contents displayed.

    The arguments are:

    - ``x_gap`` - Extra gap on x axis, in pixels
    - ``y_gap`` - Extra gap on y axis, in pixels

    .. note::

        Setting a gap is useful when positioning or centering a frame that is smaller than the LCD.

.. method:: ST7789.invert_color(invert_color_data: bool)

    Invert the color (bit-wise invert the color data line)

    - ``invert_color_data`` - Whether to invert the color data

.. method:: ST7789.disp_off(off: bool)

    Turn off the display.

    - ``off`` - Whether to turn off the screen

.. method:: ST7789.backlight_on()

    Turn on the backlight

.. method:: ST7789.backlight_off()

    turn off the backlight.

.. method:: ST7789.deint()

    Deinitialize the LCD panel.

.. method:: ST7789.rotation(r: int)

    Set the rotates the logical display in a counter-clockwise direction.

    The ``r`` parameter accepts only the following values:

        - ``0``: Portrait (0 degrees)
        - ``1``: Landscape (90 degrees)
        - ``2``: Inverse Portrait (180 degrees)
        - ``3``: Inverse Landscape (270 degrees)

    ``rotations`` sets the orientation table. The orientation table is a list of
    tuples for each ``rotation`` used to set the MADCTL register, display width,
    display height, start_x, and start_y values.

        +---------+----------------------------------------------------------------------------------------------------------+
        | Display | Default Orientation Tables                                                                               |
        +=========+==========================================================================================================+
        | 240x320 | ((0x00, 240, 320, 0, 0), (0x60, 320, 240, 0, 0), (0xC0, 240, 320, 0, 0), (0xA0, 320, 240, 0, 0))         |
        +---------+----------------------------------------------------------------------------------------------------------+
        | 170x320 | ((0x00, 170, 320, 35, 0), (0x60, 320, 170, 0, 35), (0xC0, 170, 320, 35, 0), (0xA0, 320, 170, 0, 35))     |
        +---------+----------------------------------------------------------------------------------------------------------+
        | 240x240 | ((0x00, 240, 240, 0, 0), (0x60, 240, 240, 0, 0), (0xC0, 240, 240, 0, 80), (0xA0, 240, 240, 80, 0))       |
        +---------+----------------------------------------------------------------------------------------------------------+
        | 135x240 | ((0x00, 135, 240, 52, 40), (0x60, 240, 135, 40, 53), (0xC0, 135, 240, 53, 40), (0xA0, 240, 135, 40, 52)) |
        +---------+----------------------------------------------------------------------------------------------------------+
        | 128x160 | ((0x00, 128, 160, 0, 0), (0x60, 160, 128, 0, 0), (0xC0, 128, 160, 0, 0), (0xA0, 160, 128, 0, 0))         |
        +---------+----------------------------------------------------------------------------------------------------------+
        | 128x128 | ((0x00, 128, 128, 2, 1), (0x60, 128, 128, 1, 2), (0xC0, 128, 128, 2, 3), (0xA0, 128, 128, 3, 2))         |
        +---------+----------------------------------------------------------------------------------------------------------+
        | other   | ((0x00, 0, 0, 0, 0), (0x60, 0, 0, 0, 0), (0xC0, 0, 0, 0, 0), (0xA0, 0, 0, 0, 0))                         |
        +---------+----------------------------------------------------------------------------------------------------------+

.. method:: ST7789.vscroll_area(tfa: int, height: int, bfa: int)

    Set the vertical scrolling parameters.

    - ``tfa`` is the top fixed area in pixels. The top fixed area is the upper portion of the display frame buffer that will not be scrolled.

    - ``height`` is the total height in pixels of the area scrolled.

    - ``bfa`` is the bottom fixed area in pixels. The bottom fixed area is the lower portion of the display frame buffer that will not be scrolled.

.. method:: ST7789.vscroll_start(address: int, order: bool=False)

    Set the vertical scroll address.

    - ``address`` is the vertical scroll start address in pixels. The vertical scroll start address is the line in the frame buffer will be the first line shown after the TFA.
    - ``order`` is the Vertical Refresh Order. When ``order`` == ``False``, LCD vertical refresh Top to Bottom; When ``order`` == ``False``, LCD vertical refresh Bottom to Top.
