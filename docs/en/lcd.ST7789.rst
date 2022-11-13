class ST7789 -- Sitronix ST7789 abstract classes
=================================================

The ST7789 class is the Sitronix ST7789 hardware abstraction class, which implements some common interfaces of the ST7789.

Constructors
------------

.. class:: ST7789(bus: I8080, reset: Pin=None, backlight: Pin=None, reset_level: bool=false, color_space: int=RGB, bits_per_pixel: int=16)

    Create an ST7789 hardware abstraction class.

    These parameters are:

        - ``bus``: I8080 obj
        - ``reset``: Pin used to reset the LCD panel, set to None if it’s not used
        - ``backlight``: Pin used for backlight signal
        - ``reset_level``: Setting this if the panel reset is high level active
        - ``color_space``: Set the color space used by the LCD panel
        - ``bits_per_pixel``: Color depth, in bpp

Methods
-------

.. method:: ST7789.reset()

    Reset LCD panel.

.. method:: ST7789.init()

    Initialize LCD panel.

    .. note::

        Before calling this method, make sure the LCD panel has finished the reset stage by ST7789.reset().

.. method:: ST7789.bitmap(x_start, y_start, x_end, y_end, color_data)

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

.. method:: ST7789.color565(red: int, green: int, blue: int)

    Convert RGB888 color data to RGB565.

.. method:: ST7789.deint()

    Deinitialize the LCD panel.

.. method:: rotation(r: int)

    Set the rotates the logical display in a counter-clockwise direction.

    - ``0``: Portrait (0 degrees)
    - ``1``: Landscape (90 degrees)
    - ``2``: Inverse Portrait (180 degrees)
    - ``3``: Inverse Landscape (270 degrees)

Constants
---------

.. data:: RGB

    Color space: RGB

.. data:: BGR

    Color space: BGR

.. data:: MONOCHROME

    Color space: monochrome
