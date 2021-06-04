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

#define N58_USART(fmt, ...)	 USART_printf (USART2, fmt, ##__VA_ARGS__)	//向N58串口发送数据
#define PC_USART(fmt, ...)	 printf (fmt, ##__VA_ARGS__)								//显示N58返回的数据

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
	u8 bTask_Cmd_List[TX_LIST_MAX_LEN];			//命令发送任务数组
	u8 bTask_Ack_List[TX_LIST_MAX_LEN];			//应答任务数组
	u8 bResend_Count,bData_Mode;						//失败重发次数，，数据发送模式标志
	union
	{
		__IO u8 bInfAll;
    struct 
		{
		  __IO u8 bSending_Flag   		:1;     // 0		正在发送标志
	  	__IO u8 bSend_Finish				:1; 		// 1		发送完成标志
		  __IO u8 bFinish_state				:1;			// 2		命令完成状态标志，0为发送失败
			__IO u8 bRestart_Check			:1;			// 3		重启检测标志，命令发送则开始重启检测
			__IO u8 bResend_Flag				:1;			// 4		重新发送标志
			__IO u8 bResponse_Flag			:3;			// 5-6	应答类别，0为不需要应答，
																					//							 	1为普通应答
																					//								大于1为特殊应答
	  }bInfBit;
  }; 
}N58_Send_Struct;

extern volatile u8 bRestart_flag;					//重启标志
extern u8 bN58_AT_Cmd_List[][80];					//N58 AT指令库
extern u8 bN58_AT_Ack_List[][20];					//N58 AT指令应答库
extern u8 bN58_Data_Post[80];							//N58 发送数据缓存			
extern u8 bN58_Data[1024];

void Send_Cmd(void);											//发送命令函数
void Scan_N58_Usart(void);								//N58串口接收扫描函数
void Scan_Restart(void);									//N58重启扫描函数
void Scan_Send(void);											//发送任务扫描函数

u8 *bN58_Check_Data(u8 *str);								//查找字符串函数
void N58_Send_AT_Cmd(u8 *bCmd,u8 *bAck);		//发送AT指令函数

void N58_Init(void);												//N58模块通讯初始化
void N58_Ppp_Link(void);										//N58 PPP 连接函数
void N58_Http_Get(u8 *url,u8 *port,u8 *offset,u8 *size);		//N58 http get 请求方式
void N58_Http_Head(u8 *url,u8 *port);												//N58 http head 请求方式
void N58_Http_Post(u8 *bUrl,u8 *bPort,u8 bType,u8 *bCode);	//N58 http post 发送数据


void USART_printf( USART_TypeDef * USARTx, char * Data, ... );		//printf函数
void N58_Usart_Init(u32 bound);												//N58串口初始化 
void TIM4_Set(u8 sta);																//N58串口使用的定时器TIM4
void TIM4_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);									//N58重启检测使用的定时器


#endif
