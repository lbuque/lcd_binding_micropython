class ST7789 -- Sitronix ST7789 abstract classes
=================================================

ST7789 类是 Sitronix ST7789 硬件抽象类，它实现了ST7789的一些通用接口。

Constructors
------------

.. class:: ST7789(bus: I8080, reset: Pin=None, backlight: Pin=None, reset_level: bool=false, color_space: int=RGB, bits_per_pixel: int=16)

    创建一个ST7789硬件抽象类。

    这些参数是：

        - ``bus``: I8080 对象
        - ``reset``: 用于重置 LCD 面板的引脚，如果不使用则设置为 None
        - ``backlight``: 背光控制引脚
        - ``reset_level``: 如果面板复位为高电平有效，则设置此项
        - ``color_space``: 设置 LCD 面板使用的色彩空间
        - ``bits_per_pixel``: 颜色深度，以 bpp 为单位

Methods
-------

.. method:: ST7789.reset()

    重置液晶面板。

.. method:: ST7789.init()

    初始化液晶面板。

    .. note::

        在调用此方法之前，请确保 LCD 面板已通过 RGB.reset() 完成重置阶段。

.. method:: ST7789.bitmap(x_start, y_start, x_end, y_end, color_data)

    在 LCD 面板上绘制位图。

    参数如下：

    - ``x_start`` - x 轴上的起始索引（包括 x_start）
    - ``y_start`` - y 轴上的起始索引（包括 y_start）
    - ``x_end`` - x 轴上的结束索引（不包括 x_end）
    - ``y_end`` - y 轴上的结束索引（不包括 y_end）
    - ``color_data`` - 将转储到特定窗口范围的 RGB 颜色数据

.. method:: ST7789.mirror(mirror_x: bool, mirror_y: bool)

    在特定轴上镜像 LCD 面板。

    参数如下：

    - ``mirror_x`` - 面板是否将围绕 x 轴镜像
    - ``mirror_y`` - 面板是否将围绕 y 轴镜像

    .. note::

        结合RGB.swap_xy()，可以实现屏幕旋转

.. method:: ST7789.swap_xy(swap_axes: bool)

    交换 x 和 y 轴。

    - ``swap_axes`` - 是否交换x和y轴

    .. note::

        结合RGB.mirror()，可以实现屏幕旋转

.. method:: ST7789.set_gap(x_gap: int, y_gap: int)

    在 x 和 y 轴上设置额外的间隙。

    间隙是液晶面板左侧/顶部与实际显示内容的第一行/列之间的空间（以像素为单位）。

    参数如下：

    - ``x_gap`` - x 轴上的额外间隙，以像素为单位
    - ``y_gap`` - y 轴上的额外间隙，以像素为单位

    .. note::

        在定位或居中小于 LCD 的框架时，设置间隙非常有用。

.. method:: ST7789.invert_color(invert_color_data: bool)

    反转颜色（按位反转颜色数据线）

    - ``invert_color_data`` - 是否反转颜色数据

.. method:: ST7789.disp_off(off: bool)

    关闭显示器。

    - ``off`` - 是否关闭屏幕

.. method:: ST7789.backlight_on()

    打开背光

.. method:: ST7789.backlight_off()

    关闭背光

.. method:: ST7789.color565(red: int, green: int, blue: int)

    将 RGB888 颜色数据转为 RGB565。

.. method:: ST7789.deint()

    初始化 LCD 面板。

.. method:: rotation(r: int)

    设置以逆时针方向旋转逻辑显示。

    - ``0``: Portrait (0°C)
    - ``1``: Landscape (90°C)
    - ``2``: Inverse Portrait (180°C)
    - ``3``: Inverse Landscape (270°C)

Constants
---------

.. data:: RGB

    色彩空间顺序: RGB

.. data:: BGR

    色彩空间顺序: BGR

.. data:: MONOCHROME

    色彩空间顺序: monochrome
