class I8080 -- Intel 8080 parallel LCD
=======================================

The I8080 class implements the general control and access interface of the I8080 parallel bus, and can interact with the LCD driver chip through it.

Constructors
------------

.. class:: I8080(data: tuple=None, command: Pin=None, write: Pin=None, read: Pin=None, cs: Pin=None, pclk: int= 10000000，width: int=240, height: int=240, swap_color_bytes: bool=False, cmd_bits: int=8, param_bits: int=8)

    Create an Intel 8080 object.

    These parameters are:

        - ``data``: Pins used for data lines
        - ``command``: Pin used for D/C line
        - ``write``: Pin used for WR line
        - ``read``: Pin used for RD line, set to None if it’s not used
        - ``cs``: Pin used for CS line, set to None if it’s not used
        - ``pclk``: Frequency of pixel clock
        - ``width``: Horizontal resolution, i.e. the number of pixels in a line
        - ``height``: Vertical resolution, i.e. the number of lines in the frame
        - ``swap_color_bytes``: Swap adjacent two color bytes
        - ``cmd_bits``: Bit-width of LCD command
        - ``param_bits``: Bit-width of LCD parameter

Methods
-------

.. method:: I8080.tx_param(cmd: int, param: bytes=None)

    Transmit LCD command and corresponding parameters.

    - cmd - The specific LCD command
    - param - Buffer that holds the command specific parameters

.. method:: I8080.tx_color(cmd: int, color: bytes=None)

    Transmit LCD RGB data.

    - cmd - The specific LCD command
    - param - Buffer that holds the RGB color data

.. method:: deinit()

    Destroys the Intel 8080 bus object.
