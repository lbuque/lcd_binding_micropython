class RM67162 -- Raydium RM67162 abstract classes
=================================================

The RM67162 class is the Raydium RM67162 hardware abstraction class, which implements some common interfaces of the RM67162.

Constructors
------------

.. class:: RM67162(bus: I8080, reset: Pin=None, backlight: Pin=None, reset_level: bool=false, color_space: int=RGB, bpp: int=16)

    Create an RM67162 hardware abstraction class.

    These parameters are:

        - ``bus``: I8080 obj
        - ``reset``: Pin used to reset the LCD panel, set to None if it’s not used
        - ``backlight``: Pin used for backlight signal
        - ``reset_level``: Setting this if the panel reset is high level active
        - ``color_space``: Set the color space used by the LCD panel
        - ``bpp``: Color depth, in bpp

Methods
-------

.. method:: RM67162.reset()

    Reset LCD panel.

.. method:: RM67162.init()

    Initialize LCD panel.

    .. note::

        Before calling this method, make sure the LCD panel has finished the reset stage by RM67162.reset().

.. method:: RM67162.bitmap(x_start: int, y_start: int, x_end: int, y_end: int, color_data: bytes=None)

    Draw bitmap on LCD panel.

    The arguments are:

    - ``x_start`` - Start index on x-axis (x_start included)
    - ``y_start`` - Start index on y-axis (y_start included)
    - ``x_end`` - End index on x-axis (x_end not included)
    - ``y_end`` - End index on y-axis (y_end not included)
    - ``color_data`` - RGB color data that will be dumped to the specific window range

.. method:: RM67162.mirror(mirror_x: bool, mirror_y: bool)

    Mirror the LCD panel on specific axis.

    The arguments are:

    - ``mirror_x`` - Whether the panel will be mirrored about the x axis
    - ``mirror_y`` - Whether the panel will be mirrored about the y axis

    .. note::

        Combined with RM67162.swap_xy(), one can realize screen rotation

.. method:: RM67162.swap_xy(swap_axes: bool)

    Swap/Exchange x and y axis.

    - ``swap_axes`` - Whether to swap the x and y axis

    .. note::

        Combined with RM67162.mirror(), one can realize screen rotation

.. method:: RM67162.set_gap(x_gap: int, y_gap: int)

    Set extra gap in x and y axis.

    The gap is the space (in pixels) between the left/top sides of the LCD panel and the first row/column respectively of the actual contents displayed.

    The arguments are:

    - ``x_gap`` - Extra gap on x axis, in pixels
    - ``y_gap`` - Extra gap on y axis, in pixels

    .. note::

        Setting a gap is useful when positioning or centering a frame that is smaller than the LCD.

.. method:: RM67162.invert_color(invert_color_data: bool)

    Invert the color (bit-wise invert the color data line)

    - ``invert_color_data`` - Whether to invert the color data

.. method:: RM67162.disp_off(off: bool)

    Turn off the display.

    - ``off`` - Whether to turn off the screen

.. method:: RM67162.backlight_on()

    Turn on the backlight

.. method:: RM67162.backlight_off()

    turn off the backlight.

.. method:: RM67162.backlight(brightness: int)

    Adjust the brightness of the backlight.

.. method:: RM67162.deint()

    Deinitialize the LCD panel.

.. method:: RM67162.rotation(r: int)

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
        | 240x536 | ((0x00, 240, 536, 0, 0), (0x60, 536, 240, 0, 0), (0xC0, 240, 536, 0, 0), (0xA0, 536, 240, 0, 0))         |
        +---------+----------------------------------------------------------------------------------------------------------+
        | other   | ((0x00, 0, 0, 0, 0), (0x60, 0, 0, 0, 0), (0xC0, 0, 0, 0, 0), (0xA0, 0, 0, 0, 0))                         |
        +---------+----------------------------------------------------------------------------------------------------------+

.. method:: RM67162.vscroll_area(tfa: int, height: int, bfa: int)

    Set the vertical scrolling parameters.

    - ``tfa`` is the top fixed area in pixels. The top fixed area is the upper portion of the display frame buffer that will not be scrolled.

    - ``height`` is the total height in pixels of the area scrolled.

    - ``bfa`` is the bottom fixed area in pixels. The bottom fixed area is the lower portion of the display frame buffer that will not be scrolled.

.. method:: RM67162.vscroll_start(address: int, order: bool=False)

    Set the vertical scroll address.

    - ``address`` is the vertical scroll start address in pixels. The vertical scroll start address is the line in the frame buffer will be the first line shown after the TFA.
    - ``order`` is the Vertical Refresh Order. When ``order`` == ``False``, LCD vertical refresh Top to Bottom; When ``order`` == ``False``, LCD vertical refresh Bottom to Top.
