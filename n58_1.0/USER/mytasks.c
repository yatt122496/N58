#include "mytasks.h"

//LED0闪烁
//设置任务优先级
#define LED0_TASK_PRIO       			9 
//设置任务堆栈大小
#define LED0_STK_SIZE  		    		64
//任务堆栈	
OS_STK LED0_TASK_STK[LED0_STK_SIZE];
//任务函数
void led0_task(void *pdata);

//LED1闪烁
//设置任务优先级
#define LED1_TASK_PRIO       			8 
//设置任务堆栈大小
#define LED1_STK_SIZE  		    		64
//任务堆栈	
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//任务函数
void led1_task(void *pdata);

//循环打印
//设置任务优先级
#define PRINT_TASK_PRIO       			6 
//设置任务堆栈大小
#define PRINT_STK_SIZE  		    		128
//任务堆栈	
OS_STK PRINT_TASK_STK[PRINT_STK_SIZE];
//任务函数
void print_task(void *pdata);

OS_EVENT *Mutex_send;		//蜂鸣器信号量指针	
u8 err;
u8 WebTemp[1024];

//开始任务
void start_task(void *pdata)
{
  OS_CPU_SR cpu_sr=0;
	pdata = pdata; 		  		 
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)
	
	OSTaskCreate(led0_task,(void *)0,
								(OS_STK *)&LED0_TASK_STK[LED0_STK_SIZE-1],
								LED0_TASK_PRIO );//创建任务
	OSTaskCreate(led1_task,(void *)0,
								(OS_STK *)&LED1_TASK_STK[LED1_STK_SIZE-1],
								LED1_TASK_PRIO );//创建起始任务
	OSTaskCreate(print_task,(void *)0,
								(OS_STK *)&PRINT_TASK_STK[PRINT_STK_SIZE-1],
								PRINT_TASK_PRIO );//创建循环打印
								
	Mutex_send = OSMutexCreate(OSPrioHighRdy - 1,&err);
	
 	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

//LED0任务
void led0_task(void *pdata)
{
	for(;;)
	{
		LED0=~LED0;
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//LED1任务
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

//循环打印任务
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
			u2_printf("%s\r\n",USART_RX_BUF);//插入换行
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
			memset((char*)USART_RX_BUF,0x00,len);//串口接收缓冲区清0
		}
		OSTimeDlyHMSM(0,0,0,10);
	}
}

