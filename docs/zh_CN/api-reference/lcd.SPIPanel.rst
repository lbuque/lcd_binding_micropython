class SPIPanel -- SPI Interfaced LCD
=====================================

SPIPanel 类实现了 SPI 串行行总线的通用控制和访问接口，可以通过它和LCD驱动芯片进行交互。

Constructors
------------

.. class:: SPIPanel(spi: machine.SPI, dc: Pin, cs: Pin=None, pclk: int=10000000, width: int=240, height: int=240, swap_color_bytes: bool=False, cmd_bits: int=8, param_bits: int=8)

    创建一个 SPIPanel 的对象。

    这些参数是：

        - ``spi``: SPI 对象
        - ``dc``: 用于 D/C 线的引脚对象
        - ``cs``: 用于 CS 线的引脚对象，如果不使用则设置为 None
        - ``pclk``: 像素时钟频率
        - ``width``: 水平分辨率，即一行中的像素数
        - ``height``: 垂直分辨率，即帧中的行数
        - ``swap_color_bytes``: 交换相邻的两个颜色字节
        - ``cmd_bits``: LCD 命令的位宽
        - ``param_bits``: LCD参数位宽

Methods
-------

.. method:: SPIPanel.tx_param(cmd: int, param: bytes=None)

    发送 LCD 命令和相应的参数。

    - cmd - 具体的 LCD 命令
    - param - 保存命令特定参数的缓冲区，如果命令不需要参数，则不需要传入

.. method:: SPIPanel.tx_color(cmd: int, color: bytes=None)

    发送 LCD RGB 数据。

    - cmd - 具体的 LCD 命令
    - param - 保存 RGB 颜色数据的缓冲区

.. method:: SPIPanel.deinit()

    销毁 SPIPanel 对象。
