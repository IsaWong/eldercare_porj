
//�����豸
#include "sim900.h"


//Э���ļ�
#include "onenet.h"
#include "edpkit.h"

//Ӳ������
#include "./usart/bsp_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./dht11/bsp_dht11.h"
#include "mlx90614.h"


//C��
#include <string.h>
#include <stdio.h>




extern unsigned char SIM900_buf[128];
_Bool  heart_flag;
u8 err_count;

short time=30;



/*
*���ܣ�����ƽ̨��������
*��ڲ��� ��
*���ز��� ����ֵ
*/
_Bool OneNet_DevLink(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//Э���

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , DEVID, APIKEY);

	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//����devid �� apikey��װЭ���
	{
		SIM900_SendData(edpPacket._data, edpPacket._len);			//�ϴ�ƽ̨

//		hexdump(edpPacket._data,edpPacket._len);
		dataPtr = SIM900_GetIPD(250);								//�ȴ�ƽ̨��Ӧ

		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��豸ID��Ȩʧ��\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��û�ID��Ȩʧ��\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�δ��Ȩ\r\n");break;
					case 6:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ���Ȩ����Ч\r\n");break;
					case 7:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ�������δ����\r\n");break;
					case 8:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ����豸�ѱ�����\r\n");break;
					case 9:UsartPrintf(USART_DEBUG, "WARN:	����ʧ�ܣ��ظ��������������\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		EDP_DeleteBuffer(&edpPacket);								//ɾ��
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	EDP_PacketConnect Failed\r\n");
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf,my_data_stream *my_data_STREAM)
{

	char text[16];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{");
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"humidity\":%d,", my_data_STREAM->humidt);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "\"temperature\":%d,", my_data_STREAM->temp);
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"bodytemperature\":%d", my_data_STREAM->body_temp);
	strcat(buf, text);	
	
	
//	/*DHT11�����ϴ�*/
//	memset(text, 0, sizeof(text));
//	sprintf(text, "\"humidity\":%d,", dht11_info.humi_int);
//	strcat(buf, text);	
//	
//	memset(text, 0, sizeof(text));
//	sprintf(text, "\"temperature\":%d", dht11_info.temp_int);
//	strcat(buf, text);	
	
	
	strcat(buf, "}");
	
	return strlen(buf);
}





//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_SendData(my_data_stream *my_data_STREAM)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//Э���
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-EDP\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf,my_data_STREAM);	//��ȡ��ǰ��Ҫ���͵����������ܳ���
	
	
	if(body_len)
	{
		if(EDP_PacketSaveData(DEVID, body_len, NULL, kTypeSimpleJsonWithoutTime, &edpPacket) == 0)	//���
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
			SIM900_SendData(edpPacket._data, edpPacket._len);										//�ϴ����ݵ�ƽ̨
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
			
			hexdump(edpPacket._data,edpPacket._len);
		
			EDP_DeleteBuffer(&edpPacket);															//ɾ��
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	SIM900_Clear();
	
}

/*
ƽ̨��������
*/
void OneNet_RevPro(unsigned char *cmd)
{
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};
	char *cmdid_devid = NULL;
	unsigned short cmdid_len = 0;
	char *req = NULL;
	unsigned int req_len = 0;
	unsigned char type = 0;	
	
	short result = 0;
	
	char *dataPtr = NULL;
	
	heart_flag=Heart_err;//�ȳ�ʼ��δ��ʧ
	hexdump(edpPacket._data,edpPacket._len);//��ӡ�յ�����Ϣ
	type = EDP_UnPacketRecv(cmd);
	
	switch(type)										//�ж���pushdata���������·�
	{
		case PUSHDATA:									//��pushdata��
			
			result = EDP_UnPacketPushData(cmd, &cmdid_devid, &req, &req_len);
		
			if(result == 0)
				UsartPrintf(USART_DEBUG, "src_devid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			
		break;
		case CMDREQ:									//�������
			
			result = EDP_UnPacketCmd(cmd, &cmdid_devid, &cmdid_len, &req, &req_len);
			
			if(result == 0)								//����ɹ������������ظ������
			{
				EDP_PacketCmdResp(cmdid_devid, cmdid_len, req, req_len, &edpPacket);
				UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			}
			
		break;
			
		case SAVEACK:
			
			if(cmd[3] == MSG_ID_HIGH && cmd[4] == MSG_ID_LOW)
			{
				UsartPrintf(USART_DEBUG, "Tips:	Send %s\r\n", cmd[5] ? "Err" : "Ok");
			}
			else
				UsartPrintf(USART_DEBUG, "Tips:	Message ID Err\r\n");
			
		break;
		case PINGRESP:
			printf("������Ӧ");
			heart_flag=Heart_OK;
		break;
				
			
		default:
			result = -1;
		break;
	}
	
	SIM900_Clear();
	/*�������*/
	if(result == -1)
		return;
	
	dataPtr = strchr(req, ':');
	if(dataPtr != NULL)									//����ҵ���
	{
		
	}
		
		//����û�н�������
	/*������ֽ���*/
	
		
	if(type == CMDREQ && result == 0)						//���������� �� ����ɹ�
	{
		EDP_FreeBuffer(cmdid_devid);						//�ͷ��ڴ�
		EDP_FreeBuffer(req);
															//�ظ�����
		SIM900_SendData(edpPacket._data, edpPacket._len);	//�ϴ�ƽ̨
		EDP_DeleteBuffer(&edpPacket);						//ɾ��
	}
	
}

void Send_heart(void)
{
		EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	
		if(!EDP_PacketPing(&edpPacket))
		{
		printf("send ping pkt to server, bytes: %d\r\n", edpPacket._len);
		SIM900_SendData(edpPacket._data, edpPacket._len);				//�ϴ����ݵ�ƽ̨
		hexdump(edpPacket._data,edpPacket._len);
		EDP_DeleteBuffer(&edpPacket);									//ɾ��
		}
		else 
		printf("����ʧ��");
		
}







