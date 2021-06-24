#ifndef AT_CLIENT_RADIO_H
#define AT_CLIENT_RADIO_H


typedef enum eAtError
{
	 AT_ER_OK=0,
   AT_ER_CREAT_RESP,
	 AT_ER_SEND_ERROR,
	 AT_ER_SEND_TIMEOUT,
	 AT_ER_GET_LINE,
	 AT_ER_GET_KW,
	 AT_ER_PARSE_LINE,
	 AT_ER_PARSE_KW,
	 AT_ER_OTHER,
}AtError_t;

typedef struct sAtUartCfg
{
   int baudrate;
	 int databits;
	 int stopbits;
	 int parity;
	 int control;
}AtUartCfg_t;


//射频模块返回状态
typedef enum eAtRadioStatue
{
		AT_RADIO_STATUE_SLEEP, 				//休眠中
		AT_RADIO_STATUE_JOIN, 				//入网中
		AT_RADIO_STATUE_SEND, 				//发送中
		AT_RADIO_STATUE_CYCLE_SEND,		//周期发送中
		AT_RADIO_STATUE_NULL,					//初始化中
	  AT_RADIO_STATUE_WAIT,          //等待入网中
	  AT_RADIO_STATUE_LOSE, 				//模组丢失
}AtRadioStatue_t;

typedef struct sAtRadioInfo
{
   char devEui[16+1];
	 char appEui[16+1];
	 char appKey[32+1];
}AtRadioInfo_t;


AtError_t at_radio_find_uart_config(AtUartCfg_t *UartCfg);
AtError_t at_radio_get_statue(AtRadioStatue_t *statue);
AtError_t at_radio_send_hex(uint8_t *data,uint8_t size);
AtError_t at_radio_set_OTAA_info(AtRadioInfo_t *info);
AtError_t at_radio_join_net(void);
AtError_t at_radio_set_autojoin(uint8_t mode);
#endif
