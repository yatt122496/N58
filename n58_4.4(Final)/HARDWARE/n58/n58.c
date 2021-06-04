#include "n58.h"


struct STRUCT_USART_Fram N58_Fram_Record_Struct = { 0 };		//N58���ڽ��սṹ��
struct STRUCT_USART_Send N58_Send_Struct = { 0 };		//N58���ڷ��ͽṹ��

volatile u8 bRestart_flag=0;

//���巢�͵�����
u8 bN58_AT_Cmd_List[][80]={
	"",																		////  0
	"AT",																	////  1
	"ATE0",																////  2
	"AT+CGSN",														////  3
	"AT+CGMR",														////  4
	"AT+CPIN?",														////  5
	"AT+CIMI",														////  6
	"AT+CSQ",															////  7
	"AT+CREG=2",													////  8
	"AT+CREG?",														////  9
	"AT+CREG=0",													//// 10
	"AT+CGATT?",													//// 11
	"AT+CGDCONT=1,\"IP\",\"CMNET\"",			//// 12
	"AT+XGAUTH=1,1,\"gsm\",\"1234\"",			//// 13
	"AT+XIIC=1",													//// 14
	"AT+XIIC?",														//// 15
	"AT+HTTPPARA=url,www.baidu.com",			//// 16
	"AT+HTTPPARA=port,80",								//// 17
	"AT+HTTPSETUP",												//// 18
	"AT+HTTPACTION=0",										//// 19
	"AT+HTTPACTION=0,0,512",							//// 20
	"AT+HTTPACTION=1",										//// 21
	"AT+TTSCFG=3,3,3",										//// 22
	"AT+TTSFMT=0",												//// 23
	"AT+TTSPLAY=10",											//// 24
	"AT+HTTPACTION=2,2,1",								//// 25
};

//����N58ģ������Ӧ��
u8 bN58_AT_Ack_List[][20]={
	"",																		////  0
	"OK",																	////  1
	"READY",															////  2
	"+CSQ:",															////  3
	"+CREG: 2,",													////  4
	"+CGATT: 1",													////  5
	"+XIIC:    1",												////  6
	"+HTTPRECV:",													////  7
	">",																	////  8
	"+TTSPLAY: OK",												////  9
};

//����N58����ģʽ����
u8 *bN58_Data_List[10]={0};

//����N58��ַ
u8 bN58_Data_Post[80]={0};

u8 bN58_Data[1024]={0};

//����ö������
enum {
	CommandNull=0,
	Command1,
	Command2,
	Command3,
	Command4,
	Command5,
	Command6,
	Command7,
	Command8,
	Command9,
	Command10,
	Command11,
	Command12,
	Command13,
	Command14,
	Command15,
	Command16,
	Command17,
	Command18,
	Command19,
	Command20,
	Command21,
	Command22,
	Command23,
	Command24,
	Command25,
};
	
//��4gģ�鷢��ATָ��
//cmd					�����͵�ָ��
//acd1				���ڴ���Ӧ���������Ϊ NULL�����ʾ����Ҫ�ȴ�Ӧ��
//acd2				���ڴ���Ӧ���������Ϊ NULL�����ʾ����Ҫ�ȴ�Ӧ��
//time				���ȴ�ʱ�䣨��λ��10ms)
//send_count	��ʧ���ط�����
//����ֵ	��0�����ͳɹ����õ����ڴ���Ӧ������
//					1������ʧ��
void N58_Send_AT_Cmd(u8 *bCmd,u8 *bAck)
{
	N58_Fram_Record_Struct.wInfAll = 0;	//���¿�ʼ�����µ����ݰ�
	N58_USART("%s\r\n",bCmd);
	PC_USART("%s\r\n",bCmd);
}


