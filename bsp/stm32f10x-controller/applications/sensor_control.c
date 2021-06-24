/*
主要功能:
1.自动检测传感器状态
2.自动更新传感器
3.获取状态接口
4.开关自动更新
5.获取数据


MSH命令:
display_all_sensor_node
add_node name
delete_node name
alter_node name statue

//修改传感器更新时间


备注: 该文件中有很多无关代码，原因是原本想动态添加传感器，暂定为固定传感器，后续定将修改为动态，所以为删除其中代码
*/

#include <rtthread.h>
#include "user_mb_app.h"

//DBUG
#define LOG_TAG              "sensor_ctr"
#define LOG_LVL              LOG_LVL_DBG
#include <rtdbg.h>


//作为气象站控制板，传感器是固定的，服务器通过协议号来解析，此处也根据协议号来采集对应传感器
#include "protocol.h"


#define SENSOR_CYCLE_UPDATE_MS  10*1000  

typedef struct sSensorBuffer
{
   uint8_t size;
	 uint8_t buf[255];
}SensorBuffer_s;

static struct rt_event sensor_event;
#define SENSOR_EVENT_MODBUS_OK  (uint32_t)1<<0 
#define SENSOR_EVENT_UPDATE     (uint32_t)1<<1 

static rt_timer_t sensor_cycle_update;//传感器周期更新时间
static SensorBuffer_s SensorBuf={0,{0}};//传感器数据buffer

#define thread_ModbusMasterPoll_Prio      	   9
static rt_uint8_t thread_ModbusMasterPoll_stack[512];
struct rt_thread thread_ModbusMasterPoll;

//传感器存储表的属性 : 名称,状态
//实现链表
//在传感器控制器中，动态存储传感器信息。

#include "string.h"
typedef struct sSensorNode
{
   uint8_t statue;
	 char name[10];
	 struct sSensorNode *next;
}SensorNode_s;
SensorNode_s *SensorListHead=RT_NULL;

//增加一个节点
int add_sensor_node(char *name)
{
   SensorNode_s *node = (SensorNode_s *)rt_malloc(sizeof(SensorNode_s));
	 if(node ==RT_NULL)
		 return 1;
	 strcpy(node->name,name);
	 node->statue=0;
	 if(SensorListHead ==RT_NULL) //如果一个节点都没有
	 {
	    SensorListHead = node;
		  node->next =RT_NULL;
	 }
	 else
   {
	    node->next = SensorListHead;
		  SensorListHead = node;
	 }
	 return 0;
}
//删除一个节点
int delete_sensor_node(char *name)
{
	  SensorNode_s *LastNode = SensorListHead;
	  SensorNode_s *Node = SensorListHead;
    while(Node !=RT_NULL)
		{
		    if(rt_strcmp(Node->name,name) ==0 ) //找到节点
				{
				     if(LastNode == Node) //如果就是头结点
						 {
						    SensorListHead = SensorListHead->next;
							  rt_free(Node);
						 }
						 else
						 {
						    LastNode->next = Node->next;
							  rt_free(Node);
						 }					 
						 return 0;
				}
				else
				{
				   LastNode = Node;
					 Node = Node->next;
				}
		} 
		return 1;
}
//修改节点状态
int alter_sensor_node(char *name,uint8_t statue)
{
   SensorNode_s *Node = SensorListHead;
	 while(Node !=RT_NULL)
	 {
	      if(rt_strcmp(Node->name,name) ==0 ) //找到节点
				{
				   Node->statue = statue;
					 return 0;
				}
				else
				{
					 Node = Node->next;
				}
	 }
	 return 1;
}

//查询节点状态
int find_sensor_node(char *name,uint8_t *statue)
{
   SensorNode_s *Node = SensorListHead;
	 while(Node !=RT_NULL)
	 {
	      if(rt_strcmp(Node->name,name) ==0 ) //找到节点
				{
				   *statue = Node->statue;
					 return 0;
				}
				else
				{
					 Node = Node->next;
				}
	 }
	 return 1;
}

//变量所以节点输出名称和状态
void display_all_sensor_node()
{
   SensorNode_s *Node = SensorListHead;
	 while(Node !=RT_NULL)
	 {
		    rt_kprintf("%s:%d\n",Node->name,Node->statue);
				Node = Node->next;
				
	 }
}
MSH_CMD_EXPORT(display_all_sensor_node,show node);

static void add_node(char argc,char **acgv)
{
    if(argc !=2)
		{
		   rt_kprintf("param number error\n");
			 return;
		}
		add_sensor_node(acgv[1]);
}
MSH_CMD_EXPORT(add_node,add sensor node);
static void delete_node(char argc,char **acgv)
{
    if(argc !=2)
		{
		   rt_kprintf("param number error\n");
			 return;
		}
		delete_sensor_node(acgv[1]); 
}
MSH_CMD_EXPORT(delete_node,delete_ sensor node);
static void alter_node(char argc,char ** argv)
{
    if(argc !=3)
		{
		   rt_kprintf("param number error\n");
			 return;
		}
		char *name = argv[1];
		if(rt_strcmp(argv[2],"1")==0)
		{
		    alter_sensor_node(name,1);
		}
		else if(rt_strcmp(argv[2],"0")==0)
		{
		    alter_sensor_node(name,0);
		}
		else
		{
		
		}
		rt_kprintf("alter %s:%s\n",name,argv[2]);
}
MSH_CMD_EXPORT(alter_node,alter_ sensor node);



