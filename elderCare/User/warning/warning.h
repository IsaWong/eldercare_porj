#ifndef __WARNING_H_
#define __WARNING_H_

#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./dht11/bsp_dht11.h"
#include "mlx90614.h"

#include "onenet.h"
#include <stdlib.h>
#include <string.h>


void Sensor_val_get(volatile my_data_stream *my_data);
#endif