//���ҽ��ջ������ַ���
//str				:��Ҫ���ҵ��ַ���
//����ֵ		��  		0��û���ҵ����ַ���
//						 ����ֵ�����ظ��ַ�������λ�õ�ָ��
u8 *bN58_Check_Data(u8 *bStr)
{
	char *bStrx=0;
	if((strcmp((const char *)N58_Fram_Record_Struct.bData_RX_BUF,"\r\n")==0)||(strcmp((const char *)N58_Fram_Record_Struct.bData_RX_BUF,"")==0))
		return 0;
	PC_USART("%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
	bStrx=strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bStr);
	return (u8 *)bStrx;
}


//n58��ʼ������
void N58_Init(void)
{
	u8 i;
	u8 bN58_Cmd_List[13+1]={Command1,Command2,Command3,Command4,Command5,Command6,
				Command7,Command8,Command9,Command10,Command11,Command12,Command13,CommandNull};
	u8 bN58_Ack_List[13+1]={Command1,Command1,Command1,Command1,Command2,Command1,
				Command3,Command1,Command4,Command1,Command5,Command1,Command1,CommandNull};
	for(i=0;i<5;i++)
	{
		N58_USART("AT\r\n");
		PC_USART("AT\r\n");
		delay_ms(1000);
	}
	for(i=0;i<sizeof(bN58_Cmd_List)/sizeof(bN58_Cmd_List[0]);i++)
	{
		N58_Send_Struct.bTask_Cmd_List[i]=bN58_Cmd_List[i];
		N58_Send_Struct.bTask_Ack_List[i]=bN58_Ack_List[i];
	}
	N58_Send_Struct.bResend_Count=3;
}

//ppp����ͨ���������
void N58_Ppp_Link(void)
{
	u8 i;
	u8 bN58_Cmd_List[2+1]={Command14,Command15,CommandNull};
	u8 bN58_Ack_List[2+1]={Command1,Command6,CommandNull};
	for(i=0;i<sizeof(bN58_Cmd_List)/sizeof(bN58_Cmd_List[0]);i++)
	{
		N58_Send_Struct.bTask_Cmd_List[i]=bN58_Cmd_List[i];
		N58_Send_Struct.bTask_Ack_List[i]=bN58_Ack_List[i];
	}
	N58_Send_Struct.bResend_Count=3;
}

///HTTP GET ���󷽷�
//url			��Ŀ����ַ
//port		��Ŀ��˿�
//offset	���ֶ�����ʱ��ƫ�Ƶ�ַ
//size		���ֶ�����ʱÿ�����ش�С
//	��ʹ�÷ֶ�����ʱ��offset ,size ������Ϊ NULL 
//	ע����յ����ݲ�Ҫ�������ջ�������С
void N58_Http_Get(u8 *bUrl,u8 *bPort,u8 *bOffset,u8 *bSize)
{
	u8 i;
	u8 bN58_Cmd_List[4+1]={Command16,Command17,Command18,Command19,CommandNull};
	u8 bN58_Ack_List[4+1]={Command1,Command1,Command1,Command1,CommandNull};
	if(bOffset&&bSize)
	{
		bN58_Cmd_List[3]=Command20;
		sprintf((char *)bN58_AT_Cmd_List[Command20],"AT+HTTPACTION=0,%s,%s",bOffset,bSize);
	}
	sprintf((char *)bN58_AT_Cmd_List[Command16],"AT+HTTPPARA=url,%s",bUrl);
	sprintf((char *)bN58_AT_Cmd_List[Command17],"AT+HTTPPARA=port,%s",bPort);
	for(i=0;i<sizeof(bN58_Cmd_List)/sizeof(bN58_Cmd_List[0]);i++)
	{
		N58_Send_Struct.bTask_Cmd_List[i]=bN58_Cmd_List[i];
		N58_Send_Struct.bTask_Ack_List[i]=bN58_Ack_List[i];
	}
	N58_Send_Struct.bResend_Count=3;
}

