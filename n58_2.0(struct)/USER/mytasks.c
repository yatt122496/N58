#include "mytasks.h"

//LED0��˸
//�����������ȼ�
#define LED0_TASK_PRIO       			9 
//���������ջ��С
#define LED0_STK_SIZE  		    		64
//�����ջ	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//������
void led0_task(void *pdata);

//LED1��˸
//�����������ȼ�
#define LED1_TASK_PRIO       			8 
//���������ջ��С
#define LED1_STK_SIZE  		    		64
//�����ջ	
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *pdata);

//ѭ����ӡ
//�����������ȼ�
#define PRINT_TASK_PRIO       			6 
//���������ջ��С
#define PRINT_STK_SIZE  		    		128
//�����ջ	
OS_STK PRINT_TASK_STK[PRINT_STK_SIZE];
//������
void print_task(void *pdata);

OS_EVENT *Mutex_send;		//���մ��ڻ����ź���ָ��
u8 err;

const u8 *voice_code_gbk[]={
	"BBB6D3ADB3CBD7F8D6D0B9FACCE5D3FDB2CAC6B1CFFACADBB3B5A3ACCEA2D0C5C9A8C3E8B2CAC6B1BBFAC9CFB5C4B6FECEACC2EBBCB4BFC9B9BAC2F2D6D0B9FACCE5D3FDB2CAC6B1",
	"C9A8C2EBBFC9D2D4B8F8CAD6BBFAD6C7C4DCBFECCBD9B3E4B5E7",
	"C7EBC4FAC9A8C3E8B1BEB2CAC6B1BBFAB6FECEACC2EBBDF8D0D0D6C7C4DCBFECCBD9B3E4B5E7",
	"C4FABAC3A3BAC4FAC3E6C7B0B5C4CAC7D6D0B9FACCE5D3FDB2CAC6B1D7D4D6FACFFACADBBBFAA3ACCFD6D4DAC4FABFC9D2D4D1A1B9BAD6D0B9FACCE5D3FDB2CAC6B1A3ACB9BAC2F2CDEAB3C9BAF3C1A2BCB4BFAABDB1",
	"BADCB1A7C7B8A3ACB2CAC6B1D2D1CADBCDEA",
	"C4FAD2D1D6A7B8B6CDEAB3C9A3ACC7EBC4CDD0C4B5C8B4FD",
	"CDF8C2E7C1B4BDD3B3ACCAB1A3ACD2D1CDCBBFEEA3ACC4FABFC9D4DA31B7D6D6D3BAF3D4D9B4CEB3A2CAD4A1A3C8E7D3D0CECACCE2C7EBD4DAD0A1B3CCD0F2D2B3C3E6C1AACFB5CADBBAF3B7FECEF1",
	"C7EBBDABBAECC9ABCBAEBEA7C0ADB8CBCFF2B3B5CEB2B7BDCFF2D3C3C1A6C0ADB3F6",
	
};
const u8 *voice_chinese[]={
	"��ӭ�����й�������Ʊ���۳���΢��ɨ���Ʊ���ϵĶ�ά�뼴�ɹ����й�������Ʊ",
	"ɨ����Ը��ֻ����ܿ��ٳ��",
	"����ɨ�豾��Ʊ����ά��������ܿ��ٳ��",
	"���ã�����ǰ�����й�������Ʊ���������ۻ�������������ѡ���й�������Ʊ��������ɺ���������",
	"�ܱ�Ǹ����Ʊ������",
	"����֧����ɣ������ĵȴ�",
	"�������ӳ�ʱ�����˿������1���Ӻ��ٴγ��ԡ�������������С����ҳ����ϵ�ۺ����",
	"�뽫��ɫˮ��������β������������",
	
};

//��ʼ����
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)
	
	OSTaskCreate(led0_task,(void *)0,
								(OS_STK *)&LED0_TASK_STK[LED0_STK_SIZE-1],
								LED0_TASK_PRIO );//��������
	OSTaskCreate(led1_task,(void *)0,
								(OS_STK *)&LED1_TASK_STK[LED1_STK_SIZE-1],
								LED1_TASK_PRIO );//������ʼ����
	OSTaskCreate(print_task,(void *)0,
								(OS_STK *)&PRINT_TASK_STK[PRINT_STK_SIZE-1],
								PRINT_TASK_PRIO );//����ѭ����ӡ
								
	Mutex_send = OSMutexCreate(OSPrioHighRdy - 1,&err);
	
 	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//LED0����
void led0_task(void *pdata)
{
	for(;;)
	{
		LED0=~LED0;
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//LED1����
void led1_task(void *pdata)
{
	for(;;)
	{
		OSMutexPend(Mutex_send,0,&err);
		if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
		{
			N58_Fram_Record_Struct.Data_RX_BUF[N58_Fram_Record_Struct.InfBit.FramLength ] = '\0';
			PC_USART("%s\r\n", N58_Fram_Record_Struct .Data_RX_BUF);
			N58_Fram_Record_Struct.InfAll = 0;	//���¿�ʼ�����µ����ݰ�
		}
		LED1=~LED1;
		OSMutexPost(Mutex_send);
		OSTimeDlyHMSM(0,0,0,10);
	}
}

//ѭ����ӡ����
void print_task(void * pdata)
{
	u16 len;
	u8 i=0;
	for(;;)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0x3fff;
			USART_RX_BUF[len]=0;
			N58_USART("%s\r\n",USART_RX_BUF);//���뻻��
			USART_RX_STA=0;
			if(strcmp((const char *)USART_RX_BUF,"K0")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				N58_Init();
				OSMutexPost(Mutex_send);
			}
			else if(strcmp((const char *)USART_RX_BUF,"K1")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				N58_Ppp_Init();
				OSMutexPost(Mutex_send);
			}
			else if(strcmp((const char *)USART_RX_BUF,"K2")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				N58_Http_Get((u8 *)"www.baidu.com",(u8 *)"80",(u8 *)"0",(u8 *)"512");
				OSMutexPost(Mutex_send);
			}
			else if(strcmp((const char *)USART_RX_BUF,"K3")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				N58_Http_Head((u8 *)"www.baidu.com",(u8 *)"80");
				OSMutexPost(Mutex_send);
			}
			else if(strcmp((const char *)USART_RX_BUF,"K4")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				N58_Tts_Set(3,3,3,0);
				OSMutexPost(Mutex_send);
			}
			else if(strcmp((const char *)USART_RX_BUF,"K5")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				PC_USART("���������������£�\r\n%s\r\n",voice_chinese[i]);
				N58_Tts_Play( strlen((const char *)voice_code_gbk[i] ) / 2 , (u8 *)voice_code_gbk[i] );
				i++;
				if( i > ( sizeof(voice_code_gbk)/sizeof(voice_code_gbk[0]) - 1 ) )
					i=0;
				OSMutexPost(Mutex_send);
			}
			memset((char*)USART_RX_BUF,0x00,len);//���ڽ��ջ�������0
		}
		OSTimeDlyHMSM(0,0,0,10);
	}
}

