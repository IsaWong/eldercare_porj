//单片机头文件
#include "stm32f10x.h"


//网络设备驱动
#include "sim900.h"

//硬件驱动
#include "./systick/bsp_SysTick.h"
#include "./usart/bsp_usart.h"

#define SIM900_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r"

char SIM900_buf[128];
//unsigned char SIM900_buf[128];
unsigned short SIM900_cnt = 0, SIM900_cntPre = 0;

//==========================================================
//	函数名称：	SIM900_Clear
//
//	函数功能：	清空缓存	
//==========================================================
void SIM900_Clear(void)
{

	memset(SIM900_buf, 0, sizeof(SIM900_buf));
	SIM900_cnt = 0;

}

//==========================================================
//	函数名称：	SIM900_WaitRecive
//
//	函数功能：	等待接收完成
//
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool SIM900_WaitRecive(void)
{

	if(SIM900_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(SIM900_cnt == SIM900_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		SIM900_cnt = 0;							//清0接收计数
			
		return REV_OK;							//返回接收完成标志
	}
		
	SIM900_cntPre = SIM900_cnt;					//置为相同
	
	return REV_WAIT;							//返回接收未完成标志

}
_Bool sim900_sendEnd(u8 cmd,char *res)
{
	unsigned char timeOut = 200;

	USART_SendData(USART2,cmd);
	
	while(timeOut--)
	{
		if(SIM900_WaitRecive() == REV_OK)							//如果收到数据
		{
							printf(SIM900_buf);

			if(strstr((const char *)SIM900_buf, res) != NULL)		//如果检索到关键词
			{
				
				return 0;
			}
		}
		
		Delay_ms(10);
	}
	
	return 1;

	
}



//==========================================================
//	函数名称：	SIM900_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool SIM900_SendCmd(char *cmd, char *res)
{
	
	unsigned int timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(SIM900_WaitRecive() == REV_OK)							//如果收到数据
		{
							printf(SIM900_buf);

			if(strstr((const char *)SIM900_buf, res) != NULL)		//如果检索到关键词
			{

				SIM900_Clear();										//清空缓存
				
				return 0;
			}
		}
		
		Delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	SIM900_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void SIM900_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	SIM900_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r", len);	//发送命令
	while(SIM900_SendCmd(cmdBuf,">"))				//收到‘>’时可以发送数据
	{
		Delay_ms(500);

	}
	Usart_SendString(USART2, data, len);	//发送设备连接请求数据

	while(sim900_sendEnd(0x1A,"SEND OK"))
	{

	  Delay_ms(500);
	  printf("send..");
	}
	
}

//==========================================================
//	函数名称：	SIM900_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	timeOut等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//==========================================================
unsigned char *SIM900_GetIPD(unsigned short timeOut)
{

	char *ptrIPD;
	
	do
	{
		if(SIM900_WaitRecive() == REV_OK)								//如果接收完成
		{
			ptrIPD = strstr((char *)SIM900_buf, "+IPD");				//搜索“IPDATA”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPDATA头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//找到'\n'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}
		
		Delay_ms(10);													//延时等待
	} while(timeOut--);
	
	return NULL;														//超时还未找到，返回空指针

}

//==========================================================
//	函数名称：	SIM900_Init
//
//	函数功能：	初始化SIM900
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void SIM900_Init(void)
{

	SIM900_Clear();
	
	UsartPrintf(USART_DEBUG, "1. ATE0\r");
	while(SIM900_SendCmd("ATE0\r", "OK"))
		Delay_ms(1000);
	
//	UsartPrintf(USART_DEBUG, "2. AT+CPIN?\r");
//	while(SIM900_SendCmd("AT+CPIN?\r", "+CPIN:READY"))		//确保SIM卡PIN码解锁，返回READY，表示解锁成功
//		Delay_ms(1000);
	UsartPrintf(USART_DEBUG, "2. AT+CPIN?\r");
	while(SIM900_SendCmd("AT+CPIN?\r", "OK"))		//确保SIM卡PIN码解锁，返回READY，表示解锁成功
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "3. AT+CSQ\r");
	while(SIM900_SendCmd("AT+CSQ\r", "OK"))
		Delay_ms(1000);
	UsartPrintf(USART_DEBUG, "3. AT+CIPSHUT\r");
	while(SIM900_SendCmd("AT+CIPSHUT=?\r", "OK"))
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "4. AT+CGCLASS=\"B\r");
	while(SIM900_SendCmd("AT+CGCLASS=\"B\"\r","OK"))					//检查网络注册状态
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "5. AT+CGDCONT=1,1\r");
	while(SIM900_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r","OK"))				//激活
		Delay_ms(1000);
	
	
	UsartPrintf(USART_DEBUG, "6. AT+CGATT=1\r");
	while(SIM900_SendCmd("AT+CGATT=1\r","OK"))					//附着GPRS业务
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "5. AT+CIPCSGP=1\r\n");
	while(SIM900_SendCmd("AT+CIPCSGP=1,\"CMNET\"\r","OK"))				//激活
		Delay_ms(1000);
	UsartPrintf(USART_DEBUG, "5. AT+CIPMUX=0");
	while(SIM900_SendCmd("AT+CIPMUX=0\r","OK"))				//激活
		Delay_ms(1000);
			UsartPrintf(USART_DEBUG, "5. AT+CMMUX=0\r\n");
		while(SIM900_SendCmd("AT+CIPMUX=0\r","OK"))					//必须为单连接，不然平台IP都连不上
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "8. AT+CMHEAD=1\r\n");
	while(SIM900_SendCmd("AT+CIPHEAD=1\r","OK"))				//显示IP头
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "9. AT+IPSTART\r\n");
	while(SIM900_SendCmd(SIM900_ONENET_INFO,"CONNECT"))				//连接云平台
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "10. SIM900 Init OK\r\n");

}



void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(SIM900_cnt >= sizeof(SIM900_buf))	
			SIM900_cnt = 0; //防止串口被刷爆
		SIM900_buf[SIM900_cnt++] = USART2->DR;
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
		
	}

}

void reLink(void )
{
	UsartPrintf(USART_DEBUG, "9. AT+IPSTART\r\n");
	while(SIM900_SendCmd("AT+CIPSTATUS\r","CONNECT"))				//
		Delay_ms(1000);
	
	
	UsartPrintf(USART_DEBUG, "relink OK\r\n");
	if(OneNet_DevLink())			//接入OneNET
		Delay_ms(1000);
	
}


u8 Check_net(void )
{
	
	SIM900_SendCmd("AT+CIPSTATUS\r","STATUS");
	if(strstr((const char *)SIM900_buf,"CONNECT OK"))
	{
			printf("\r\ngprs link normal\r\n");

		return 0;		
		
	}
	else if(strstr((const char *)SIM900_buf,"CLOSSED")) //失去连接  2
	{
		printf("\r\ntcp link lost\r\n");
		reLink();

		return 1;
		
	}	
	else if(strstr((const char *)SIM900_buf,"STATUS:5"))
	{
		printf("\r\ngprs  is unusable\r\n");
		SIM900_Clear(); 
		return 2;		
	}
	else
	{
		return 3;
	}
	
}
