#include "warning.h"
#include "hwtimer.h"

#include "MAX30100.h"
#include "MAX30100_PulseOximeter.h"
#include "MAX30100_SpO2Calculator.h"
#include "MAX30100_Filters.h"
#include "timer3.h"
#include "myiic.h"


void Sensor_val_get(volatile my_data_stream *my_data)
{
	//DHT11存储数据结构
	DHT11_Data_TypeDef dht11_info;
	//MAX30100存储数据结构
	max_30100_typedef	max30100_info;
	//体温数据存储
	float body_temperature;
	
	body_temperature = SMBus_ReadTemp();
	
	DHT11_Read_TempAndHumidity(&dht11_info);
	
	checkSample(&max30100_info);
	
	my_data->humidt = dht11_info.humi_int;
	my_data->temp = dht11_info.temp_int;
	my_data->body_temp = body_temperature+10;
	
	my_data->heart_beats = max30100_info.HeartRate_dat;
	my_data->blood_oxygen = max30100_info.SPO2_dat;
	
	sms_warning(&my_data);
}

sms_warning(my_data_stream *my_data)
{
	if (my_data.body_temp < 37)
	{
		
	}
}


