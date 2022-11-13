class RGB -- RGB Interface
===========================

The RGB class is the RGB parallel interface hardware abstract class, which implements some common interfaces of RGB.

Constructors
------------

.. class:: RGB(data: tuple, hsync: Pin, vsync: Pin, de: Pin, pclk_pin: Pin, timings: tuple, disp: Pin=None, backlight: Pin=None, pclk: int=8000000, width: int=480, height: int=480)

    Create an RGB hardware abstract class.

    These parameters are:

        - ``data``: Tuple of data bus, currently only supports 16-bit RGB interface
        - ``hsync``: Pin used for hsync signal
        - ``vsync``: Pin used for vsync signal
        - ``de``: Pin used for DE signal, set to None if it’s not used
        - ``pclk_pin``: Pin used for PCLK signal
        - ``timings``: RGB timing parameters
        - ``disp``: Pin used for display control signal, set to None if it’s not used
        - ``backlight``: Pin used for backlight signal
        - ``pclk``: Frequency of pixel clock
        - ``width``: Horizontal resolution, i.e. the number of pixels in a line
        - ``height``: Vertical resolution, i.e. the number of lines in the frame


    The ``timing`` must be a 6-tuple in the following format:

        (hsync_pulse_width, hsync_back_porch, hsync_front_porch, vsync_pulse_width, vsync_back_porch, vsync_front_porch)

        - ``hsync_pulse_width``: Horizontal sync width, unit: PCLK period
        - ``hsync_back_porch``: Horizontal back porch, number of PCLK between hsync and start of line active data
        - ``hsync_front_porch``: Horizontal front porch, number of PCLK between the end of active data and the next hsync
        - ``vsync_pulse_width``: Vertical sync width, unit: number of lines
        - ``vsync_back_porch``: Vertical back porch, number of invalid lines between vsync and start of frame
        - ``vsync_front_porch``: Vertical front porch, number of invalid lines between the end of frame and the next vsync

Methods
-------

.. method:: RGB.reset()

    Reset LCD panel.

.. method:: RGB.init()

    Initialize LCD panel.

    .. note::

        Before calling this method, make sure the LCD panel has finished the reset stage by RGB.reset().

.. method:: RGB.bitmap(x_start, y_start, x_end, y_end, color_data)

    Draw bitmap on LCD panel.

    The arguments are:

    - ``x_start`` - Start index on x-axis (x_start included)
    - ``y_start`` - Start index on y-axis (y_start included)
    - ``x_end`` - End index on x-axis (x_end not included)
    - ``y_end`` - End index on y-axis (y_end not included)
    - ``color_data`` - RGB color data that will be dumped to the specific window range

.. method:: RGB.mirror(mirror_x: bool, mirror_y: bool)

    Mirror the LCD panel on specific axis.

    The arguments are:

    - ``mirror_x`` - Whether the panel will be mirrored about the x axis
    - ``mirror_y`` - Whether the panel will be mirrored about the y axis

    .. note::

        Combined with RGB.swap_xy(), one can realize screen rotation

.. method:: RGB.swap_xy(swap_axes: bool)

    Swap/Exchange x and y axis.

    - ``swap_axes`` - Whether to swap the x and y axis

    .. note::

        Combined with RGB.mirror(), one can realize screen rotation

.. method:: RGB.set_gap(x_gap: int, y_gap: int)

    Set extra gap in x and y axis.

    The gap is the space (in pixels) between the left/top sides of the LCD panel and the first row/column respectively of the actual contents displayed.

    The arguments are:

    - ``x_gap`` - Extra gap on x axis, in pixels
    - ``y_gap`` - Extra gap on y axis, in pixels

    .. note::

        Setting a gap is useful when positioning or centering a frame that is smaller than the LCD.

.. method:: RGB.invert_color(invert_color_data: bool)

    Invert the color (bit-wise invert the color data line)

    - ``invert_color_data`` - Whether to invert the color data

.. method:: RGB.disp_off(off: bool)

    Turn off the display.

    - ``off`` - Whether to turn off the screen

.. method:: RGB.backlight_on()

    Turn on the backlight.

.. method:: RGB.backlight_off()

    turn off the backlight.

.. method:: RGB.deint()

    Deinitialize the LCD panel.
