class RGB -- RGB Interface
===========================

RGB 类是 RGB并行接口硬件抽象类，它实现了RGB的一些通用接口。

Constructors
------------

.. class:: RGB(data: tuple, hsync: Pin, vsync: Pin, de: Pin, pclk_pin: Pin, timings: tuple, disp: Pin=None, backlight: Pin=None, pclk: int=8000000, width: int=480, height: int=480)

    创建一个 RGB 硬件抽象类。

    这些参数是：

        - ``data``: 数据总线的元组，目前只支持 16-bit RGB 接口
        - ``hsync``: 垂直同步引脚对象
        - ``vsync``: 水平同步引脚对象
        - ``de``: 用于 DE 信号的引脚，如果不使用则设置为 None
        - ``pclk_pin``: 用于 PCLK 信号的引脚对象
        - ``timings``: RGB时序参数
        - ``disp``: 用于显示控制信号的引脚对象，如果不使用则设置为 None
        - ``backlight``: 背光控制引脚对象
        - ``pclk``: 像素时钟频率
        - ``width``: 水平分辨率，即一行中的像素数
        - ``height``: 垂直分辨率，即帧中的行数


    ``timing`` 必须是以下格式的 6 元组：

        (hsync_pulse_width, hsync_back_porch, hsync_front_porch, vsync_pulse_width, vsync_back_porch, vsync_front_porch)

        - ``hsync_pulse_width``: 水平同步宽度，单位：PCLK 周期
        - ``hsync_back_porch``: 水平后沿，hsync 和行活动数据开始之间的 PCLK 数
        - ``hsync_front_porch``: 水平前沿，活动数据结束和下一个 hsync 之间的 PCLK 数
        - ``vsync_pulse_width``: 垂直同步宽度，单位：行数
        - ``vsync_back_porch``: 垂直后廊，垂直同步和帧开始之间的无效行数
        - ``vsync_front_porch``: 垂直前沿，帧结束和下一个垂直同步之间的无效行数

Methods
-------

.. method:: RGB.reset()

    重置液晶面板。

.. method:: RGB.init()

    初始化液晶面板。

    .. note::

        在调用此方法之前，请确保 LCD 面板已通过 RGB.reset() 完成重置阶段。

.. method:: RGB.bitmap(x_start, y_start, x_end, y_end, color_data)

    在 LCD 面板上绘制位图。

    参数如下：

    - ``x_start`` - x 轴上的起始索引（包括 x_start）
    - ``y_start`` - y 轴上的起始索引（包括 y_start）
    - ``x_end`` - x 轴上的结束索引（不包括 x_end）
    - ``y_end`` - y 轴上的结束索引（不包括 y_end）
    - ``color_data`` - 将转储到特定窗口范围的 RGB 颜色数据

.. method:: RGB.mirror(mirror_x: bool, mirror_y: bool)

    在特定轴上镜像 LCD 面板。

    参数如下：

    - ``mirror_x`` - 面板是否将围绕 x 轴镜像
    - ``mirror_y`` - 面板是否将围绕 y 轴镜像

    .. note::

        结合RGB.swap_xy()，可以实现屏幕旋转

.. method:: RGB.swap_xy(swap_axes: bool)

    交换 x 和 y 轴。

    - ``swap_axes`` - 是否交换x和y轴

    .. note::

        结合RGB.mirror()，可以实现屏幕旋转

.. method:: RGB.set_gap(x_gap: int, y_gap: int)

    在 x 和 y 轴上设置额外的间隙。

    间隙是液晶面板左侧/顶部与实际显示内容的第一行/列之间的空间（以像素为单位）。

    参数如下：

    - ``x_gap`` - x 轴上的额外间隙，以像素为单位
    - ``y_gap`` - y 轴上的额外间隙，以像素为单位

    .. note::

        在定位或居中小于 LCD 的框架时，设置间隙非常有用。

.. method:: RGB.invert_color(invert_color_data: bool)

    反转颜色（按位反转颜色数据线）

    - ``invert_color_data`` - 是否反转颜色数据

.. method:: RGB.disp_off(off: bool)

    关闭显示器。

    - ``off`` - 是否关闭屏幕

.. method:: RGB.backlight_on()

    打开背光

.. method:: RGB.backlight_off()

    关闭背光

.. method:: RGB.deint()

    去初始化 LCD 面板。
