#ifndef __PM2_5_H__
#define __PM2_5_H__

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>


struct pm2_5_config
{
   void * p; 
};

extern rt_err_t pm2_5_register(const char *device_name,const char *i2c_bus,void *user_data);

#endif
