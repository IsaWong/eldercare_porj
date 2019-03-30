#include "warning.h"
#include "hwtimer.h"


void Sensor_val_get(volatile my_data_stream *my_data)
{
	DHT11_Data_TypeDef dht11_info;
	float body_temperature;
	
	body_temperature = SMBus_ReadTemp();
	DHT11_Read_TempAndHumidity(&dht11_info);
	
	my_data->humidt = dht11_info.humi_int;
	my_data->temp = dht11_info.temp_int;
	my_data->body_temp = body_temperature;
	
}