//HTTP HEAD ���󷽷�
//url				��Ŀ����ַ
//port			��Ŀ��˿�
void N58_Http_Head(u8 *bUrl,u8 *bPort)
{
	u8 i;
	u8 bN58_Cmd_List[4+1]={Command16,Command17,Command18,Command21,CommandNull};
	u8 bN58_Ack_List[4+1]={Command1,Command1,Command1,Command1,CommandNull};
	sprintf((char *)bN58_AT_Cmd_List[Command16],"AT+HTTPPARA=url,%s",bUrl);
	sprintf((char *)bN58_AT_Cmd_List[Command17],"AT+HTTPPARA=port,%s",bPort);
	for(i=0;i<sizeof(bN58_Cmd_List)/sizeof(bN58_Cmd_List[0]);i++)
	{
		N58_Send_Struct.bTask_Cmd_List[i]=bN58_Cmd_List[i];
		N58_Send_Struct.bTask_Ack_List[i]=bN58_Ack_List[i];
	}
	N58_Send_Struct.bResend_Count=3;
}

//HTTP HEAD ���󷽷�
//url				��Ŀ����ַ
//port			��Ŀ��˿�
void N58_Http_Post(u8 *bUrl,u8 *bPort,u8 bType,u8 *bCode)
{
	u8 i;
	u8 bN58_Cmd_List[4+1]={Command16,Command17,Command18,Command25,CommandNull};
	u8 bN58_Ack_List[4+1]={Command1,Command1,Command1,Command8,CommandNull};
	if(bType<0||bType>4)
		bType=1;
	sprintf((char *)bN58_AT_Cmd_List[Command16],"AT+HTTPPARA=url,%s",bUrl);
	sprintf((char *)bN58_AT_Cmd_List[Command17],"AT+HTTPPARA=port,%s",bPort);
	sprintf((char *)bN58_AT_Cmd_List[Command25],"AT+HTTPACTION=2,%d,%d",strlen((const char *)bCode),bType);
	strcpy((char *)bN58_Data_Post,(const char *)bCode);
	for(i=0;i<sizeof(bN58_Cmd_List)/sizeof(bN58_Cmd_List[0]);i++)
	{
		N58_Send_Struct.bTask_Cmd_List[i]=bN58_Cmd_List[i];
		N58_Send_Struct.bTask_Ack_List[i]=bN58_Ack_List[i];
	}
	bN58_Data_List[0]=bN58_Data_Post;
	bN58_Data_List[1]=0;
	N58_Send_Struct.bResend_Count=3;
}


/////////////////////////////// N58 ɨ�躯�� //////////////////////////////////////////////


void Scan_Send(void)
{
	u8 i;
	if(!N58_Send_Struct.bInfBit.bSending_Flag)
	{
		if(N58_Send_Struct.bInfBit.bSend_Finish)
		{
			if(N58_Send_Struct.bInfBit.bFinish_state)
			{
				if(N58_Send_Struct.bData_Mode)
				{
//					PC_USART("1N58_Send_Struct.bData_Mode=%d\r\n",N58_Send_Struct.bData_Mode);
//					for(i=0;i<sizeof(bN58_Data_List)/sizeof(bN58_Data_List[0])-1;i++)
//					{
//						bN58_Data_List[i]=bN58_Data_List[i+1];
//						bN58_Data_List[i]=bN58_Data_List[i+1];
//					}
				}
				else
				{
					if(N58_Send_Struct.bTask_Ack_List[0]==8)
						N58_Send_Struct.bData_Mode=1;
					for(i=0;i<sizeof(N58_Send_Struct.bTask_Cmd_List)/sizeof(N58_Send_Struct.bTask_Cmd_List[0])-1;i++)
					{
						N58_Send_Struct.bTask_Cmd_List[i]=N58_Send_Struct.bTask_Cmd_List[i+1];
						N58_Send_Struct.bTask_Ack_List[i]=N58_Send_Struct.bTask_Ack_List[i+1];
					}
				}
				N58_Send_Struct.bResend_Count=3;
			}
			else
			{
				N58_Send_Struct.bResend_Count--;
				delay_ms(1000);
			}
			N58_Send_Struct.bInfBit.bResponse_Flag=0;
			N58_Send_Struct.bInfBit.bSend_Finish=0;
			N58_Send_Struct.bInfBit.bRestart_Check=0;
			N58_Fram_Record_Struct.wInfAll=0;
		}
		else if(N58_Send_Struct.bTask_Cmd_List[0]!=0||N58_Send_Struct.bData_Mode)
		{
			if(N58_Send_Struct.bResend_Count==0)
			{
				bRestart_flag=1;
			}
			else
			{
				Send_Cmd();
			}
		}
	}
}

