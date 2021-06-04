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

OS_EVENT *Mutex_send;		//�������ź���ָ��	
u8 err;
u8 WebTemp[1024];

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
	u8 err;
	for(;;)
	{
		OSMutexPend(Mutex_send,0,&err);
		if(USART2_RX_STA&0X8000)
		{
			USART2_RX_BUF[USART2_RX_STA&0X7fff]=0;
			printf("%s\r\n",USART2_RX_BUF);
			USART2_RX_STA=0;
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
	u8 *data;
	for(;;)
	{
		if(USART_RX_STA&0x8000)
		{
			len=USART_RX_STA&0x3fff;
			USART_RX_BUF[len]=0;
			u2_printf("%s\r\n",USART_RX_BUF);//���뻻��
			USART_RX_STA=0;
			if(strcmp(USART_RX_BUF,"K0")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				n58_init();
				OSMutexPost(Mutex_send);
			}
			else if(strcmp(USART_RX_BUF,"K1")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				n58_ppp_init();
				OSMutexPost(Mutex_send);
			}
			else if(strcmp(USART_RX_BUF,"K2")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				data=n58_http_get("www.baidu.com","80","0","1024");
				if(data)
					printf("\r\n%s\r\n",WebTemp);
				OSMutexPost(Mutex_send);
			}
			else if(strcmp(USART_RX_BUF,"K3")==0)
			{
				OSMutexPend(Mutex_send,0,&err);
				data=n58_http_head("www.baidu.com","80");
				if(data)
				{
					printf("\r\n%s\r\n",WebTemp);
				}
				OSMutexPost(Mutex_send);
			}
			memset((char*)USART_RX_BUF,0x00,len);//���ڽ��ջ�������0
		}
		OSTimeDlyHMSM(0,0,0,10);
	}
}

