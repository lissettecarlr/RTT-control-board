#ifndef __WINDDIRECTION_H__
#define __WINDDIRECTION_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>


struct windDirection_config
{
   void * p; 
};

extern rt_err_t windDirection_register(const char *device_name,const char *i2c_bus,void *user_data);

#endif
