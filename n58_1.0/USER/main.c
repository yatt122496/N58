#include "mytasks.h"

 
/////////////////////////UCOSII�����ջ����///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//���������ջ�ռ�	
OS_STK START_TASK_STK[START_STK_SIZE];
//�������ӿ�
void start_task(void *pdata);
			
 int main(void)
 {	 
  
 
	delay_init();	    	 //��ʱ������ʼ��	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�   
	uart_init(115200);
	USART2_Init(115200);  //��ʼ������2������Ϊ115200
	LED_Init();		  		//��ʼ����LED���ӵ�Ӳ���ӿ�
	OSInit();  	 			//��ʼ��UCOSII		 			  
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	    
}

