#include "n58.h"


struct STRUCT_USART_Fram N58_Fram_Record_Struct = { 0 };		//N58串口接收结构体

volatile u8 restart_flag=0;

//向4g模块发送AT指令
//cmd					：发送的指令
//acd1				：期待的应答结果，如果为 NULL，则表示不需要等待应答
//acd2				：期待的应答结果，如果为 NULL，则表示不需要等待应答
//time				：等待时间（单位：10ms)
//send_count	：失败重发次数
//返回值	：0，发送成功（得到了期待的应答结果）
//					1，发送失败
u8 N58_Send_AT_Cmd(u8 *cmd,u8 *ack1,u8 *ack2,u8 time,u8 send_count)
{
	u8 count;
	if(send_count<=0)
		return 1;
	while(send_count--)
	{
		N58_Fram_Record_Struct.InfAll = 0;	//从新开始接收新的数据包
		N58_USART("%s\r\n",cmd);
		PC_USART("%s\r\n",cmd);
		if(( ack1 || ack2 ) && time)
		{
			count=time;
			while(--count)
			{
				OSTimeDlyHMSM(0,0,0,10);
				if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
				{
					if(ack1!=0&&ack2!=0)
					{
						if(N58_Check_Data(ack1)||N58_Check_Data(ack2)) return 0;
					}
					else if( ack1 != 0 )
					{
						if(N58_Check_Data(ack1)) return 0;
					}
					else
						if(N58_Check_Data(ack2)) return 0;
					N58_Fram_Record_Struct.InfAll = 0;
					break;
				}
			}
		}
	}
	return 1;
}

//查找接收缓冲区字符串
//str				:需要查找的字符串
//返回值		：  		0，没有找到该字符串
//						 其他值，返回该字符串所在位置的指针
u8 *N58_Check_Data(u8 *str)
{
	char *strx=0;
	if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
	{
		N58_Fram_Record_Struct.Data_RX_BUF[N58_Fram_Record_Struct.InfBit.FramLength ] = '\0';
		if((strcmp((const char *)N58_Fram_Record_Struct.Data_RX_BUF,"\r\n")==0)||(strcmp((const char *)N58_Fram_Record_Struct.Data_RX_BUF,"")==0))
			return 0;
		PC_USART("%s\r\n", N58_Fram_Record_Struct.Data_RX_BUF);
		strx=strstr((const char*)N58_Fram_Record_Struct.Data_RX_BUF,(const char *)str);
	}
	return (u8 *)strx;
}

//向4g模块发送数据
//cmd					：发送的数据（不需要添加回车了）
//acd1				：期待的应答结果，如果为 NULL，则表示不需要等待应答
//acd2				：期待的应答结果，如果为 NULL，则表示不需要等待应答
//time				：等待时间（单位：10ms)
//send_count	：失败重发次数
//返回值	：0，发送成功（得到了期待的应答结果）
//					1，发送失败
u8 N58_Send_Data(u8 *cmd,u8 *ack1,u8 *ack2,u8 time,u8 send_count)
{
	u8 count;
	if(send_count<=0)
		return 1;
	while(send_count--)
	{
		N58_Fram_Record_Struct.InfAll = 0;	//从新开始接收新的数据包
		N58_USART("%s",cmd);
		PC_USART("%s",cmd);
		if(( ack1 || ack2 ) && time)
		{
			count=time;
			while(--count)
			{
				OSTimeDlyHMSM(0,0,0,10);
				if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
				{
					if(ack1!=0&&ack2!=0)
					{
						if(N58_Check_Data(ack1)||N58_Check_Data(ack2)) return 0;
					}
					else if( ack1 != 0 )
					{
						if(N58_Check_Data(ack1)) return 0;
					}
					else
						if(N58_Check_Data(ack2)) return 0;
					N58_Fram_Record_Struct.InfAll = 0;
					break;
				}
			}
		}
	}
	return 1;
}

