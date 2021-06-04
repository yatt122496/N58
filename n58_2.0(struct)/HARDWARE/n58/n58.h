#ifndef __N58_H__
#define __N58_H__

#include "delay.h"
#include "includes.h"


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define N58_USART(fmt, ...)	 USART_printf (USART2, fmt, ##__VA_ARGS__)
#define PC_USART(fmt, ...)	 printf (fmt, ##__VA_ARGS__)

#define RX_BUF_MAX_LEN 2048		  //最大接收缓存字节数

extern struct STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
{
	u8 Data_RX_BUF[RX_BUF_MAX_LEN];
	union 
	{
    	__IO u16 InfAll;
    	struct 
		{
		  	__IO u16 FramLength       :15;                               // 14:0 
		  	__IO u16 FramFinishFlag   :1;                                // 15 
	  	}InfBit;
  	}; 
}N58_Fram_Record_Struct;


u8 *N58_Check_Data(u8 *str);
u8 N58_Send_AT_Cmd(u8 *cmd,u8 *ack1,u8 *ack2,u16 time);
u8 N58_Send_Data(u8 *cmd,u8 *ack1,u8 *ack2,u16 time);
u8 N58_Init(void);
u8 N58_Ppp_Init(void);
u8 *N58_Http_Get(u8 *url,u8 *port,u8 *offset,u8 *size);
u8 *N58_Http_Head(u8 *url,u8 *port);
u8 N58_Tts_Set(u8 volume,u8 speed,u8 pitch,u8 format);
u8 *N58_Tts_Play(u8 size, u8 *code);


void USART_printf( USART_TypeDef * USARTx, char * Data, ... );
void USART2_Init(u32 bound);				//串口2初始化 
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);


#endif
