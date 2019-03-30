#ifndef __DHT11_TASK_H
#define __DHT11_TASH_H
#include "FreeRTOS.h"
#include "./dht11/bsp_dht11.h"
#include "task.h"


#define DHT11_TASK_PRIO			2
#define DHT11_STK_SIZE			50
TaskHandle_t	dht11Task_Handler;
void dht11_task(void *p_arg);

#endif
