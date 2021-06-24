#include <rthw.h>
#include <string.h>
#include "windSpend.h"

struct windSpend_device
{
		struct rt_device parent;
    struct rt_i2c_bus_device *bus;
};

static rt_err_t windSpend_init(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t windSpend_open(rt_device_t dev,rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t windSpend_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t windSpend_control(rt_device_t dev,int cmd,void *args)
{
	return RT_EOK;
}

static rt_size_t windSpend_write(rt_device_t dev,rt_off_t pos,const void *buffer,rt_size_t size)
{
	return 2;
}

static rt_size_t windSpend_read(rt_device_t dev,rt_off_t pos,void *buffer,rt_size_t size)
{
	return 2;
	
}

#ifdef RT_USING_DEVICE_OPS

const static struct rt_device_ops windSpend_ops = 
{
	windSpend_init,
	windSpend_open,
	windSpend_close,
	windSpend_read,
	windSpend_write,
	windSpend_control,
};
#endif

rt_err_t windSpend_register(const char *device_name,const char *bus_name,void *user_data)
{
	static struct windSpend_device windSpend_dev;
	bus = rt_device_find(bus_name);
	windSpend_dev.bus 									= bus;
	windSpend_dev.parent.type				  = RT_Device_Class_Char;
#ifdef RT_USING_DEVICE_OPS
	windSpend_dev.parent.ops						=	&windSpend_ops;
#else
	windSpend_dev.parent.init 				  = windSpend_init;
	windSpend_dev.parent.open						= windSpend_open;
	windSpend_dev.parent.close					= windSpend_close;
	windSpend_dev.parent.read 					= windSpend_read;
	windSpend_dev.parent.write					= windSpend_write;
	windSpend_dev.parent.control				= windSpend_control;
	
#endif
	windSpend_dev.parent.user_data 	  = user_data;
	return rt_device_register(&windSpend_dev.parent,device_name,RT_DEVICE_FLAG_RDWR);
	
}

	//  struct windSpend_config cfg={addr};
		windSpend_register("windSpend","usart2"，RT_NULL);
INIT_APP_EXPORT(windSpend_register);




