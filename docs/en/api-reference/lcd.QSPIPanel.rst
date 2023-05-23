class QSPIPanel -- SPI Interfaced LCD
=====================================

The QSPIPanel class implements the general control and access interface of the SPI serial bus, and can interact with the LCD driver chip through it.

Constructors
------------

.. class:: QSPIPanel(spi: machine.SPI, data: tuple, dc: Pin, cs: Pin=None, pclk: int=10000000, width: int=240, height: int=240, cmd_bits: int=8, param_bits: int=8)

    Create an QSPIPanel object.

    These parameters are:

        - ``spi``: SPI object
        - ``data``: Pins used for data lines
        - ``dc``: Pins used for data lines
        - ``cs``: Pin used for CS line, set to None if it’s not used
        - ``pclk``: Frequency of pixel clock
        - ``width``: Horizontal resolution, i.e. the number of pixels in a line
        - ``height``: Vertical resolution, i.e. the number of lines in the frame
        - ``cmd_bits``: Bit-width of LCD command
        - ``param_bits``: Bit-width of LCD parameter

Methods
-------

.. method:: QSPIPanel.tx_param(cmd: int, param: bytes=None)

    Transmit LCD command and corresponding parameters.

    - cmd - The specific LCD command
    - param - Buffer that holds the command specific parameters

.. method:: QSPIPanel.tx_color(cmd: int, color: bytes=None)

    Transmit LCD RGB data.

    - cmd - The specific LCD command
    - param - Buffer that holds the RGB color data

.. method:: QSPIPanel.deinit()

    Destroys QSPIPanel object.
