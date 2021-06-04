#ifndef __N58_H__
#define __N58_H__

#include "sys.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>


#if defined ( __CC_ARM   )
#pragma anon_unions
#endif


#define cRX_BUF_MAX_LEN 1024		  //最大接收缓存字节数
#define cTX_LIST_MAX_LEN	20			//发送命令列表最大命令个数

#define cN58_USART USART2				//N58 通讯串口，，波特率为115200

extern struct STRUCT_USART_Fram	  //定义一个全局串口数据帧的处理结构体
{
	u8 bData_RX_BUF[cRX_BUF_MAX_LEN];
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
	u8 bTask_Cmd_List[cTX_LIST_MAX_LEN];			//命令发送任务数组
	u8 bTask_Ack_List[cTX_LIST_MAX_LEN];			//应答任务数组
	u8 bResend_Count,bData_Mode,bNow_Task;						//失败重发次数，，数据发送模式标志
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

extern u8 bRestart_flag;					//重启标志
extern u8 bN58_AT_Cmd_List[][80];					//N58 AT指令库
extern u8 bN58_AT_Ack_List[][40];					//N58 AT指令应答库
extern u8 bN58_Data_Post[80];							//N58 发送数据缓存			
extern u8 bN58_Data[1024];
extern u16 bRestart_Check_CNT;

void Send_Cmd(void);											//发送命令函数

u8 *bN58_Check_Data(u8 *str);								//查找字符串函数
void N58_Send_AT_Cmd(u8 *bCmd,u8 *bAck);		//发送AT指令函数

void N58_printf(char* fmt,...);							//printf函数
void N58_Usart_Send(u8 *bData,u8 bLen);			//N58发送字节函数

void N58_Init(void);												//N58模块通讯初始化
void N58_Ppp_Link(void);										//N58 PPP 连接函数
void N58_Http_Set(u8 *bUrl,u8 *bPort);
void N58_Http_Get(u8 *offset,u8 *size);					//N58 http get 请求方式
void N58_Http_Head(void);												//N58 http head 请求方式
void N58_Http_Post(u8 bType,u8 *bCode,u8 bLen);	//N58 http post 发送数据
//void N58_Tcp_Link(u8 *bUrl,u8 *bPort,u8 *bCode);





//////////////////////////需要使用到的函数
void Scan_N58_Usart(void);								//N58串口接收扫描函数
void Scan_Restart(void);									//N58重启扫描函数
void Scan_Send(void);											//发送任务扫描函数
///////////使用时只需将这三个函数放入主循环中扫描


void N58_Restart_Check_Scan(void);					//该函数检测发送命令后无响应的时间，超时则重启，使用串口接收时，将该函数放至 1ms 扫描一次的函数里
void N58_USART_SCAN(void);									//该函数检测串口接收完成，使用串口接收时，将该函数放至 1ms 扫描一次的函数里
void N58_USART_IRQHandler(void);						//该函数为串口接收中断服务函数，使用串口接收时，用这个函数替换掉对应串口的中断服务函数里面的内容



#endif
