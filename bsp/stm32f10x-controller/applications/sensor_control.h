#ifndef _SENSOR_CONTRIL_H
#define _SENSOR_CONTRIL_H

void sensor_thread_init(void);
void sensor_close_update(void);
void sensor_open_update(void);
uint8_t sensor_get_data(uint8_t *buf);

#endif
