/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-09-04     armink       the first version
 */

#include "rthw.h"
#include "ulog.h"

#ifdef ULOG_BACKEND_USING_CONSOLE

#if defined(ULOG_ASYNC_OUTPUT_BY_THREAD) && ULOG_ASYNC_OUTPUT_THREAD_STACK < 384
#error "The thread stack size must more than 384 when using async output by thread (ULOG_ASYNC_OUTPUT_BY_THREAD)"
#endif

static struct ulog_backend console;

void ulog_console_backend_output(struct ulog_backend *backend, rt_uint32_t level, const char *tag, rt_bool_t is_raw,
        const char *log, size_t len)
{
    rt_device_t dev = rt_console_get_device();

#ifdef RT_USING_DEVICE
    if (dev == RT_NULL)
    {
        rt_hw_console_output(log);
    }
    else
    {
        rt_uint16_t old_flag = dev->open_flag;

        dev->open_flag |= RT_DEVICE_FLAG_STREAM;
        rt_device_write(dev, 0, log, len);
        dev->open_flag = old_flag;
    }
#else
    rt_hw_console_output(log);
#endif

}

int ulog_console_backend_init(void)
{
    ulog_init();
    console.output = ulog_console_backend_output;

    ulog_backend_register(&console, "console", RT_TRUE);

    return 0;
}
INIT_PREV_EXPORT(ulog_console_backend_init);

#endif /* ULOG_BACKEND_USING_CONSOLE */



//备注
/*
#ifdef RT_USING_ULOG

#define LOG_TAG              "main"
#define LOG_LVL              LOG_LVL_DBG //该模块对应的日志输出级别。不定义时，默认：调试级别
#include <ulog.h>

#else

#define LOG_E(format, ...)                    do{rt_kprintf(format, ##__VA_ARGS__);rt_kprintf("\n");}while(0)
#define LOG_W(format, ...)                    do{rt_kprintf(format, ##__VA_ARGS__);rt_kprintf("\n");}while(0)
#define LOG_I(format, ...)                    do{rt_kprintf(format, ##__VA_ARGS__);rt_kprintf("\n");}while(0)
#define LOG_D(format, ...)                    do{rt_kprintf(format, ##__VA_ARGS__);rt_kprintf("\n");}while(0)
#define LOG_RAW(format, ...)                  do{rt_kprintf(format, ##__VA_ARGS__);rt_kprintf("\n");}while(0)
//#define LOG_HEX(format, ...)    						  ulog_hex(format, ##__VA_ARGS__)

RT_WEAK void ulog_hexdump(const char *tag, rt_size_t width, rt_uint8_t *buf, rt_size_t size)
{
	int i=0;
	for(i=0;i<size;i++)
     rt_kprintf("%02X ",buf[i]);
	rt_kprintf("\n");
}

#endif
*/
