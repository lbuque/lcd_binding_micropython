class ST7735 -- Sitronix ST7735 abstract classes
=================================================

ST7735 类是 Sitronix ST7735 硬件抽象类，它实现了ST7735的一些通用接口。

Constructors
------------

.. class:: ST7735(bus: I8080, reset: Pin=None, backlight: Pin=None, reset_level: bool=false, color_space: int=RGB, bpp: int=16)

    创建一个ST7735硬件抽象类。

    这些参数是：

        - ``bus``: I8080 对象
        - ``reset``: 用于重置 LCD 面板的引脚，如果不使用则设置为 None
        - ``backlight``: 背光控制引脚
        - ``reset_level``: 如果面板复位为高电平有效，则设置此项
        - ``color_space``: 设置 LCD 面板使用的色彩空间
        - ``bpp``: 颜色深度，以 bpp 为单位

Methods
-------

.. method:: ST7735.reset()

    重置液晶面板。

.. method:: ST7735.init()

    初始化液晶面板。

    .. note::

        在调用此方法之前，请确保 LCD 面板已通过 RGB.reset() 完成重置阶段。

.. method:: ST7735.bitmap(x_start: int, y_start: int, x_end: int, y_end: int, color_data: bytes=None)

    在 LCD 面板上绘制位图。

    参数如下：

    - ``x_start`` - x 轴上的起始索引（包括 x_start）
    - ``y_start`` - y 轴上的起始索引（包括 y_start）
    - ``x_end`` - x 轴上的结束索引（不包括 x_end）
    - ``y_end`` - y 轴上的结束索引（不包括 y_end）
    - ``color_data`` - 将转储到特定窗口范围的 RGB 颜色数据

.. method:: ST7735.mirror(mirror_x: bool, mirror_y: bool)

    在特定轴上镜像 LCD 面板。

    参数如下：

    - ``mirror_x`` - 面板是否将围绕 x 轴镜像
    - ``mirror_y`` - 面板是否将围绕 y 轴镜像

    .. note::

        结合RGB.swap_xy()，可以实现屏幕旋转

.. method:: ST7735.swap_xy(swap_axes: bool)

    交换 x 和 y 轴。

    - ``swap_axes`` - 是否交换x和y轴

    .. note::

        结合RGB.mirror()，可以实现屏幕旋转

.. method:: ST7735.set_gap(x_gap: int, y_gap: int)

    在 x 和 y 轴上设置额外的间隙。

    间隙是液晶面板左侧/顶部与实际显示内容的第一行/列之间的空间（以像素为单位）。

    参数如下：

    - ``x_gap`` - x 轴上的额外间隙，以像素为单位
    - ``y_gap`` - y 轴上的额外间隙，以像素为单位

    .. note::

        在定位或居中小于 LCD 的框架时，设置间隙非常有用。

.. method:: ST7735.invert_color(invert_color_data: bool)

    反转颜色（按位反转颜色数据线）

    - ``invert_color_data`` - 是否反转颜色数据

.. method:: ST7735.disp_off(off: bool)

    关闭显示器。

    - ``off`` - 是否关闭屏幕

.. method:: ST7735.backlight_on()

    打开背光

.. method:: ST7735.backlight_off()

    关闭背光

.. method:: ST7735.deint()

    初始化 LCD 面板。

.. method:: ST7735.rotation(r: int, rotations: None | tuple)

    设置以逆时针方向旋转逻辑显示。

    ``r`` 参数只接受以下的值：

        - ``0``: Portrait (0°C)
        - ``1``: Landscape (90°C)
        - ``2``: Inverse Portrait (180°C)
        - ``3``: Inverse Landscape (270°C)

    ``rotations`` 设置方向表。 方向表是用于设置 MADCTL 寄存器、显示宽度、显示高度、start_x 和 start_y 值的每个“旋转”的元组列表。

        +---------+----------------------------------------------------------------------------------------------------------+
        | Display | Default Orientation Tables                                                                               |
        +=========+==========================================================================================================+
        | 160x80  | ((0x00, 80, 160, 26, 1), (0x60, 160, 80, 1, 26), (0xC0, 80, 160, 26, 1), (0xA0, 80, 160, 1, 26))         |
        +---------+----------------------------------------------------------------------------------------------------------+
        | other   | ((0x00, 0, 0, 0, 0), (0x60, 0, 0, 0, 0), (0xC0, 0, 0, 0, 0), (0xA0, 0, 0, 0, 0))                         |
        +---------+----------------------------------------------------------------------------------------------------------+

.. method:: ILI9488.vscroll_area(tfa: int, height: int, bfa: int)

    设置垂直滚动参数。

    - ``tfa`` 是以像素为单位的顶部固定区域。 顶部固定区域是不会滚动的显示帧缓冲区的上部。

    - ``height`` 是滚动区域的总高度（以像素为单位）。

    - ``bfa`` 是以像素为单位的底部固定区域。 底部固定区域是显示帧缓冲区的下部，不会滚动。

.. method:: ILI9488.vscroll_start(address: int, order: bool=False)

    设置垂直滚动地址。

    - ``address`` 是以像素为单位的垂直滚动起始地址。 垂直滚动起始地址是帧缓冲区中的行，将是 TFA 之后显示的第一行。
    - ``order`` 是垂直刷新顺序。 当``order`` == ``False``时，LCD垂直刷新从上到下； 当``order`` == ``False``时，LCD垂直刷新从下到上。