//n58初始化程序
//返回值		：	0，初始化成功
//						1，初始化失败
u8 N58_Init(void)
{
	u8 i,num_csq=0;
	OSTimeDlyHMSM(0,0,10,0);
	if(N58_Send_AT_Cmd("AT","OK",NULL,100,5))							//串口接通测试，若返回"OK"，则进行下一步，反之继续拨号
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("ATE0","OK",NULL,100,3))							//设置发送不回显，若返回"OK"，则进行下一步，反之继续拨号
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CGSN","OK",NULL,100,3))							//模块IMEI，若返回"OK"，则进行下一步，反之继续拨号
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CGMR","OK",NULL,100,3))							//模块固件版本，若返回"OK"，则进行下一步，反之继续拨号
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CPIN?","READY",NULL,100,5))							//SIM卡状态，若返回"READY"，则进行下一步，反之继续拨号
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CIMI","OK",NULL,100,2))							//SIM卡IMSI，若返回"OK"，则进行下一步，反之继续拨号
	{
		return 1;
	}
	
	i=5;
	while(i--)							//信号强度"若返回+CSQ: <signal>,<ber>,且0<signal<=31时进行下一步
	{
		N58_Send_AT_Cmd("AT+CSQ","+CSQ:",NULL,100,3);
		if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
		{
			num_csq=atoi(strstr(strstr((const char*)N58_Fram_Record_Struct.Data_RX_BUF,"+CSQ:"),":")+1);
			N58_Fram_Record_Struct.InfAll = 0;
			if(num_csq>0&&num_csq<=31) break;
		}
	}
	if(i<=0) return 1;
	
	if(N58_Send_AT_Cmd("AT+CREG=2","OK",NULL,100,3))							//主动上报网络注册小区信息，若返回"OK"，则进行下一步，反之继续拨号
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CREG?","+CREG: 2,1","+CREG: 2,5",100,5))							//查询网络注册情况,若返回"+CREG: 2,1"或者"+CREG: 2,5"字段，则进行下一步，反之继续拨号
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CREG=0","OK",NULL,100,3))							//主动上报网络注册小区信息，若返回"OK"，则进行下一步，反之继续拨号
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CGATT?","+CGATT: 1",NULL,100,5))							//主动上报网络注册小区信息，若返回"OK"，则进行下一步，反之继续拨号
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",NULL,100,3))					//设置APN，若返回"OK"，则进行下一步，反之继续拨号
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+XGAUTH=1,1,\"gsm\",\"1234\"","OK",NULL,100,3))					//设置身份认证参数，若返回"OK"，则进行下一步，反之继续拨号
	{
		restart_flag=1;
		return 1;
	}
	
	N58_Fram_Record_Struct.InfAll=0;
	return 0;
}

//ppp基础通信条件检测
//返回值		：	0，检测成功
//						1，检测失败
u8 N58_Ppp_Init(void)
{
	if(N58_Send_AT_Cmd("AT+XIIC=1","OK",NULL,100,3))							//进行PPP拨号，若返回"OK"，则进行下一步，反之继续拨号，1s/次，最多3次，3次还不行则重启
	{	
		restart_flag=1;
		return 1;
	}
	if(N58_Send_AT_Cmd("AT+XIIC?","+XIIC:    1",NULL,100,3))				//查询PPP连接建立情况，若回码包含"+XIIC:	1",则进行下一步，反之循环查询，1s/次，最多30次。
	{
		restart_flag=1;
		return 1;
	}
	N58_Fram_Record_Struct.InfAll=0;
	return 0;
}

///HTTP GET 请求方法
//url			：目标网址
//port		：目标端口
//offset	：分段下载时的偏移地址
//size		：分段下载时每段下载大小
//	不使用分段下载时，offset ,size 都设置为 NULL 
//返回值		：1，请求成功
//					0，发送失败
//	注意接收的内容不要超过接收缓冲区大小
u8 *N58_Http_Get(u8 *url,u8 *port,u8 *offset,u8 *size)
{
	u8 cmd[40]={0},segmentDownload;
	u8 time=100,*data=0;
	u32 num_offset,num_size,num_length=0;
	sprintf((char *)cmd,"AT+HTTPPARA=url,%s",url);			//设置域名
	if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))
	{
		return NULL;
	}
	sprintf((char *)cmd,"AT+HTTPPARA=port,%s",port);		//设置端口号
	if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))
	{
		return NULL;
	}
	if(N58_Send_AT_Cmd("AT+HTTPSETUP","OK",NULL,200,3))			//建立HTTP连接
	{
		restart_flag=1;
		return NULL;
	}
	if(offset&&size)
		segmentDownload=1;		
	else
		segmentDownload=0;
	if(!segmentDownload)																//不分段请求HTTP
	{
		sprintf((char *)cmd,"AT+HTTPACTION=0");
		if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))		//执行http get 请求
		{
			restart_flag=1;
			return NULL;
		}
		N58_Fram_Record_Struct.InfAll = 0;
		while(--time)
		{
			OSTimeDlyHMSM(0,0,0,10);
			if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
			{
				data=N58_Check_Data("+HTTPRECV:");
				N58_Fram_Record_Struct.InfAll = 0;
				break;
			}
		}
	}
	else
	{
		num_offset=(u32)atoi((const char *)offset);						//字符串转为数字
		num_size=(u32)atoi((const char *)size);
		do																										//分段请求HTTP
		{
			if(N58_Send_AT_Cmd("AT+HTTPSETUP","OK",NULL,200,3))			//建立HTTP连接
			{
				restart_flag=1;
				return NULL;
			}
			sprintf((char *)cmd,"AT+HTTPACTION=0,%d,%d",num_offset,num_size);
			if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))		//执行http get 请求
			{
				restart_flag=1;
				return NULL;
			}
			N58_Fram_Record_Struct.InfAll = 0;
			OSTimeDlyHMSM(0,0,0,50);
			while(--time)
			{
				OSTimeDlyHMSM(0,0,0,10);
				data=0;
				if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
				{
					data=N58_Check_Data("+HTTPRECV:");
					if(data)
						num_offset+=num_size;
					if(!num_length)
						num_length=atoi(strstr(strstr((const char*)N58_Fram_Record_Struct.Data_RX_BUF,"Content-Range:"),"/")+1);
					N58_Fram_Record_Struct.InfAll = 0;
					break;
				}
			}
		}while((num_offset)<(num_length-1));
	}
	return data;
}

