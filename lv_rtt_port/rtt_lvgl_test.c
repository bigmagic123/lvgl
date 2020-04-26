#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "lv_port_rtt.h"
#include "sysmon.h"

void lvgl_service(void *param)
{
    lv_init();
    fbdev_init();
    rtt_lcd_register();
    sysmon_create();
    while (1)
    {
        lv_task_handler();
        rt_thread_delay(1);
    }
}


int test_lvgl(void)
{
    rt_thread_t tid;
    /* create thread for gui device */
    tid = rt_thread_create("lvgl",
                           lvgl_service,
                           NULL,
                           4096,
                           12,
                           10);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}

MSH_CMD_EXPORT(test_lvgl, test lvgl)
