class I8080 -- Intel 8080 parallel LCD
=======================================

I8080类实现了 I8080 并行总线的通用控制和访问接口，可以通过它和LCD驱动芯片进行交互。

Constructors
------------

.. class:: I8080(data: tuple=None, command: Pin=None, write: Pin=None, read: Pin=None, cs: Pin=None, pclk: int= 10000000，width: int=240, height: int=240, swap_color_bytes: bool=False, cmd_bits: int=8, param_bits: int=8)

    创建一个Intel 8080的对象。

    这些参数是：

        - ``data``: 用于数据线的引脚对象元组
        - ``command``: 用于 D/C 线的引脚对象
        - ``write``: 用于 WR 线的引脚对象
        - ``read``: RD 线使用的引脚对象，如果不使用则设置为None
        - ``cs``: 用于 CS 线的引脚对象，如果不使用则设置为 None
        - ``pclk``: 像素时钟频率
        - ``width``: 水平分辨率，即一行中的像素数
        - ``height``: 垂直分辨率，即帧中的行数
        - ``swap_color_bytes``: 交换相邻的两个颜色字节
        - ``cmd_bits``: LCD 命令的位宽
        - ``param_bits``: LCD参数位宽

Methods
-------

.. method:: I8080.tx_param(cmd: int, param: bytes=None)

    发送 LCD 命令和相应的参数。

    - cmd - 具体的 LCD 命令
    - param - 保存命令特定参数的缓冲区，如果命令不需要参数，则不需要传入

.. method:: I8080.tx_color(cmd: int, color: bytes=None)

    发送 LCD RGB 数据。

    - cmd - 具体的 LCD 命令
    - param - 保存 RGB 颜色数据的缓冲区

.. method:: deinit()

    销毁 Intel 8080总线对象。
