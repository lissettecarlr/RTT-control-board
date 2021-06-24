/*
主要功能:
1.周期查询设备模块连接状态
2.发送数据
3.设置是否模块自动入网
4.设置模块的OTAA入网信息

模组:
入网周期默认60，次数2，控制板将不进行该控制
自动入网默认关闭，控制板将其置为自动入网

工作模式:
启动后先进行一次状态检查，然后进行周期自动自检，当判断射频模块处于等待启动入网状态时，
查询时候有配置，如果有则根据配置设置射频模板，开启入网。否则等待外部传入配置文件。

函数接口:
初始化 : void radio_thread_init(void)
发送数据: int radio_send(uint8_t *data,uint8_t size )
射频模组状态: uint8_t radio_read_statue()

MSH接口:
radio_read_statue
radio_set
clear_node_info
*/
#include <rtthread.h>
#include "AT_client_radio.h"
#include "at.h"

//DBUG
#define LOG_TAG              "radio_ctr"
#define LOG_LVL              LOG_LVL_DBG
#include <rtdbg.h>

typedef struct sData
{
   uint8_t *ptr;
	 uint8_t size;
}Data_s;


typedef struct sRadioInfo
{
	 AtRadioInfo_t OTAA;	
}RadioInfo_s;

static struct rt_event radio_event;

//#define  RADIO_EVENT_START_JOIN        (uint32_t)1<<0 //main->radio
//#define  RADIO_EVENT_FINISH_JOIN       (uint32_t)1<<1 //main->radio
//#define  RADIO_EVENT_JOIN_OK           (uint32_t)1<<2 //radio->main
//#define  RADIO_EVENT_STATUE_LOSE       (uint32_t)1<<3 //radio->main


#define  RADIO_EVENT_SEND               (uint32_t)1<<1 //radio->main
#define  RADIO_EVENT_CHECK_STATUE				(uint32_t)1<<2 //radio->radio



#define RADIO_CHECK_STATUE_CYCLE_MS   5*60*1000

//数据指针
static Data_s *RadioBuffer = NULL;

static rt_timer_t check_statue_timer;
static AtRadioStatue_t radioStatue = AT_RADIO_STATUE_LOSE;

/*数据发送，使用malloc分配了内存，在使用完毕后需要释放*/
int radio_send(uint8_t *data,uint8_t size )
{
	   if(size >=50)
			 return 1;
     RadioBuffer = (Data_s *) rt_malloc(sizeof(Data_s));

	   RadioBuffer->ptr =data;
	   RadioBuffer->size = size;
	   //数据发送事件
	   rt_event_send(&radio_event,RADIO_EVENT_SEND);
	   return 0;
}

void radio_check_statue_timer_irq(void *p)
{
   rt_event_send(&radio_event,RADIO_EVENT_CHECK_STATUE);
}

/*开启周期状态检查*/
void radio_open_statue_check()
{
    	 //周期状态查询
	 check_statue_timer= rt_timer_create("radio_check_statue",\
	                     radio_check_statue_timer_irq,
	                     RT_NULL,
	                     rt_tick_from_millisecond(RADIO_CHECK_STATUE_CYCLE_MS),RT_TIMER_FLAG_PERIODIC);
	 rt_timer_start(check_statue_timer);
}
/*关闭周期状态检查*/
void radio_close_statue_check()
{
    rt_timer_stop(check_statue_timer);
}



//这里简化模式   存在=1，丢失=0
uint8_t radio_read_statue(void )
{
	  if(radioStatue == AT_RADIO_STATUE_LOSE)
		{
			rt_kprintf("radio module lose\n");
			return 0;
		}
		else
		{
			rt_kprintf("radio module connect\n");
      return 1;
		}
}
MSH_CMD_EXPORT(radio_read_statue, get radio module statue);

#ifdef PKG_USING_FAL
#include "fal.h"

int read_node_info(RadioInfo_s *info)
{
	 const struct fal_partition * bli = fal_partition_find("eeprom2") ;
	 if (bli ==RT_NULL)
	 {
		  LOG_D("fal error");
		  return 1;
	 }
	 fal_partition_read(bli,0,info,sizeof(RadioInfo_s)) ;
	 
   if(info->OTAA.appEui[0] != 0 && info->OTAA.appKey!=0 && info->OTAA.devEui !=0) //这里使用三个数据的首位来判断是否保存过，是不严谨的
	 {
		  LOG_D("not find node info");
		  return 3;
	 }
	 else
	 {
	    LOG_D("node info read succeed");
		  return 0;
	 }
}

//保存当前节点信息，返回0则表示保存成功
int save_node_info(RadioInfo_s *info)
{
	 const struct fal_partition * bli = fal_partition_find("eeprom2") ;
	 if (bli ==RT_NULL)
	 {
		  LOG_D("fal error");
		  return 1;
	 }
	 
		fal_partition_erase(bli,0,sizeof(RadioInfo_s));
	  rt_thread_mdelay(1000);
	  fal_partition_write(bli,0,info,sizeof(RadioInfo_s));
		return 0;
}

