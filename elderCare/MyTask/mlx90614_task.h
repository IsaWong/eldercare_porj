#ifndef __MLX90614_TASK_H
#define __MLX90614_TASK_H

#include "FreeRTOS.h"
#include "task.h"
#include "mlx90614.h"

#define MLX90614_TASK_PRIO      3
#define MLX90614_STK_SIZE       50
TaskHandle_t    MLX90614_Task_Handler;
void mlx90614_task(void *p_arg);

#endif
