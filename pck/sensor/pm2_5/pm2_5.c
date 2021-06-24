#include <rthw.h>
#include <string.h>
#include "pm2_5.h"

struct pm2_5_device
{
		struct rt_device parent;
    rt_device_t bus;
};

static rt_err_t pm2_5_init(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t pm2_5_open(rt_device_t dev,rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t pm2_5_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t pm2_5_control(rt_device_t dev,int cmd,void *args)
{
	return RT_EOK;
}

static rt_size_t pm2_5_write(rt_device_t dev,rt_off_t pos,const void *buffer,rt_size_t size)
{
	return 2;	
}

static rt_size_t pm2_5_read(rt_device_t dev,rt_off_t pos,void *buffer,rt_size_t size)
{
	return 2;
	
}

#ifdef RT_USING_DEVICE_OPS

const static struct rt_device_ops pm2_5_ops = 
{
	pm2_5_init,
	pm2_5_open,
	pm2_5_close,
	pm2_5_read,
	pm2_5_write,
	pm2_5_control,
};
#endif
rt_err_t pm2_5_register(const char *device_name,const char *bus_name,void *user_data)
{
	static struct pm2_5_device pm2_5_dev;
	bus = rt_device_find(bus_name);
	pm2_5_dev.bus 									= bus;
	pm2_5_dev.parent.type				  = RT_Device_Class_Char;
#ifdef RT_USING_DEVICE_OPS
	pm2_5_dev.parent.ops						=	&pm2_5_ops;
#else
	pm2_5_dev.parent.init 				  = pm2_5_init;
	pm2_5_dev.parent.open						= pm2_5_open;
	pm2_5_dev.parent.close					= pm2_5_close;
	pm2_5_dev.parent.read 					= pm2_5_read;
	pm2_5_dev.parent.write					= pm2_5_write;
	pm2_5_dev.parent.control				= pm2_5_control;
	
#endif
	pm2_5_dev.parent.user_data 	  = user_data;
	return rt_device_register(	pm2_5_dev.parent,device_name,RT_DEVICE_FLAG_RDWR);
	
}

	 // struct	pm2_5_config cfg={addr};
		pm2_5_register(	pm2_5","usart2",RT_NULL);
INIT_APP_EXPORT	pm2_5_register);