void Scan_N58_Usart(void)
{
	u8 *bData=0;
	u32 dwNum_csq=0;
	if(N58_Fram_Record_Struct.wInfBit.wFramFinishFlag)
	{
		if(N58_Send_Struct.bInfBit.bRestart_Check)
			N58_Send_Struct.bInfBit.bRestart_Check=0;
		N58_Fram_Record_Struct.bData_RX_BUF[N58_Fram_Record_Struct.wInfBit.wFramLength ] = '\0';
		if(N58_Send_Struct.bData_Mode)
		{
			PC_USART("%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
			bData=(u8 *)strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bN58_AT_Ack_List[7]);
			if(bData)
			{
				strcpy((char *)bN58_Data,(const char *)N58_Fram_Record_Struct.bData_RX_BUF);
				N58_Send_Struct.bData_Mode=0;
				N58_Send_Struct.bInfBit.bFinish_state=1;
				N58_Send_Struct.bInfBit.bSend_Finish=1;
				N58_Send_Struct.bInfBit.bSending_Flag=0;
			}
		}
		else if(N58_Send_Struct.bInfBit.bResponse_Flag==1)
		{
			bData=bN58_Check_Data((u8 *)bN58_AT_Ack_List[N58_Send_Struct.bTask_Ack_List[0]]);
			if(bData)
			{
				N58_Send_Struct.bInfBit.bFinish_state=1;
				N58_Send_Struct.bInfBit.bResponse_Flag=0;
			}
			else
			{
				N58_Send_Struct.bInfBit.bFinish_state=0;
			}
			N58_Send_Struct.bInfBit.bSend_Finish=1;
			N58_Send_Struct.bInfBit.bSending_Flag=0;
		}
		else if(N58_Send_Struct.bInfBit.bResponse_Flag==2)
		{
			dwNum_csq=atoi(strstr(strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,"+CSQ:"),":")+1);
			PC_USART("%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
			if(dwNum_csq>0&&dwNum_csq<=31)
			{
				N58_Send_Struct.bInfBit.bFinish_state=1;
			}
			else
			{
				N58_Send_Struct.bInfBit.bFinish_state=0;
			}
			N58_Send_Struct.bInfBit.bSend_Finish=1;
			N58_Send_Struct.bInfBit.bSending_Flag=0;
		}
		else if(N58_Send_Struct.bInfBit.bResponse_Flag==3)
		{
			bData=bN58_Check_Data((u8 *)bN58_AT_Ack_List[7]);
			if(bData)
			{
				
			}
			N58_Send_Struct.bInfBit.bFinish_state=1;
			N58_Send_Struct.bInfBit.bSend_Finish=1;
			N58_Send_Struct.bInfBit.bSending_Flag=0;
		}
		else
		{
			N58_Fram_Record_Struct.bData_RX_BUF[N58_Fram_Record_Struct.wInfBit.wFramLength ] = '\0';
			PC_USART("REC:\r\n%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
			N58_Fram_Record_Struct.wInfAll = 0;	//���¿�ʼ�����µ����ݰ�
		}
		N58_Fram_Record_Struct.wInfAll=0;
	}
	else if(N58_Send_Struct.bInfBit.bSending_Flag&&!N58_Send_Struct.bData_Mode)
	{
		if(!N58_Send_Struct.bInfBit.bRestart_Check)
		{
			N58_Send_Struct.bInfBit.bRestart_Check=1;
			TIM3->CNT=0;
//			PC_USART("TIM3->CNT=0\r\n");
		}
	}
}


void Send_Cmd(void)
{
	if(N58_Send_Struct.bData_Mode)
		N58_Send_AT_Cmd(bN58_Data_List[0],bN58_AT_Ack_List[1]);
	else
		N58_Send_AT_Cmd((u8 *)bN58_AT_Cmd_List[N58_Send_Struct.bTask_Cmd_List[0]],
			(u8 *)bN58_AT_Ack_List[N58_Send_Struct.bTask_Ack_List[0]]);
	N58_Send_Struct.bInfBit.bSending_Flag=1;
	if(strstr((const char *)bN58_AT_Cmd_List[N58_Send_Struct.bTask_Cmd_List[0]],"AT+CSQ"))
		N58_Send_Struct.bInfBit.bResponse_Flag=2;
	else if(strstr((const char *)bN58_AT_Cmd_List[N58_Send_Struct.bTask_Cmd_List[0]],"AT+HTTPACTION="))
		N58_Send_Struct.bInfBit.bResponse_Flag=3;
	else
		N58_Send_Struct.bInfBit.bResponse_Flag=1;
}

void Scan_Restart(void)
{
	u8 i;
	if(bRestart_flag)
	{
		PC_USART("\r\n��������\r\n");
		N58_Send_Struct.bInfAll=0;
		N58_Send_Struct.bResend_Count=3;
		for(i=0;i<sizeof(N58_Send_Struct.bTask_Cmd_List)/sizeof(N58_Send_Struct.bTask_Cmd_List[0]);i++)
		{
			N58_Send_Struct.bTask_Cmd_List[i]=0;
			N58_Send_Struct.bTask_Ack_List[i]=0;
		}
		bRestart_flag=0;
	}
}


/////////////////////////////// USART2 ���ڳ�ʼ�� //////////////////////////////////////////////

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
		if(N58_Fram_Record_Struct.wInfBit.wFramLength < RX_BUF_MAX_LEN - 1 )		//�����Խ�������
		{
			TIM_SetCounter(TIM4,0);//���������
			if(N58_Fram_Record_Struct.wInfAll==0)TIM4_Set(1);	 	//ʹ�ܶ�ʱ��4���ж�
			N58_Fram_Record_Struct.bData_RX_BUF[N58_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			N58_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//ǿ�Ʊ�ǽ������
		}
	}
}

//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������
void N58_Usart_Init(u32 bound)
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

	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������2

	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���

	//ʹ�ܽ����ж�
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	TIM4_Init(99,7199);		//10ms�ж�
	N58_Fram_Record_Struct.wInfAll=0;		//����
	TIM4_Set(0);			//�رն�ʱ��4
	
	TIM3_Int_Init(14999,14399);//5Khz�ļ���Ƶ�ʣ�������5000Ϊ1s��ÿ��������ʱ 14400/72M=0.2ms

}

//��ʱ��4�жϷ������		    
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{
		N58_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//��ǽ������
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


//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx
}
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־
		if(N58_Send_Struct.bInfBit.bRestart_Check)
			bRestart_flag=1;
	}
}


/////////////////////////////// USART2 ���ڷ��� //////////////////////////////////////////////
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

	while ( * Data != 0 )     // �ж��Ƿ񵽴��ַ���������
	{
		if ( * Data == 0x5c )  //'\'
		{
			switch ( *++Data )
			{
				case 'r':							          //�س���
				USART_SendData(USARTx, 0x0d);
				Data ++;
				break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
				s = va_arg(ap, const char *);
				for ( ; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;
				case 'd':
					//ʮ����
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


