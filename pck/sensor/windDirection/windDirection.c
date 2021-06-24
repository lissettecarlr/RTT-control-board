#include <rthw.h>
#include <string.h>
#include "windDirection.h"

struct windDirection_device
{
		struct rt_device parent;
    struct rt_i2c_bus_device *bus;
};

static rt_err_t windDirection_init(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t windDirection_open(rt_device_t dev,rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t windDirection_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t windDirection_control(rt_device_t dev,int cmd,void *args)
{
	return RT_EOK;
}

static rt_size_t windDirection_write(rt_device_t dev,rt_off_t pos,const void *buffer,rt_size_t size)
{
	return 2;
}

static rt_size_t windDirection_read(rt_device_t dev,rt_off_t pos,void *buffer,rt_size_t size)
{
	
	return 2;
	
}

#ifdef RT_USING_DEVICE_OPS

const static struct rt_device_ops windDirection_ops = 
{
	windDirection_init,
	windDirection_open,
	windDirection_close,
	windDirection_read,
	windDirection_write,
	windDirection_control,
};
#endif

rt_err_t windDirection_register(const char *device_name,const char *bus_name,void *user_data)
{
	static struct windDirection_device windDirection_dev;
	bus = rt_device_find(bus_name);
	windDirection_dev.bus 									= bus;
	windDirection_dev.parent.type				  = RT_Device_Class_Char;
#ifdef RT_USING_DEVICE_OPS
	windDirection_dev.parent.ops						=	&windDirection_ops;
#else
	windDirection_dev.parent.init 				  = windDirection_init;
	windDirection_dev.parent.open						= windDirection_open;
	windDirection_dev.parent.close					= windDirection_close;
	windDirection_dev.parent.read 					= windDirection_read;
	windDirection_dev.parent.write					= windDirection_write;
	windDirection_dev.parent.control				= windDirection_control;
	
#endif
	windDirection_dev.parent.user_data 	  = user_data;
	return rt_device_register(&windDirection_dev.parent,device_name,RT_DEVICE_FLAG_RDWR);
	
}

	//  struct windDirection_config cfg={addr};
		windDirection_register("windDirection","usart2"，RT_NULL);
INIT_APP_EXPORT(windDirection_register);




