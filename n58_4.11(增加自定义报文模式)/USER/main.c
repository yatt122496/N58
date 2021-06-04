#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "n58.h"
#include "timer.h"

	
int main(void)
{
	u16 wLen;
	
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
	Usart2_Init(115200);
	TIM4_Init(9,7199);		//1ms中断
 	LED_Init();			     //LED端口初始化
	
  while(1)
	{
		Scan_Send();
		Scan_Restart();
		Scan_N58_Usart();
		
		if(USART_RX_STA&0x8000)
		{
			wLen=USART_RX_STA&0x3fff;
			USART_RX_BUF[wLen]=0;
			N58_printf("%s\r\n",USART_RX_BUF);//插入换行
			USART_RX_STA=0;
			if(strcmp((const char *)USART_RX_BUF,"K0")==0)
			{
				N58_Init();
			}
			else if(strcmp((const char *)USART_RX_BUF,"K1")==0)
			{
				N58_Ppp_Link();
			}
			else if(strcmp((const char *)USART_RX_BUF,"K2")==0)
			{
				N58_Http_Set("v1-tml.lulucai.cn","18060");
			}
			else if(strcmp((const char *)USART_RX_BUF,"K3")==0)
			{
				N58_Http_Get((u8 *)"0",(u8 *)"512");
			}
			else if(strcmp((const char *)USART_RX_BUF,"K4")==0)
			{
				N58_Http_Head();
			}
			else if(strcmp((const char *)USART_RX_BUF,"K5")==0)
			{
				N58_Http_Post(1,(u8 *)"12",2);
			}
			memset((char*)USART_RX_BUF,0x00,wLen);//串口接收缓冲区清0
		}
		LED0=!LED0;
	}
}


