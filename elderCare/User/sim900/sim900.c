//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"


//�����豸����
#include "sim900.h"

//Ӳ������
#include "./systick/bsp_SysTick.h"
#include "./usart/bsp_usart.h"

#define SIM900_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",876\r"

char SIM900_buf[128];
//unsigned char SIM900_buf[128];
unsigned short SIM900_cnt = 0, SIM900_cntPre = 0;

//==========================================================
//	�������ƣ�	SIM900_Clear
//
//	�������ܣ�	��ջ���	
//==========================================================
void SIM900_Clear(void)
{

	memset(SIM900_buf, 0, sizeof(SIM900_buf));
	SIM900_cnt = 0;

}

//==========================================================
//	�������ƣ�	SIM900_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
_Bool SIM900_WaitRecive(void)
{

	if(SIM900_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
		return REV_WAIT;
		
	if(SIM900_cnt == SIM900_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
	{
		SIM900_cnt = 0;							//��0���ռ���
			
		return REV_OK;							//���ؽ�����ɱ�־
	}
		
	SIM900_cntPre = SIM900_cnt;					//��Ϊ��ͬ
	
	return REV_WAIT;							//���ؽ���δ��ɱ�־

}
_Bool sim900_sendEnd(u8 cmd,char *res)
{
	unsigned char timeOut = 200;

	USART_SendData(USART2,cmd);
	
	while(timeOut--)
	{
		if(SIM900_WaitRecive() == REV_OK)							//����յ�����
		{
							printf(SIM900_buf);

			if(strstr((const char *)SIM900_buf, res) != NULL)		//����������ؼ���
			{
				
				return 0;
			}
		}
		
		Delay_ms(10);
	}
	
	return 1;

	
}



//==========================================================
//	�������ƣ�	SIM900_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����		
//==========================================================
_Bool SIM900_SendCmd(char *cmd, char *res)
{
	
	unsigned int timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(SIM900_WaitRecive() == REV_OK)							//����յ�����
		{
							printf(SIM900_buf);

			if(strstr((const char *)SIM900_buf, res) != NULL)		//����������ؼ���
			{

				SIM900_Clear();										//��ջ���
				
				return 0;
			}
		}
		
		Delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	�������ƣ�	SIM900_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void SIM900_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	SIM900_Clear();								//��ս��ջ���
	sprintf(cmdBuf, "AT+CIPSEND=%d\r", len);	//��������
	while(SIM900_SendCmd(cmdBuf,">"))				//�յ���>��ʱ���Է�������
	{
		Delay_ms(500);

	}
	Usart_SendString(USART2, data, len);	//�����豸������������

	while(sim900_sendEnd(0x1A,"SEND OK"))
	{

	  Delay_ms(500);
	  printf("send..");
	}
	
}

//==========================================================
//	�������ƣ�	SIM900_GetIPD
//
//	�������ܣ�	��ȡƽ̨���ص�����
//
//	��ڲ�����	timeOut�ȴ���ʱ��(����10ms)
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		��ͬ�����豸���صĸ�ʽ��ͬ����Ҫȥ����
//==========================================================
unsigned char *SIM900_GetIPD(unsigned short timeOut)
{

	char *ptrIPD;
	
	do
	{
		if(SIM900_WaitRecive() == REV_OK)								//����������
		{
			ptrIPD = strstr((char *)SIM900_buf, "+IPD");				//������IPDATA��ͷ
			if(ptrIPD == NULL)											//���û�ҵ���������IPDATAͷ���ӳ٣�������Ҫ�ȴ�һ�ᣬ�����ᳬ���趨��ʱ��
			{
				UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				ptrIPD = strchr(ptrIPD, ':');							//�ҵ�'\n'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}
		
		Delay_ms(10);													//��ʱ�ȴ�
	} while(timeOut--);
	
	return NULL;														//��ʱ��δ�ҵ������ؿ�ָ��

}

//==========================================================
//	�������ƣ�	SIM900_Init
//
//	�������ܣ�	��ʼ��SIM900
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void SIM900_Init(void)
{

	SIM900_Clear();
	
	UsartPrintf(USART_DEBUG, "1. ATE0\r");
	while(SIM900_SendCmd("ATE0\r", "OK"))
		Delay_ms(1000);
	
//	UsartPrintf(USART_DEBUG, "2. AT+CPIN?\r");
//	while(SIM900_SendCmd("AT+CPIN?\r", "+CPIN:READY"))		//ȷ��SIM��PIN�����������READY����ʾ�����ɹ�
//		Delay_ms(1000);
	UsartPrintf(USART_DEBUG, "2. AT+CPIN?\r");
	while(SIM900_SendCmd("AT+CPIN?\r", "OK"))		//ȷ��SIM��PIN�����������READY����ʾ�����ɹ�
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "3. AT+CSQ\r");
	while(SIM900_SendCmd("AT+CSQ\r", "OK"))
		Delay_ms(1000);
	UsartPrintf(USART_DEBUG, "3. AT+CIPSHUT\r");
	while(SIM900_SendCmd("AT+CIPSHUT=?\r", "OK"))
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "4. AT+CGCLASS=\"B\r");
	while(SIM900_SendCmd("AT+CGCLASS=\"B\"\r","OK"))					//�������ע��״̬
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "5. AT+CGDCONT=1,1\r");
	while(SIM900_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r","OK"))				//����
		Delay_ms(1000);
	
	
	UsartPrintf(USART_DEBUG, "6. AT+CGATT=1\r");
	while(SIM900_SendCmd("AT+CGATT=1\r","OK"))					//����GPRSҵ��
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "5. AT+CIPCSGP=1\r\n");
	while(SIM900_SendCmd("AT+CIPCSGP=1,\"CMNET\"\r","OK"))				//����
		Delay_ms(1000);
	UsartPrintf(USART_DEBUG, "5. AT+CIPMUX=0");
	while(SIM900_SendCmd("AT+CIPMUX=0\r","OK"))				//����
		Delay_ms(1000);
			UsartPrintf(USART_DEBUG, "5. AT+CMMUX=0\r\n");
		while(SIM900_SendCmd("AT+CIPMUX=0\r","OK"))					//����Ϊ�����ӣ���Ȼƽ̨IP��������
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "8. AT+CMHEAD=1\r\n");
	while(SIM900_SendCmd("AT+CIPHEAD=1\r","OK"))				//��ʾIPͷ
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "9. AT+IPSTART\r\n");
	while(SIM900_SendCmd(SIM900_ONENET_INFO,"CONNECT"))				//������ƽ̨
		Delay_ms(1000);
	
	UsartPrintf(USART_DEBUG, "10. SIM900 Init OK\r\n");

}



void USART2_IRQHandler(void)
{

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
	{
		if(SIM900_cnt >= sizeof(SIM900_buf))	
			SIM900_cnt = 0; //��ֹ���ڱ�ˢ��
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
	if(OneNet_DevLink())			//����OneNET
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
	else if(strstr((const char *)SIM900_buf,"CLOSSED")) //ʧȥ����  2
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
