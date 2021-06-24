/*
该文件用于实现和射频模组的AT通讯
*/
#include "at.h"
#include "AT_client_radio.h"

#define LOG_TAG              "at.radio"
#include <at_log.h>




//获取串口信息
AtError_t at_radio_find_uart_config(AtUartCfg_t *UartCfg)
{
	 rt_err_t  re=0;
	 at_response_t resp = RT_NULL;
	 //建立一个能接收64字节大小响应，当5s未接收到响应则超时
   resp = at_create_resp(64, 0, rt_tick_from_millisecond(5000));
	 if(resp ==RT_NULL)
	 {return AT_ER_CREAT_RESP;}
	 re=at_exec_cmd(resp, "AT+UART?");
	 if (re ==-1) 
		{ return AT_ER_SEND_ERROR;}
	 if (re ==-2)
		{return AT_ER_SEND_TIMEOUT;}
	 
	 //解析响应 %*[^=]=表示忽律到=号为止，然后在忽律=号
	 if( at_resp_parse_line_args(resp, 1,"%*[^=]=%d,%d,%d,%d,%d", \
	   &(UartCfg->baudrate), &(UartCfg->databits), &(UartCfg->stopbits), &(UartCfg->parity), &(UartCfg->control) ) ==RT_NULL)
	   return AT_ER_PARSE_LINE;
	 LOG_D("baudrate=%d, databits=%d, stopbits=%d, parity=%d, control=%d",\
	 UartCfg->baudrate, UartCfg->databits, UartCfg->stopbits, UartCfg->parity, UartCfg->control);
	  
	 at_delete_resp(resp);
	 return AT_ER_OK;
}


//查询模组信息
AtError_t at_radio_get_statue(AtRadioStatue_t *statue)
{
	 rt_err_t re=0;
	 char str[10];
	 at_response_t resp =RT_NULL;
	 resp = at_create_resp(64,0,rt_tick_from_millisecond(5000));
	 if(resp ==RT_NULL)
	 {return AT_ER_CREAT_RESP;}
	 re=at_exec_cmd(resp,"AT+STATUE");
	 if (re ==-1) 
		{ return AT_ER_SEND_ERROR;}
	 if (re ==-2)
	 {
		   *statue =AT_RADIO_STATUE_LOSE;
		   return AT_ER_SEND_TIMEOUT;
	 }
	 if(at_resp_parse_line_args_by_kw(resp,"+STATUE","%*[^:]:%s",str) ==RT_NULL)
	 {
		  return AT_ER_PARSE_KW;
	 }
		
	LOG_D("radio statue :%s",str);
  if ( !rt_strcmp(str,"LORA_SLEEP") )
	{
	   *statue =AT_RADIO_STATUE_SLEEP;
	}
	else if( !rt_strcmp(str,"LORA_JOIN") )
	{
	    *statue =AT_RADIO_STATUE_JOIN;
	}
	else if( !rt_strcmp(str,"LORA_SEND") )
	{
	    *statue =AT_RADIO_STATUE_SEND;
	}
	else if( !rt_strcmp(str,"LORA_CYCLE_SEND") )
	{
	    *statue =AT_RADIO_STATUE_CYCLE_SEND;
	}
	else if( !rt_strcmp(str,"LORA_NULL") )
	{
	    *statue =AT_RADIO_STATUE_NULL;
	}
	else if( !rt_strcmp(str,"LORA_WAIT_JOIN"))
	{
	    *statue =AT_RADIO_STATUE_WAIT;
	}
	else
  {
	   LOG_I("unknow statue");
	}
	 at_delete_resp(resp);
   return AT_ER_OK;
}
//MSH_CMD_EXPORT(at_radio_get_statue, get radio module statue);


//入网 AT+JOIN  ，该函数不返回入网状态，使用at_radio_get_statue来查询是否入网
//60s进行一次入网尝试，先进行入网配置，设置入网周期为60s，每个周期执行1次入网尝试
AtError_t at_radio_join_net(void)
{
	   rt_err_t re=0;
	   at_response_t resp =RT_NULL;
     //进行配置
	   resp = at_create_resp(10,0,rt_tick_from_millisecond(5000));
	  if(resp ==RT_NULL)
	  {return AT_ER_CREAT_RESP;}
	  re=at_exec_cmd(resp,"AT+JOIN=60,1");
	  if (re ==-1) 
		 { return AT_ER_SEND_ERROR;}
	  if (re ==-2)
	  {
		    return AT_ER_SEND_TIMEOUT;
	  }
	  at_delete_resp(resp);
	  //进行入网
		resp = at_create_resp(64,0,rt_tick_from_millisecond(5000));
		if(resp ==RT_NULL)
	  {return AT_ER_CREAT_RESP;}
	  re=at_exec_cmd(resp,"AT+JOIN");
	  if (re ==-1) 
		 { return AT_ER_SEND_ERROR;}
	  if (re ==-2)
	  {
			  LOG_D("radio lose");
		    return AT_ER_SEND_TIMEOUT;
	  }
	  at_delete_resp(resp);
		return AT_ER_OK;
}
//MSH_CMD_EXPORT(at_radio_join_net, network access use radio modules);

