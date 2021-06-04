#ifndef __N58_H__
#define __N58_H__

#include "delay.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define N58_USART(fmt, ...)	 USART_printf (USART2, fmt, ##__VA_ARGS__)
#define PC_USART(fmt, ...)	 printf (fmt, ##__VA_ARGS__)

#define RX_BUF_MAX_LEN 2048		  //最大接收缓存字节数
#define TX_LIST_MAX_LEN	20			//发送命令列表最大命令个数

extern struct STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
{
	u8 bData_RX_BUF[RX_BUF_MAX_LEN];
	union 
	{
    	__IO u16 wInfAll;
    	struct 
		{
		  	__IO u16 wFramLength       :15;                               // 14:0 
		  	__IO u16 wFramFinishFlag   :1;                                // 15 
	  	}wInfBit;
  	}; 
}N58_Fram_Record_Struct;

extern struct STRUCT_USART_Send	  //定义一个全局串口发送的处理结构体
{
	u8 bTask_Cmd_List[TX_LIST_MAX_LEN];
	u8 bTask_Ack_List[TX_LIST_MAX_LEN];
	u8 bResend_Count;
	union
	{
		__IO u8 bInfAll;
    struct 
		{
		  __IO u8 bSending_Flag   		:1;                              // 0 发送应答标志
	  	__IO u8 bSend_Finish				:1; 														 // 1
		  __IO u8 bFinish_state				:1;															 // 2
			__IO u8 bRestart_Check			:1;															 // 3
			__IO u8 bResend_Flag				:1;															 // 4
			__IO u8 bResponse_Flag			:3;															 // 5-6
	  }bInfBit;
  }; 
}N58_Send_Struct;

extern volatile u8 bRestart_flag;
extern u8 bN58_AT_Cmd_List[][40];
extern u8 bN58_AT_Ack_List[][20];

u8 *bN58_Check_Data(u8 *str);
void N58_Send_AT_Cmd(u8 *bCmd,u8 *bAck);
u8 bN58_Send_Data(u8 *cmd,u8 *ack1,u8 *ack2,u8 time,u8 send_count);
void N58_Init(void);
void N58_Ppp_Init(void);
void N58_Http_Get(u8 *url,u8 *port,u8 *offset,u8 *size);
void N58_Http_Head(u8 *url,u8 *port);
void N58_Tts_Set(u8 volume,u8 speed,u8 pitch,u8 format);
void N58_Tts_Play(u8 size, u8 *code);


void USART_printf( USART_TypeDef * USARTx, char * Data, ... );
void USART2_Init(u32 bound);				//串口2初始化 
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);


#endif
