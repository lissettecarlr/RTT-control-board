#ifndef __WINDSPEED_H__
#define __WINDSPEED_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>


struct windSpend_config
{
  void *p;
};

extern rt_err_t windSpend_register(const char *device_name,const char *i2c_bus,void *user_data);

#endif
