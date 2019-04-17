

注意#######
sim900文件内的原始sim900_buf[128]聲明
//unsigned char SIM900_buf[128];

定时器使用情况：
timer2---用于心率传感器计时器
timer3---用于gprs数据上传计时器



线序#######
stlink 连接线序：
					SWDIO-grey	PA13
					SWCLK-white	PA14


//原PB6更改为PB5
DHT11:
					DATA		PB5
					
MLX90614:			
					SCL			PB14
					SDA			PB15
					
GSM-USART2
					TX			PA2
					RX 			PA3
					
