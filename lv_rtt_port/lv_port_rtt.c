#include "lv_port_rtt.h"

#include <rtthread.h>
#include <rtdevice.h>
#include <stdint.h>
#include <rthw.h>
#include <stdio.h>
#include "../src/lv_hal/lv_hal_disp.h"

#ifndef FBDEV_NAME
#define FBDEV_NAME "lcd"
#endif

static struct rt_device_graphic_info lcd_info;
static rt_uint8_t *fbp = 0;
static long int screensize = 0;
static rt_device_t lv_lcd;

static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 480];                     /*Declare a buffer for 10 lines*/

void fbdev_flush(lv_disp_t * disp, const lv_area_t * area, const lv_color_t *color_p);

void rtt_lcd_register(void)
{
    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 480);    /*Initialize the display buffer*/

    lv_disp_drv_t disp_drv;      /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv); /*Basic initialization*/

    /*Set up the functions to access to your display*/

    disp_drv.flush_cb = fbdev_flush; /*Set your driver function*/
    disp_drv.buffer = &disp_buf; /*Assign the buffer to the display*/

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

void fbdev_init(void)
{
    // Open the file for reading and writing
    lv_lcd = rt_device_find(FBDEV_NAME);
    if (lv_lcd == RT_NULL)
    {
        rt_kprintf("can't find the lcd:%s\n", FBDEV_NAME);
        return;
    }
    rt_kprintf("\nThe framebuffer device was opened successfully.\n");
    rt_device_open(lv_lcd, RT_DEVICE_OFLAG_RDWR);
    // Get fixed screen information
    rt_device_control(lv_lcd, RTGRAPHIC_CTRL_GET_INFO, &lcd_info);

    rt_kprintf("\n%dx%d, %dbpp\n", lcd_info.width, lcd_info.height, lcd_info.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = lcd_info.width * lcd_info.height * lcd_info.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (rt_uint8_t *)((lcd_info).framebuffer);
    rt_kprintf("\nThe framebuffer device was mapped to memory successfully.\n");
}

#define RGB888_TO_RGB565(r, g, b)    ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))
#define RGB332_TO_RGB565(r, g, b)    ((((r) << 2) << 11) | (((g) << 3) << 5) | ((b) << 2))
void fbdev_flush(lv_disp_t * disp, const lv_area_t * area, const lv_color_t *color_p)
{
    rt_uint32_t x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    long int location = 0;
    uint32_t x;
    uint32_t y;
    x1 = area->x1;
    x2 = area->x2;
    y1 = area->y1;
    y2 = area->y2;

    /*Return if the area is out the screen*/
    if (x2 < 0)
        return;
    if (y2 < 0)
        return;
    if (x1 > lcd_info.width - 1)
        return;
    if (y1 > lcd_info.height - 1)
        return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > lcd_info.width - 1 ? lcd_info.width - 1 : x2;
    int32_t act_y2 = y2 > lcd_info.height - 1 ? lcd_info.height - 1 : y2;

    lv_color_int_t *rtt_fbp = (lv_color_int_t *)fbp;

    for (y = act_y1; y <= act_y2; y++)
    {
        for (x = act_x1; x <= act_x2; x++)
        {
            location = (x) + (y)*lcd_info.width;
            rtt_fbp[location] = color_p->full;
            color_p++;
        }

        color_p += x2 - act_x2;
    }

    rt_device_control(lv_lcd, RTGRAPHIC_CTRL_RECT_UPDATE, &lcd_info);
    lv_disp_flush_ready(disp); 
}