/*自动更新定时器回调*/
void sensor_cycle_update_irp(void *p)
{
   rt_event_send(&sensor_event ,SENSOR_EVENT_UPDATE );
}

/*ModBUS进程*/
void thread_entry_ModbusMasterPoll(void* parameter)
{
	 eMBMasterInit(MB_RTU, 2, 9600,  MB_PAR_EVEN);
	 eMBMasterEnable();
	//初始化完毕后抛出事件，表示传感器可以通讯了
	 rt_event_send(&sensor_event,SENSOR_EVENT_MODBUS_OK);
	while (1)
	{
		 eMBMasterPoll();
		 rt_thread_delay(1);
	}
}

/*接口 打开传感器自动更新*/
void sensor_open_update()
{
   rt_timer_start(sensor_cycle_update);
}
/*接口 关闭传感器自动更新*/
void sensor_close_update()
{
   rt_timer_stop(sensor_cycle_update);
}
/*接口 获取数据*/
uint8_t sensor_get_data(uint8_t *buf)
{
	  //关闭中断
	  rt_base_t base = rt_hw_interrupt_disable();
    rt_memcmp(buf,SensorBuf.buf,SensorBuf.size);
	  rt_hw_interrupt_enable(base);
	  return SensorBuf.size;
}

/*接口 传感器状态,参数为空则打印所以传感器状态*/
int sensor_get_statue(char *name)
{
	  uint8_t statue=0;
	 if(name ==RT_NULL)
	 {
	    display_all_sensor_node();
	 }
	 else
	 {
	    find_sensor_node(name,&statue);
	 }
	 return statue;
}

void use_protocol_v1()
{
	 rt_device_t dev;
	 uint8_t size=0;
   for(int i=0;i<Ptc.amount;i++)
	 {
	     //查询设备
		   dev = rt_device_find(Ptc.sensor[i]);
		   if(dev != RT_NULL)
			 {
			   	rt_device_open(dev, RT_DEVICE_OFLAG_RDWR);
					size+= rt_device_read(dev,1,SensorBuf.buf+size,0);
					rt_device_close(dev);
			 }
		   //RT_ASSERT(dev);   //因为传感器固定，不允许没有驱动，不想这么写哎，链表就没意义了，不能动态！！

	 }
	 SensorBuf.size = size;
}

//轮询链表，将读取的数据保存到buf中
void update_sensor()
{
	 //清空buffer
	 SensorBuf.size=0;
	 rt_memcpy(SensorBuf.buf,0,255);
	  
   if(Ptc.version == 1)
	 {
	     use_protocol_v1();
	 }
	 else
	 {
	    LOG_D("Protocol version error");
	 }

}

//传感器控制进程
void sensor_thread_entry(void *p)
{
	  rt_uint32_t event;
    //等待modbus初始化完毕
    rt_event_recv(&sensor_event, SENSOR_EVENT_MODBUS_OK \
		                    ,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &event);
	  
	  //默认开启自动传感器更新
	  sensor_cycle_update= rt_timer_create("radio_check_statue",\
	                     sensor_cycle_update_irp,
	                     RT_NULL,
	                     rt_tick_from_millisecond(SENSOR_CYCLE_UPDATE_MS),RT_TIMER_FLAG_PERIODIC);
	  rt_timer_start(sensor_cycle_update);
	  update_sensor(); //启动则尝试更新一次数据
	  while(1)
		{
		   rt_event_recv(&sensor_event, SENSOR_EVENT_UPDATE \
										,RT_EVENT_FLAG_OR |RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, &event);
			 if(event & SENSOR_EVENT_UPDATE)
			 {
			      //轮询读取已知传感器，最后封装成一帧数据
				    update_sensor();
			 }
			
		}			
}


void sensor_thread_init()
{
	 rt_event_init(&sensor_event,"sensor event",RT_IPC_FLAG_FIFO);
	
	//开启MODBUS
	rt_thread_init(&thread_ModbusMasterPoll, "MBMasterPoll",
	thread_entry_ModbusMasterPoll, RT_NULL, thread_ModbusMasterPoll_stack,
	sizeof(thread_ModbusMasterPoll_stack), thread_ModbusMasterPoll_Prio,5);
	rt_thread_startup(&thread_ModbusMasterPoll);
	
	//传感器控制进程
  rt_thread_t sensor_hander;
	sensor_hander =rt_thread_create("sensor thread",sensor_thread_entry,RT_NULL,2048,10,10);
	if(sensor_hander !=RT_NULL)
	{
		 rt_thread_startup(sensor_hander);
	}
	else
	{
	   LOG_E("sensor thread init error");
	}
		
}
