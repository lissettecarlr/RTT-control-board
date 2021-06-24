/*
 * Change Logs:
 * Date           Author              Notes
 * 2019-01-5     lissettecarlr       the first version
 */

#include <fal.h>
#include "rtthread.h"

#define EEPROM_BASE_ADDR    0x08080000 

static int read(long offset, uint8_t *buf, size_t size)
{
    uint8_t *wAddr;
	  size_t i;
    wAddr=(uint8_t *)(EEPROM_BASE_ADDR+offset);  
	   
	  for (i = 0; i < size; i++, wAddr++, buf++)
	  {
		   *buf = *(uint8_t *) wAddr;
		}
			
//    while(size--){  
//        *buf++=*wAddr++;  
//    }   
    return size;
}


static int write(long offset, const uint8_t *buf, size_t size)
{
       size_t i;
       uint32_t read_data;
       uint32_t addr = EEPROM_BASE_ADDR + offset;
   	   HAL_FLASHEx_DATAEEPROM_Unlock();

       for (i = 0; i < size; i++, buf++, addr++)
       {
          // HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEERASEDATA_BYTE,addr);
           HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_FASTBYTE , addr, *buf);
           read_data = *(uint8_t *) addr;
          /* check data */
          if (read_data != *buf)
          {
            return -1;
          }
       }		
	   HAL_FLASHEx_DATAEEPROM_Lock();	
       return size;	
}

static int erase(long offset, size_t size)
{
    size_t i;
    uint32_t addr = EEPROM_BASE_ADDR + offset;

    for(i=0;i<size;i++ , addr++)
    {
       HAL_FLASHEx_DATAEEPROM_Erase(FLASH_TYPEERASEDATA_BYTE,addr);
    }
    

    return size;
}
const struct fal_flash_dev stm32l_eeprom = { "stm32l_eeprom", 0, 2*1024, 2048, {NULL, read, write, erase} };
