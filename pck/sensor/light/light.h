#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>


struct light_config
{
   void * p; 
};

extern rt_err_t light_register(const char *device_name,const char *i2c_bus,void *user_data);

#endif
