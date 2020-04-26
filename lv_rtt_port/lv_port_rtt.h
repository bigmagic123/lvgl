/**
 * @file lv_port_rtt.h
 *
 */

#ifndef LV_PORT_RTT_H
#define LV_PORT_RTT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
//#include "../../lv_drv_conf.h"

#include <stdint.h>
#include "../src/lv_misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void fbdev_init(void);
void rtt_lcd_register(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PORT_RTT_H*/