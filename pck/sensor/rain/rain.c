#include <rthw.h>
#include <string.h>
#include "rain.h"

struct rain_device
{
		struct rt_device parent;
		rt_device_t bus;
};

static rt_err_t rain_init(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t rain_open(rt_device_t dev,rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rain_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t rain_control(rt_device_t dev,int cmd,void *args)
{
	return RT_EOK;
}

static rt_size_t rain_write(rt_device_t dev,rt_off_t pos,const void *buffer,rt_size_t size)
{
	return 1;
}

static rt_size_t rain_read(rt_device_t dev,rt_off_t pos,void *buffer,rt_size_t size)
{
	return 1;
	
}

#ifdef RT_USING_DEVICE_OPS

const static struct rt_device_ops rain_ops = 
{
	rain_init,
	rain_open,
	rain_close,
	rain_read,
	rain_write,
	rain_control,
};
#endif

rt_err_t rain_register(const char *device_name,const char *bus_name,void *user_data)
{
	static struct rain_device rain_dev;
	bus = rt_device_find(bus_name);
	rain_dev.bus 									= bus;
	rain_dev.parent.type				  = RT_Device_Class_Char;
#ifdef RT_USING_DEVICE_OPS
	rain_dev.parent.ops						=	&rain_ops;
#else
	rain_dev.parent.init 				  = rain_init;
	rain_dev.parent.open						= rain_open;
	rain_dev.parent.close					= rain_close;
	rain_dev.parent.read 					= rain_read;
	rain_dev.parent.write					= rain_write;
	rain_dev.parent.control				= rain_control;
	
#endif
	rain_dev.parent.user_data 	  = user_data;
	return rt_device_register(&rain_dev.parent,device_name,RT_DEVICE_FLAG_RDWR);
	
}

	//  struct rain_config cfg={addr};
		rain_register("rain","usart2"，RT_NULL);
INIT_APP_EXPORT(rain_register);


