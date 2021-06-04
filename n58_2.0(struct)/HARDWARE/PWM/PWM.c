#include "PWM.h"

#define 	TIM1_CCR1_Address    	0x40012C34
#define 	TIM3_CCR2_Address 		0x40000438
#define 	TIM3_CCR3_Address 		0x4000043C
#define 	TIM3_CCR4_Address 		0x40000440


u16 wRGBData[200];


/************************************************************************
	*Function: TIM3_Init
	*Description: 定时器3初始化开启DMA用于驱动RGB
	*Input: None
	*Output: None
	*Author: Fly
	*Date: 2020/06/12
	*Others:
*************************************************************************/
void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
  	GPIO_InitTypeDef GPIO_InitStructure;
  	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* GPIOA Configuration: TIM3 Channel  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 90-1; 		// 800kHz 
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* PWM1 Mode configuration */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
//	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
//	TIM_OC4Init(TIM3, &TIM_OCInitStructure);

//	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
//	TIM_OC3PreloadConfig(TIM3,TIM_OCPreload_Enable);
//	TIM_OC4PreloadConfig(TIM3,TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(TIM3,ENABLE);
/***************************************************************************/
	/* configure DMA */
	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* DMA1 Channel3 Config for PWM3 by TIM3_UP*/
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM3_CCR2_Address;	// 外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)wRGBData;				// 内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						// 从内存到外设
	DMA_InitStructure.DMA_BufferSize = 200;									// 数据长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// 外设地址不自动增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//内存地址自动增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// stop DMA feed after buffer size is reached
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	
	/* TIM3 DMA Request enable */
//	TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
//	TIM_DMACmd(TIM3, TIM_DMA_CC2, ENABLE);
//	TIM_DMACmd(TIM3, TIM_DMA_CC3, ENABLE);
//	TIM_DMACmd(TIM3, TIM_DMA_CC4, ENABLE);
	TIM_DMACmd(TIM3, TIM_DMA_Update, ENABLE);
}
void TIM1_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
   TIM_OCInitTypeDef TIM_OCInitStructure;
   DMA_InitTypeDef DMA_InitStructure;

   
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
   
   GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
   GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP; 
   GPIO_Init(GPIOA,&GPIO_InitStructure);
   
   TIM_TimeBaseInitStructure.TIM_Period = 90-1;
   TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
   
   TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;
   TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
   TIM_TimeBaseInitStructure.TIM_RepetitionCounter =0;
   TIM_TimeBaseInit(TIM1, & TIM_TimeBaseInitStructure);
   
   TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
   TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
   TIM_OCInitStructure.TIM_Pulse=0;
   TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;
   TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Reset;
   TIM_OC1Init(TIM1,&TIM_OCInitStructure);
    
   TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
   TIM_ARRPreloadConfig(TIM1,ENABLE);
   /***************************************************************************/
	/* configure DMA */
	/* DMA clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	   
	/* DMA1 Channel3 Config for PWM3 by TIM3_UP*/
	DMA_DeInit(DMA1_Channel2);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)TIM1_CCR1_Address; // 外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)wRGBData;			   // 内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;					   // 从内存到外设
	DMA_InitStructure.DMA_BufferSize = 200;								   // 数据长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 	   // 外设地址不自动增加
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				   //内存地址自动增加
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;						   // stop DMA feed after buffer size is reached
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	   
	/* TIM1 DMA Request enable */
	TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
//	TIM_DMACmd(TIM1, TIM_DMA_CC2, ENABLE);
//	TIM_DMACmd(TIM1, TIM_DMA_CC3, ENABLE);
//	TIM_DMACmd(TIM1, TIM_DMA_CC4, ENABLE);
//	TIM_DMACmd(TIM1, TIM_DMA_Update, ENABLE);

//   TIM_Cmd(TIM1,ENABLE);
   TIM_CtrlPWMOutputs(TIM1, ENABLE);
}


/************************************************************************
	*Function: sRGBSend
	*Description: TIM的DMA发送数据
	*Input: *pColorBuff数据数组,wRGBNum数据长度
	*Output: None
	*Author: Fly
	*Date: 2020/06/15
	*Others:
*************************************************************************/
void sRGBSend(u8 *pColorBuff, u16 wRGBNum)
{
	u8 bTemp1,bTemp2,bTemp3;
	u16 wMemAddr;
	u16 wBufferSize;
	wBufferSize = (wRGBNum*24) + 40;	
	wMemAddr = 0;				
	bTemp2 = wRGBNum*3;
	for(bTemp1 = 0;bTemp1 < bTemp2;bTemp1++)
	{
		for(bTemp3 = 0;bTemp3 < 8;bTemp3++)
		{
			wRGBData[wMemAddr++] = ((pColorBuff[bTemp1]<<bTemp3)&0x0080)?cZERO_CODE1:cZERO_CODE0;
		}

	}
	while(wMemAddr < wBufferSize)
	{
		wRGBData[wMemAddr++] = 0;
	}
	DMA_SetCurrDataCounter(DMA1_Channel2, wBufferSize); 	
	TIM_SetCounter(TIM1, 0x0000);
	DMA_Cmd(DMA1_Channel2, ENABLE); 			
	TIM_Cmd(TIM1, ENABLE); 						
	while(!DMA_GetFlagStatus(DMA1_FLAG_TC2)) ; 	
	TIM_Cmd(TIM1, DISABLE); 	
	DMA_Cmd(DMA1_Channel2, DISABLE); 			
	DMA_ClearFlag(DMA1_FLAG_TC2); 					
}