void byte_2_hexstr(const unsigned char* source, char* dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;

    for (i = 0; i < sourceLen; i++)
    {
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;

        highByte += 0x30;

        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    return ;
}


//插入字符串
char *strcat_m(char *dest,const char *str)
{
	char *cp=dest;
	while(*cp!='\0') ++cp;
 
	while((*cp++=*str++)!='\0')
	{
	}
	return dest;
}


//hex发数据  限制128
AtError_t at_radio_send_hex(uint8_t *data,uint8_t size)
{
	
	 //将hex转化为str
	 char sendStr[512]={"AT+SENDHEX="};
	 char str[256]={0};
	 byte_2_hexstr(data,str,size);
	 //发送数据
	 rt_err_t re=0;
	 at_response_t resp =RT_NULL;
	 resp =at_create_resp(64,0,rt_tick_from_millisecond(5000));
	 if(resp ==RT_NULL)
	 {return AT_ER_CREAT_RESP;}
	 
	 strcat_m(sendStr,str);
	 
	 LOG_D("send :%s",str);
	 re=at_exec_cmd(resp,sendStr);
	 if (re ==-1) 
	 { return AT_ER_SEND_ERROR;}
	  if (re ==-2)
	  {
		    return AT_ER_SEND_TIMEOUT;
	  }
		at_delete_resp(resp);
		return AT_ER_OK;
}
//MSH_CMD_EXPORT(at_radio_send_hex, network access use radio modules);

//配置是否自动入网 1:开启  0关闭
AtError_t at_radio_set_autojoin(uint8_t mode)
{
   rt_err_t re=0;
	 char sendStr[50]={"AT+AUTOJOIN="};
	 if(mode ==1)
	   strcat_m(sendStr,"1");
	 else
		 strcat_m(sendStr,"0");
	 
	 at_response_t resp =RT_NULL;
	 resp =at_create_resp(64,0,rt_tick_from_millisecond(5000));
	 if(resp ==RT_NULL)
	 {return AT_ER_CREAT_RESP;}
	 
	 re=at_exec_cmd(resp,sendStr);
	 if (re ==-1) 
	 { return AT_ER_SEND_ERROR;}
	  if (re ==-2)
	  {
		    return AT_ER_SEND_TIMEOUT;
	  }
		at_delete_resp(resp);
		return AT_ER_OK;
}

//OATT配置，修改DEVEUI APPEUI APPKEY，修改完成后，进行AT+ID读取对比
AtError_t at_radio_set_OTAA_info(AtRadioInfo_t *info)
{
//	  AtRadioInfo_t info_s={{"1122334455667788"},{"8877665544332211"},{"11112222333344445555666677778888"}};
//		AtRadioInfo_t *info =&info_s;
   if(info ==RT_NULL)
		 return AT_ER_OTHER;
	 char str[3][100]={{"AT+DEVEUI="},{"AT+APPEUI="},{"AT+APPKEY="}};
	 strcat_m(str[0],info->devEui);
	 strcat_m(str[1],info->appEui);
	 strcat_m(str[2],info->appKey);
	 rt_err_t re=0;
	 at_response_t resp =RT_NULL;
	 //设置deveui
   resp =at_create_resp(64,0,rt_tick_from_millisecond(5000));
	 if(resp ==RT_NULL)
	 {return AT_ER_CREAT_RESP;}
	  re=at_exec_cmd(resp,str[0]);
	 if (re ==-1) 
	 { return AT_ER_SEND_ERROR;}
	 if (re ==-2)
	 {return AT_ER_SEND_TIMEOUT;}
		at_delete_resp(resp);
	 //设置appeui
	  resp =at_create_resp(64,0,rt_tick_from_millisecond(5000));
	 if(resp ==RT_NULL)
	 {return AT_ER_CREAT_RESP;}
	  re=at_exec_cmd(resp,str[1]);
	 if (re ==-1) 
	 { return AT_ER_SEND_ERROR;}
	 if (re ==-2)
	 {return AT_ER_SEND_TIMEOUT;}
		at_delete_resp(resp);
	 //设置appkey
	  resp =at_create_resp(100,0,rt_tick_from_millisecond(5000));
	 if(resp ==RT_NULL)
	 {return AT_ER_CREAT_RESP;}
	  re=at_exec_cmd(resp,str[2]);
	 if (re ==-1) 
	 { return AT_ER_SEND_ERROR;}
	 if (re ==-2)
	 {return AT_ER_SEND_TIMEOUT;}
		at_delete_resp(resp);
	 	return AT_ER_OK;
}
//MSH_CMD_EXPORT(at_radio_set_OTAA_info, network access use radio modules);


//校验模组入网信息和控制板保存的入网信息是否匹配,返回为1则表示匹配
AtError_t at_radio_check_OTAA_info(uint8_t rel)
{
  return AT_ER_OK;
}