//HTTP HEAD 请求方法
//url				：目标网址
//port			：目标端口
//返回值		：1，请求成功
//						0，发送失败
u8 *N58_Http_Head(u8 *url,u8 *port)
{
	u8 cmd[40]={0};
	u8 time=100,*data=0;
	sprintf((char *)cmd,"AT+HTTPPARA=url,%s",url);			//设置域名
	if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))
	{
		return NULL;
	}
	sprintf((char *)cmd,"AT+HTTPPARA=port,%s",port);		//设置端口号
	if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))
	{
		return NULL;
	}
	if(N58_Send_AT_Cmd((u8 *)"AT+HTTPSETUP",(u8 *)"OK",NULL,200,3))			//建立HTTP连接
	{
		restart_flag=1;
		return NULL;
	}
	if(N58_Send_AT_Cmd((u8 *)"AT+HTTPACTION=1",(u8 *)"OK",NULL,100,3))				//执行http get 请求
	{
		restart_flag=1;
		return NULL;
	}
	N58_Fram_Record_Struct.InfAll = 0;
	while(--time)
	{
		OSTimeDlyHMSM(0,0,0,10);
		if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
		{
			data=N58_Check_Data("+HTTPRECV:");
			N58_Fram_Record_Struct.InfAll = 0;
			break;
		}
	}
	return data;
}


//N58 语音播报设置
//volume			：音量大小(0-6)
//speed				：声音速度(0-6)
//pitch				：音色(0-6)
//format			：播报内容编码格式(0-1),0:gbk编码；1:UTF-16编码
//返回值			：0，设置成功
//							1，设置失败
u8 N58_Tts_Set(u8 volume,u8 speed,u8 pitch,u8 format)
{
	u8 cmd[40]={0},cfg_flag=0,fmt_flag=0,res;
	sprintf((char *)cmd,"AT+TTSCFG=%d,%d,%d",volume,speed,pitch);			//语音播报配置
	cfg_flag=!N58_Send_AT_Cmd(cmd,"OK",NULL,100,3);						//进行语音播报配置，若返回"OK"，则进行下一步，反之继续拨号，1s/次，最多3次，3次还不行则重启
	
	sprintf((char *)cmd,"AT+TTSFMT=%d",format);			//播报内容编码格式
	fmt_flag=!N58_Send_AT_Cmd(cmd,"OK",NULL,100,3);
	if(cfg_flag&&fmt_flag)
		res=0;
	else if(cfg_flag)
	{
		sprintf((char *)cmd,"AT+TTSFMT=%d",format);			//播报内容编码格式
		if(N58_Send_AT_Cmd(cmd,"OK",NULL,100,3))
		{
			restart_flag=1;
			res=1;
		}
	}
	else if(fmt_flag)
	{
		sprintf((char *)cmd,"AT+TTSCFG=%d,%d,%d",volume,speed,pitch);			//语音播报配置
		if(N58_Send_AT_Cmd(cmd,"OK",NULL,100,3))
		{
			restart_flag=1;
			res=1;
		}
	}
	else
	{
		restart_flag=1;
		res=1;
	}
	N58_Fram_Record_Struct.InfAll = 0;
	return res;
}

