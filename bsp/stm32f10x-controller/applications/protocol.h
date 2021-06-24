/*气象站通信协议   V1.0*/

//温度  °c/10  2byte    eg [01 23] = 29.1℃
//湿度  %      1byte    eg [23] = 35%
//风速  0,1m/s 2byte    eg [00][56] = 8.6m/s   
//风向  度     2byte    eg [00][5a] = 90度
//PM2_5        2byte
//光照  lux    2byte
//雨滴  有无   1byte

//协议格式
//|协议版本1byte||
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <rtthread.h>

#define TEMP_SENSOR    				"temp"
#define HUMI_SENSOR    				"humi"
#define PM2_5_SENSOR   				"pm2_5"
#define WIND_SPEND_SENSOR   	    "windDpend"
#define WIND_DIRECTION_SENSOR       "windDirection"
#define RAIN_SENSOR                 "rain"
#define LIGHT_SENSOR                "light"

typedef struct sProtocol
{
   uint8_t version;
   uint8_t amount;
   char sensor[7][20];  //保存名称

}sProtocol_s;

sProtocol_s Ptc={1,7,{TEMP_SENSOR,HUMI_SENSOR,WIND_SPEND_SENSOR,WIND_DIRECTION_SENSOR,PM2_5_SENSOR,LIGHT_SENSOR,RAIN_SENSOR} };

#endif
