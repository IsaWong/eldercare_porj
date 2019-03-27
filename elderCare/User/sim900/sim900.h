#ifndef _SIM900_H_
#define _SIM900_H_

//C��
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "onenet.h"



#define REV_OK		0	//������ɱ�־
#define REV_WAIT	1	//����δ��ɱ�־


void SIM900_Init(void);

void SIM900_Clear(void);

void SIM900_SendData(unsigned char *data, unsigned short len);

unsigned char *SIM900_GetIPD(unsigned short timeOut);
unsigned char Check_net(void);

#endif