//N58 语音播报
//size				：语音播报字节
//code				：语音内容编码
//返回值			：1，发送成功
//							0，发送失败
u8 *N58_Tts_Play(u8 size, u8 *code)
{
	u8 cmd[40]={0},time=100,*data=0,second=0;
	u16 msecond=0,num=0;
	sprintf((char *)cmd,"AT+TTSPLAY=%d",size);			//语音播报配置
	if(N58_Send_AT_Cmd(cmd,(u8 *)">",NULL,100,3))
	{
		restart_flag=1;
		return NULL;
	}
	if(N58_Send_AT_Cmd(code,(u8 *)"OK",NULL,100,3))
	{
		restart_flag=1;
		return NULL;
	}
	N58_Fram_Record_Struct.InfAll = 0;
	num=150*size;
	second=num/1000;
	msecond=num-second*1000;
	OSTimeDlyHMSM(0,0,second,msecond);
	while(--time)
	{
		OSTimeDlyHMSM(0,0,0,10);
		if(N58_Fram_Record_Struct.InfBit.FramFinishFlag)
		{
			data=N58_Check_Data("+TTSPLAY: OK");
			PC_USART("data=%x\r\n",data);
			N58_Fram_Record_Struct.InfAll = 0;
			break;
		}
	}
	return data;
}


/////////////////////////////// USART2 串口初始化 //////////////////////////////////////////////

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
void USART2_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//接收到数据
	{
		res =USART_ReceiveData(USART2);		
		if(N58_Fram_Record_Struct.InfBit.FramLength < RX_BUF_MAX_LEN - 1 )		//还可以接收数据
		{
			TIM_SetCounter(TIM4,0);//计数器清空
			if(N58_Fram_Record_Struct.InfAll==0)TIM4_Set(1);	 	//使能定时器4的中断
			N58_Fram_Record_Struct.Data_RX_BUF[N58_Fram_Record_Struct.InfBit.FramLength++]=res;		//记录接收到的值	 
		}else 
		{
			N58_Fram_Record_Struct.InfBit.FramFinishFlag=1;					//强制标记接收完成
		}
	}
}

//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率
void USART2_Init(u32 bound)
{

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

 	USART_DeInit(USART2);  //复位串口2
		 //USART2_TX   PA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2

    //USART2_RX	  PA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3

	USART_InitStructure.USART_BaudRate = bound;//一般设置为115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口2

	USART_Cmd(USART2, ENABLE);                    //使能串口

	//使能接收中断
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	TIM4_Init(999,7199);		//10ms中断
	N58_Fram_Record_Struct.InfAll=0;		//清零
	TIM4_Set(0);			//关闭定时器4
}

//定时器4中断服务程序		    
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		N58_Fram_Record_Struct.InfBit.FramFinishFlag = 1;	//标记接收完成
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志   
		TIM4_Set(0);			//关闭TIM4  
	}	    
}
//设置TIM4的开关
//sta:0，关闭;1,开启;
void TIM4_Set(u8 sta)
{
	if(sta)
	{
		TIM_SetCounter(TIM4,0);//计数器清空
		TIM_Cmd(TIM4, ENABLE);  //使能TIMx	
	}else TIM_Cmd(TIM4, DISABLE);//关闭定时器4	   
}
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM4_Init(u16 arr,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能//TIM4时钟使能    
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}



/////////////////////////////// USART2 串口发送 //////////////////////////////////////////////
static char *itoa( int value, char *string, int radix )
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;

	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;
		
	}

	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;

		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;

} /* NCL_Itoa */


void USART_printf ( USART_TypeDef * USARTx, char * Data, ... )
{
	const char *s;
	int d;   
	char buf[16];

	va_list ap;
	va_start(ap, Data);

	while ( * Data != 0 )     // 判断是否到达字符串结束符
	{
		if ( * Data == 0x5c )  //'\'
		{
			switch ( *++Data )
			{
				case 'r':							          //回车符
				USART_SendData(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //换行符
				USART_SendData(USARTx, 0x0a);	
				Data ++;
				break;

				default:
				Data ++;
				break;
			}
		}
		else if ( * Data == '%')
		{
			switch ( *++Data )
			{
				case 's':										  //字符串
				s = va_arg(ap, const char *);
				for ( ; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;
				case 'd':
					//十进制
				d = va_arg(ap, int);
				itoa(d, buf, 10);
				for (s = buf; *s; s++)
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;
				default:
				Data++;
				break;
			}
		}
		
		else USART_SendData(USARTx, *Data++);
		
		while ( USART_GetFlagStatus ( USARTx, USART_FLAG_TXE ) == RESET );
		
	}
}


