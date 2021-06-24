#ifndef _RADIO_CONTROL_H
#define _RADIO_CONTROL_H


void radio_thread_init(void);
int radio_send(uint8_t *data,uint8_t size );
uint8_t radio_read_statue(void);

#endif
