
//网络设备
#include "sim900.h"


//协议文件
#include "onenet.h"
#include "edpkit.h"

//硬件驱动
#include "./usart/bsp_usart.h"
#include "./systick/bsp_SysTick.h"
#include "./dht11/bsp_dht11.h"
#include "mlx90614.h"


//C库
#include <string.h>
#include <stdio.h>




extern unsigned char SIM900_buf[128];
_Bool  heart_flag;
u8 err_count;

short time=30;



/*
*功能：和云平台创建连接
*入口参数 无
*返回参数 布尔值
*/
_Bool OneNet_DevLink(void)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};				//协议包

	unsigned char *dataPtr;
	
	unsigned char status = 1;
	
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                        "DEVID: %s,     APIKEY: %s\r\n"
                        , DEVID, APIKEY);

	if(EDP_PacketConnect1(DEVID, APIKEY, 256, &edpPacket) == 0)		//根据devid 和 apikey封装协议包
	{
		SIM900_SendData(edpPacket._data, edpPacket._len);			//上传平台

//		hexdump(edpPacket._data,edpPacket._len);
		dataPtr = SIM900_GetIPD(250);								//等待平台响应

		if(dataPtr != NULL)
		{
			if(EDP_UnPacketRecv(dataPtr) == CONNRESP)
			{
				switch(EDP_UnPacketConnectRsp(dataPtr))
				{
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：设备ID鉴权失败\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户ID鉴权失败\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：未授权\r\n");break;
					case 6:UsartPrintf(USART_DEBUG, "WARN:	连接失败：授权码无效\r\n");break;
					case 7:UsartPrintf(USART_DEBUG, "WARN:	连接失败：激活码未分配\r\n");break;
					case 8:UsartPrintf(USART_DEBUG, "WARN:	连接失败：该设备已被激活\r\n");break;
					case 9:UsartPrintf(USART_DEBUG, "WARN:	连接失败：重复发送连接请求包\r\n");break;
					
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		EDP_DeleteBuffer(&edpPacket);								//删包
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
	
	
//	/*DHT11数据上传*/
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
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(my_data_stream *my_data_STREAM)
{
	
	EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-EDP\r\n");
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf,my_data_STREAM);	//获取当前需要发送的数据流的总长度
	
	
	if(body_len)
	{
		if(EDP_PacketSaveData(DEVID, body_len, NULL, kTypeSimpleJsonWithoutTime, &edpPacket) == 0)	//封包
		{
			for(; i < body_len; i++)
				edpPacket._data[edpPacket._len++] = buf[i];
			
			SIM900_SendData(edpPacket._data, edpPacket._len);										//上传数据到平台
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", edpPacket._len);
			
			hexdump(edpPacket._data,edpPacket._len);
		
			EDP_DeleteBuffer(&edpPacket);															//删包
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	EDP_NewBuffer Failed\r\n");
	}
	SIM900_Clear();
	
}

/*
平台返回数据
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
	
	heart_flag=Heart_err;//先初始化未丢失
	hexdump(edpPacket._data,edpPacket._len);//打印收到的信息
	type = EDP_UnPacketRecv(cmd);
	
	switch(type)										//判断是pushdata还是命令下发
	{
		case PUSHDATA:									//解pushdata包
			
			result = EDP_UnPacketPushData(cmd, &cmdid_devid, &req, &req_len);
		
			if(result == 0)
				UsartPrintf(USART_DEBUG, "src_devid: %s, req: %s, req_len: %d\r\n", cmdid_devid, req, req_len);
			
		break;
		case CMDREQ:									//解命令包
			
			result = EDP_UnPacketCmd(cmd, &cmdid_devid, &cmdid_len, &req, &req_len);
			
			if(result == 0)								//解包成功，则进行命令回复的组包
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
			printf("心跳回应");
			heart_flag=Heart_OK;
		break;
				
			
		default:
			result = -1;
		break;
	}
	
	SIM900_Clear();
	/*命令处理部分*/
	if(result == -1)
		return;
	
	dataPtr = strchr(req, ':');
	if(dataPtr != NULL)									//如果找到了
	{
		
	}
		
		//这里没有结束！！
	/*命令处理部分结束*/
	
		
	if(type == CMDREQ && result == 0)						//如果是命令包 且 解包成功
	{
		EDP_FreeBuffer(cmdid_devid);						//释放内存
		EDP_FreeBuffer(req);
															//回复命令
		SIM900_SendData(edpPacket._data, edpPacket._len);	//上传平台
		EDP_DeleteBuffer(&edpPacket);						//删包
	}
	
}

void Send_heart(void)
{
		EDP_PACKET_STRUCTURE edpPacket = {NULL, 0, 0, 0};	
		if(!EDP_PacketPing(&edpPacket))
		{
		printf("send ping pkt to server, bytes: %d\r\n", edpPacket._len);
		SIM900_SendData(edpPacket._data, edpPacket._len);				//上传数据到平台
		hexdump(edpPacket._data,edpPacket._len);
		EDP_DeleteBuffer(&edpPacket);									//删包
		}
		else 
		printf("心跳失败");
		
}







