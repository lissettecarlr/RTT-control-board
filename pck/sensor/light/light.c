#include <rthw.h>
#include <string.h>
#include "light.h"

struct light_device
{
		struct rt_device parent;
    struct rt_i2c_bus_device *bus;
};

static rt_err_t light_init(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t light_open(rt_device_t dev,rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t light_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t light_control(rt_device_t dev,int cmd,void *args)
{
	return RT_EOK;
}

static rt_size_t light_write(rt_device_t dev,rt_off_t pos,const void *buffer,rt_size_t size)
{
	rt_size_t ret =0;
	return (ret == 2) ? size : 0;
}

static rt_size_t light_read(rt_device_t dev,rt_off_t pos,void *buffer,rt_size_t size)
{
	rt_size_t ret =0;
	return (ret == 2) ? size : 0;
	
}

#ifdef RT_USING_DEVICE_OPS

const static struct rt_device_ops light_ops = 
{
	light_init,
	light_open,
	light_close,
	light_read,
	light_write,
	light_control,
};
#endif

rt_err_t light_register(const char *device_name,const char *bus_name,void *user_data)
{
	static struct light_device light_dev;
	bus = rt_device_find(bus_name);
	light_dev.bus 									= bus;
	light_dev.parent.type				  = RT_Device_Class_Char;
#ifdef RT_USING_DEVICE_OPS
	light_dev.parent.ops						=	&light_ops;
#else
	light_dev.parent.init 				  = light_init;
	light_dev.parent.open						= light_open;
	light_dev.parent.close					= light_close;
	light_dev.parent.read 					= light_read;
	light_dev.parent.write					= light_write;
	light_dev.parent.control				= light_control;
	
#endif
	light_dev.parent.user_data 	  = user_data;
	return rt_device_register(&light_dev.parent,device_name,RT_DEVICE_FLAG_RDWR);
	
}

	 // struct light_config cfg={addr};
		light_register("light","usart2",RT_NULL);
INIT_APP_EXPORT(light_register);




