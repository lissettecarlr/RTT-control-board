/*
控制板主程序
主要功能是采集传感器数据，通过射频模块发送给服务器
辅助功能: 
	1.能够修改射频模块的入网信息
	2.能够监控射频模块的状态
	3.能够监控传感器设备的状态
	4.可配置传感器采集周期
	5.可掉电存储一些基本信息(射频模块配置信息，采集周期)

*/
#include <rtthread.h>
#include "AT_client_radio.h"


//DBUG
#define LOG_TAG              "main"
#define LOG_LVL              LOG_LVL_DBG
#include <rtdbg.h>


#include "sensor_control.h"
#include "radio_control.h"



//主函数进程，控制传感器进程进行数据采集，控制射频模组进程和服务器通讯
//周期读取传感器数据，然后判断射频模块是否存在，存在则发数据

static uint32_t upload_cycle = 15*1000;

int atoi_(char s[])  
{  
    int i = 0;  
    int n = 0;  
    for (i = 0; s[i] >= '0' && s[i] <= '9'; ++i)  
    {  
        n = 10 * n + (s[i] - '0');  
    }  
    return n;  
}

void doing_cyc(char argc,char **argv)
{
    if(argc != 2)
		{
		   rt_kprintf("param error\n");
		}
		upload_cycle = atoi_(argv[1]);	
		rt_kprintf("cyc: %d",upload_cycle);
}
MSH_CMD_EXPORT(doing_cyc,upload cycle);

int main(void)
{
	   uint8_t buffer[255];
	   uint8_t size=0;
  	 radio_thread_init();
	   sensor_thread_init();
	   rt_thread_mdelay(3*1000); //等待两个模块初始化完毕
	   while(1)
		 {
		    rt_thread_mdelay(upload_cycle);
			  //获取传感器数据
			  size = sensor_get_data(buffer); 
			  LOG_D("send :size %d",size);
			  //判断射频模组状态
			  if( radio_read_statue() )
				{
				    radio_send(buffer,size);
				}					 
		 }		
    return 0;
}
