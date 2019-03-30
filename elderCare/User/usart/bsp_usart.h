#ifndef __USART_H
#define	__USART_H


#include "stm32f10x.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#define USART_DEBUG		USART1		//调试打印所使用的串口组
// 串口1-USART1
#define  DEBUG_USARTx                   USART1
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
#define  DEBUG_USART_BAUDRATE           115200

// USART GPIO 引脚宏定义
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
#define  DEBUG_USART_TX_GPIO_PORT         GPIOA   
#define  DEBUG_USART_TX_GPIO_PIN          GPIO_Pin_9
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10

#define  DEBUG_USART_IRQ                USART1_IRQn
#define  DEBUG_USART_IRQHandler         USART1_IRQHandler



void USART_Config(void);
//GSM串口
void Usart_2_Config(unsigned int baud);
void USART_SendByte(USART_TypeDef* USARTx, u8 data);
void USART_Send2Byre(USART_TypeDef* USARTx, u16 data);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

#endif /* __USART_H */
