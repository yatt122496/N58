#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"
#include "n58.h"


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
void USART2_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{
		res =USART_ReceiveData(USART2);		
		if(N58_Fram_Record_Struct.InfBit.FramLength < RX_BUF_MAX_LEN - 1 )		//�����Խ�������
		{
			TIM_SetCounter(TIM4,0);//���������        				 
			if(N58_Fram_Record_Struct.InfAll==0)TIM4_Set(1);	 	//ʹ�ܶ�ʱ��4���ж� 
			N58_Fram_Record_Struct.Data_RX_BUF[N58_Fram_Record_Struct.InfBit.FramLength++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			N58_Fram_Record_Struct.InfBit.FramFinishFlag=1;					//ǿ�Ʊ�ǽ������
		}
	}
}

//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART2_Init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

 	USART_DeInit(USART2);  //��λ����2
		 //USART2_TX   PA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2
   
    //USART2_RX	  PA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA3
	
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(USART2, &USART_InitStructure); //��ʼ������	2

	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

	//ʹ�ܽ����ж�
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	TIM4_Init(999,7199);		//10ms�ж�
	N58_Fram_Record_Struct.InfAll=0;		//����
	TIM4_Set(0);			//�رն�ʱ��4 	

}

//��ʱ��4�жϷ������		    
void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		N58_Fram_Record_Struct.InfBit.FramFinishFlag = 1;	//��ǽ������
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־   
		TIM4_Set(0);			//�ر�TIM4  
	}	    
}
//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM4_Set(u8 sta)
{
	if(sta)
	{
       
		TIM_SetCounter(TIM4,0);//���������
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	
	}else TIM_Cmd(TIM4, DISABLE);//�رն�ʱ��4	   
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM4_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��//TIM4ʱ��ʹ��    
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}



