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

#define N58_USART(fmt, ...)	 USART_printf (USART2, fmt, ##__VA_ARGS__)	//��N58���ڷ�������
#define PC_USART(fmt, ...)	 printf (fmt, ##__VA_ARGS__)								//��ʾN58���ص�����

#define RX_BUF_MAX_LEN 2048		  //�����ջ����ֽ���
#define TX_LIST_MAX_LEN	20			//���������б�����������

extern struct STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
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

extern struct STRUCT_USART_Send	  //����һ��ȫ�ִ��ڷ��͵Ĵ���ṹ��
{
	u8 bTask_Cmd_List[TX_LIST_MAX_LEN];			//�������������
	u8 bTask_Ack_List[TX_LIST_MAX_LEN];			//Ӧ����������
	u8 bResend_Count,bData_Mode;						//ʧ���ط������������ݷ���ģʽ��־
	union
	{
		__IO u8 bInfAll;
    struct 
		{
		  __IO u8 bSending_Flag   		:1;     // 0		���ڷ��ͱ�־
	  	__IO u8 bSend_Finish				:1; 		// 1		������ɱ�־
		  __IO u8 bFinish_state				:1;			// 2		�������״̬��־��0Ϊ����ʧ��
			__IO u8 bRestart_Check			:1;			// 3		��������־���������ʼ�������
			__IO u8 bResend_Flag				:1;			// 4		���·��ͱ�־
			__IO u8 bResponse_Flag			:3;			// 5-6	Ӧ�����0Ϊ����ҪӦ��
																					//							 	1Ϊ��ͨӦ��
																					//								����1Ϊ����Ӧ��
	  }bInfBit;
  }; 
}N58_Send_Struct;

extern volatile u8 bRestart_flag;					//������־
extern u8 bN58_AT_Cmd_List[][80];					//N58 ATָ���
extern u8 bN58_AT_Ack_List[][20];					//N58 ATָ��Ӧ���
extern u8 bN58_Data_Post[80];							//N58 �������ݻ���			
extern u8 bN58_Data[1024];

void Send_Cmd(void);											//���������
void Scan_N58_Usart(void);								//N58���ڽ���ɨ�躯��
void Scan_Restart(void);									//N58����ɨ�躯��
void Scan_Send(void);											//��������ɨ�躯��

u8 *bN58_Check_Data(u8 *str);								//�����ַ�������
void N58_Send_AT_Cmd(u8 *bCmd,u8 *bAck);		//����ATָ���

void N58_Init(void);												//N58ģ��ͨѶ��ʼ��
void N58_Ppp_Link(void);										//N58 PPP ���Ӻ���
void N58_Http_Get(u8 *url,u8 *port,u8 *offset,u8 *size);		//N58 http get ����ʽ
void N58_Http_Head(u8 *url,u8 *port);												//N58 http head ����ʽ
void N58_Http_Post(u8 *bUrl,u8 *bPort,u8 bType,u8 *bCode);	//N58 http post ��������


void USART_printf( USART_TypeDef * USARTx, char * Data, ... );		//printf����
void N58_Usart_Init(u32 bound);												//N58���ڳ�ʼ�� 
void TIM4_Set(u8 sta);																//N58����ʹ�õĶ�ʱ��TIM4
void TIM4_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);									//N58�������ʹ�õĶ�ʱ��


#endif
