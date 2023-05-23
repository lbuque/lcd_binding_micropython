class SPIPanel -- SPI Interfaced LCD
=====================================

The SPIPanel class implements the general control and access interface of the SPI serial bus, and can interact with the LCD driver chip through it.

Constructors
------------

.. class:: SPIPanel(spi: machine.SPI, dc: Pin, cs: Pin=None, pclk: int=10000000, width: int=240, height: int=240, cmd_bits: int=8, param_bits: int=8)

    Create an SPIPanel object.

    These parameters are:

        - ``spi``: SPI object
        - ``dc``: Pins used for data lines
        - ``cs``: Pin used for CS line, set to None if it’s not used
        - ``pclk``: Frequency of pixel clock
        - ``width``: Horizontal resolution, i.e. the number of pixels in a line
        - ``height``: Vertical resolution, i.e. the number of lines in the frame
        - ``cmd_bits``: Bit-width of LCD command
        - ``param_bits``: Bit-width of LCD parameter

Methods
-------

.. method:: SPIPanel.tx_param(cmd: int, param: bytes=None)

    Transmit LCD command and corresponding parameters.

    - cmd - The specific LCD command
    - param - Buffer that holds the command specific parameters

.. method:: SPIPanel.tx_color(cmd: int, color: bytes=None)

    Transmit LCD RGB data.

    - cmd - The specific LCD command
    - param - Buffer that holds the RGB color data

.. method:: SPIPanel.deinit()

    Destroys SPIPanel object.
