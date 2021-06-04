#ifndef _PWM_H_
#define _PWM_H_
#include "stm32f10x.h"


#define 	cZERO_CODE1  			50
#define 	cZERO_CODE0 			25


extern void TIM1_Init(void);
extern void TIM3_Init(void);
extern void sRGBSend(u8 *pColorBuff, u16 wRGBNum);
#endif

