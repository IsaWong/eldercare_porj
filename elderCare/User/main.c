
#include "onenet.h"
#include "EdpKit.h"
//网络设备
#include  "sim900.h"

#include "stm32f10x.h"
#include "./systick/bsp_SysTick.h"
#include "./dht11/bsp_dht11.h"
#include "./usart/bsp_usart.h"
#include "hwtimer.h"
#include "mlx90614.h"
#include "warning.h"

#include <string.h>
#include <stdio.h>

/*系統發送數據主函數*/
my_data_stream my_data_2_update;


enum net_status{Conneted=0,Closed,Lost,Error};
enum data_type Data_type;
enum net_status Net_status;
unsigned short timeCount = 0;	//发送间隔变量
void Hardware_Init()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置
	USART_Config();
	SysTick_Init();
	DHT11_Init ();
	SMBus_Init();
	Usart_2_Config(115200);//gsm串口
	SIM900_Init();
	printf("Hardware init ok\r\n");
}



int main(void)
{
	

	unsigned char *dataPtr = NULL;//命令指针
	unsigned short timeCount = 0;//发送间隔变量

	
	//硬件初始化
	Hardware_Init();
	Delay_ms(500);
	
	while(OneNet_DevLink())			//接入云平台
		Delay_ms(500);
		Net_status=Conneted;
	
	Data_type=Cmd;//这是啥意思？
	while(1)
	{
		if(++timeCount >= 500)
		{
//			get_dht_val();
			Sensor_val_get(&my_data_2_update);
			UsartPrintf(USART_DEBUG, "OneNet_Sending!!!!!\r\n");
			OneNet_SendData(&my_data_2_update);
			timeCount = 0;
			SIM900_Clear();
		}
		
		dataPtr = SIM900_GetIPD(0);
		if(dataPtr != NULL)
		{
			OneNet_RevPro(dataPtr);
		}
		Delay_ms(10);

	}
	
	
}	
	



///*系統測試紅外人體測溫*/
//int main(void)
//{
//	float temp;
//	SysTick_Init();
//	USART_Config();
//	SMBus_Init();
//	printf("\r\n系統初始化成功\r\n");
//	
//	while(1)
//	{
//		temp = SMBus_ReadTemp();
//		printf("\r\n temp is %0.2f\r\n",temp);
//		Delay_ms(500);
//	}
//}


	


