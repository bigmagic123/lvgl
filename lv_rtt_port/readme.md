# rtt上移植littlevgl补丁说明

[toc]

## 1.修改Sconscript脚本

进入bsp/raspi3-64/applications/目录下，修改脚本如下：

```
from building import *

cwd     = GetCurrentDir()
src     = Glob('*.c') + Glob('*.cpp')
CPPPATH = [cwd]

group = DefineGroup('Applications', src, depend = [''], CPPPATH = CPPPATH)

list = os.listdir(cwd)
objs = []

for d in list:
    path = os.path.join(cwd, d)
    if os.path.isfile(os.path.join(path, 'SConscript')):
        objs = objs + SConscript(os.path.join(d, 'SConscript'))
group = group + objs

Return('group')
```

表示寻找子目录。

## 2.修改board.c

在bsp/raspi3-64/driver目录下：

首先加上头文件

```
#ifdef RT_USING_LVGL
#include <lvgl/src/lv_hal/lv_hal_tick.h> 
#endif
```

然后在`rt_hw_timer_isr`函数中修改如下

```
void rt_hw_timer_isr(int vector, void *parameter)
{
#ifdef BSP_USING_CORETIMER
    rt_hw_set_gtimer_val(timerStep);
#else
    ARM_TIMER_IRQCLR = 0;
#endif

#ifdef RT_USING_LVGL
    lv_tick_inc(1);
#endif

    rt_tick_increase();
}
```

主要就是增加`  lv_tick_inc(1);`。

## 3.修改Kconfig

这里主要的修改就是`Kconfig`前面加上一句

```
config RT_USING_LVGL
    bool "Using littlevgl"
    default n
```

## 4.配置menuconfig

选择使用Using littlevgl即可。