int clear_node_info()
{
   const struct fal_partition * bli = fal_partition_find("eeprom2") ;
	 if (bli ==RT_NULL)
	 {
		  LOG_D("fal error");
		  return 1;
	 }
	 fal_partition_erase(bli,0,sizeof(RadioInfo_s));
	 return 0;
}
MSH_CMD_EXPORT(clear_node_info, clear flash);
#endif

//对模组进行入网配置，设置成自动入网，间隔60s 尝试两次
//使用该配置进行入网
//返回0则操作成功
static int  raido_join_config_join(RadioInfo_s *info)
{
	 
	 AtError_t re=AT_ER_OK;
	 if(info ==RT_NULL)
	 {
		   #ifdef PKG_USING_FAL
		   RadioInfo_s UseInfo;
	     if( read_node_info(&UseInfo) ==0) //读取flash			  
			 {
			     raido_join_config_join(UseInfo);
			 }
			 else //读取失败，等待外部写入配置
			 #endif	 
			 {
			    
			 }
	 }
	 else//使用传入的配置信息
	 {
		   //将传入信息保存在本地flash中
		   #ifdef PKG_USING_FAL
		   save_node_info(info);
		   #endif
		 
		   //设置OTAA参数
	     re = at_radio_set_OTAA_info( &(info->OTAA) );
		   if(re !=AT_ER_OK)
			 {
			    LOG_D("set_OTAA_info fail");
				  return re;
			 }
		   //开启启动自动入网
	 	   re=at_radio_set_autojoin(1);
			 if(re !=AT_ER_OK)
			 {
			    LOG_D("set_autojoin fail");
				  return re;
			 }
			 
	     //开启模组周期入网
       re =at_radio_join_net();
	 }
	 return re;
}

//射频模组进程
void radio_thread_entry(void *p)
{

	 AtError_t re;
	 rt_uint32_t event;
	 
	 rt_thread_mdelay(5000); //给5s使模组初始化完毕
	 
	 //开启通信，并且查询一次状态
		re = at_radio_get_statue(&radioStatue); 
		LOG_D("first check radio statue:%d",re);
	  radio_open_statue_check();
	 
	 while(1)
	 {
		 
				if(radioStatue == AT_RADIO_STATUE_WAIT) //模组只要在没开启自动入网时才会进入该状态
				{					  
						raido_join_config_join(RT_NULL);
						rt_thread_mdelay(5000);
				}
				
	      rt_event_recv(&radio_event, RADIO_EVENT_SEND|RADIO_EVENT_CHECK_STATUE  \
		                    ,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &event);

			  if(event & RADIO_EVENT_CHECK_STATUE)  //周期检查状态，在状态改变时发送事件
				{					  
				   	 re = at_radio_get_statue(&radioStatue); 
	           LOG_D("check radio statue:%d",re);					 
				}
				if(event & RADIO_EVENT_SEND)//发送数据
				{
					  if(radioStatue != AT_RADIO_STATUE_LOSE  && radioStatue != AT_RADIO_STATUE_WAIT)
						{
							//发送
							re = at_radio_send_hex(RadioBuffer->ptr,RadioBuffer->size);
							LOG_D("send hex:%d",re);				
							//释放
							rt_free(RadioBuffer);
						}
						else{LOG_I("send be not allowed ");}
				}
	 }
}


void radio_thread_init(void)
{
   rt_thread_t radio_hander;
	 rt_event_init(&radio_event,"radio event",RT_IPC_FLAG_FIFO);
	 at_client_init("uart2",512);
	
	 radio_hander = rt_thread_create("radio thread",radio_thread_entry,RT_NULL,1024,2,10);
	 if(radio_hander !=RT_NULL )
	 {
	    rt_thread_startup(radio_hander);
	 }
	 else
	 {
	    LOG_E("radio thread init error");
	 }
}



#include "string.h"
//MSH命令，修改配置   输入 radio_set 1122334455667788 8877665544332211 11112222333344445555666677778888
static void radio_set(uint8_t argc, char **argv) 
{
    if(argc != 4)
		{
			 LOG_I("radio set fail argc:%d",argc);
			 return ;
		}
		
		char *deveui = argv[1];
		LOG_D("radio set deveui:%d",deveui);
		if(rt_strlen(deveui) !=16)
			return;
		
		char *appeui = argv[2];
		LOG_D("radio set appeui:%d",appeui);
		if(rt_strlen(appeui) !=16)
			return;
		
		char *appeky = argv[3];
		LOG_D("radio set appeky:%d",appeky);
		if(rt_strlen(appeky) !=32)
			return;
		
		RadioInfo_s info;
		strcpy(info.OTAA.devEui,deveui);
		strcpy(info.OTAA.appEui,appeui);
		strcpy(info.OTAA.appKey,appeky);
		raido_join_config_join(&info);
}
MSH_CMD_EXPORT(radio_set, radio set info.);
