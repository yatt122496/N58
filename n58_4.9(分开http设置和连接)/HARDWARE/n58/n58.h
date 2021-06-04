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


#define cRX_BUF_MAX_LEN 1024		  //�����ջ����ֽ���
#define cTX_LIST_MAX_LEN	20			//���������б�����������

#define cN58_USART USART2				//N58 ͨѶ���ڣ���������Ϊ115200

extern struct STRUCT_USART_Fram	  //����һ��ȫ�ִ�������֡�Ĵ���ṹ��
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

extern struct STRUCT_USART_Send	  //����һ��ȫ�ִ��ڷ��͵Ĵ���ṹ��
{
	u8 bTask_Cmd_List[cTX_LIST_MAX_LEN];			//�������������
	u8 bTask_Ack_List[cTX_LIST_MAX_LEN];			//Ӧ����������
	u8 bResend_Count,bData_Mode,bNow_Task;						//ʧ���ط������������ݷ���ģʽ��־
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

extern u8 bRestart_flag;					//������־
extern u8 bN58_AT_Cmd_List[][80];					//N58 ATָ���
extern u8 bN58_AT_Ack_List[][40];					//N58 ATָ��Ӧ���
extern u8 bN58_Data_Post[80];							//N58 �������ݻ���			
extern u8 bN58_Data[1024];
extern u16 bRestart_Check_CNT;

void Send_Cmd(void);											//���������

u8 *bN58_Check_Data(u8 *str);								//�����ַ�������
void N58_Send_AT_Cmd(u8 *bCmd,u8 *bAck);		//����ATָ���

void N58_printf(char* fmt,...);							//printf����
void N58_Usart_Send(u8 *bData,u8 bLen);			//N58�����ֽں���

void N58_Init(void);												//N58ģ��ͨѶ��ʼ��
void N58_Ppp_Link(void);										//N58 PPP ���Ӻ���
void N58_Http_Set(u8 *bUrl,u8 *bPort);
void N58_Http_Get(u8 *offset,u8 *size);					//N58 http get ����ʽ
void N58_Http_Head(void);												//N58 http head ����ʽ
void N58_Http_Post(u8 bType,u8 *bCode,u8 bLen);	//N58 http post ��������
//void N58_Tcp_Link(u8 *bUrl,u8 *bPort,u8 *bCode);





//////////////////////////��Ҫʹ�õ��ĺ���
void Scan_N58_Usart(void);								//N58���ڽ���ɨ�躯��
void Scan_Restart(void);									//N58����ɨ�躯��
void Scan_Send(void);											//��������ɨ�躯��
///////////ʹ��ʱֻ�轫����������������ѭ����ɨ��


void N58_Restart_Check_Scan(void);					//�ú�����ⷢ�����������Ӧ��ʱ�䣬��ʱ��������ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����
void N58_USART_SCAN(void);									//�ú�����⴮�ڽ�����ɣ�ʹ�ô��ڽ���ʱ�����ú������� 1ms ɨ��һ�εĺ�����
void N58_USART_IRQHandler(void);						//�ú���Ϊ���ڽ����жϷ�������ʹ�ô��ڽ���ʱ������������滻����Ӧ���ڵ��жϷ��������������



#endif
