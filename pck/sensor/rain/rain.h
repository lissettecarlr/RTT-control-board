#ifndef __RAIN_H__
#define __RAIN_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>


struct rain_config
{
   void * p; 
};

extern rt_err_t rain_register(const char *device_name,const char *i2c_bus,void *user_data);

#endif
