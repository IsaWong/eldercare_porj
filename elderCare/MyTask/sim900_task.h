#ifndef __SIM900_TASK_H
#define __SIM900_TASK_H

#include "sim900.h"
#include "FreeRTOS.h"
#include "task.h"

#define SIM900_TASK_PRIO        4
#define SIM900_STK_SIZE         50
TaskHandle_t    SIM900_Task_Handler;
void SIM900_task(void *p_arg);

#endif
