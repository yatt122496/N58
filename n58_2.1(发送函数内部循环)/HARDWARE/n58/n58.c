#include "n58.h"


struct STRUCT_USART_Fram N58_Fram_Record_Struct = { 0 };		//N58���ڽ��սṹ��

volatile u8 restart_flag=0;

//��4gģ�鷢��ATָ��
//cmd					�����͵�ָ��
//acd1				���ڴ���Ӧ���������Ϊ NULL�����ʾ����Ҫ�ȴ�Ӧ��
//acd2				���ڴ���Ӧ���������Ϊ NULL�����ʾ����Ҫ�ȴ�Ӧ��
//time				���ȴ�ʱ�䣨��λ��10ms)
//send_count	��ʧ���ط�����
//����ֵ	��0�����ͳɹ����õ����ڴ���Ӧ������
//					1������ʧ��
u8 N58_Send_AT_Cmd(u8 *cmd,u8 *ack1,u8 *ack2,u8 time,u8 send_count)
{
	u8 count;
	if(send_count<=0)
		return 1;
	while(send_count--)
	{
		N58_Fram_Record_Struct.InfAll = 0;	//���¿�ʼ�����µ����ݰ�
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

//���ҽ��ջ������ַ���
//str				:��Ҫ���ҵ��ַ���
//����ֵ		��  		0��û���ҵ����ַ���
//						 ����ֵ�����ظ��ַ�������λ�õ�ָ��
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

//��4gģ�鷢������
//cmd					�����͵����ݣ�����Ҫ��ӻس��ˣ�
//acd1				���ڴ���Ӧ���������Ϊ NULL�����ʾ����Ҫ�ȴ�Ӧ��
//acd2				���ڴ���Ӧ���������Ϊ NULL�����ʾ����Ҫ�ȴ�Ӧ��
//time				���ȴ�ʱ�䣨��λ��10ms)
//send_count	��ʧ���ط�����
//����ֵ	��0�����ͳɹ����õ����ڴ���Ӧ������
//					1������ʧ��
u8 N58_Send_Data(u8 *cmd,u8 *ack1,u8 *ack2,u8 time,u8 send_count)
{
	u8 count;
	if(send_count<=0)
		return 1;
	while(send_count--)
	{
		N58_Fram_Record_Struct.InfAll = 0;	//���¿�ʼ�����µ����ݰ�
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

//n58��ʼ������
//����ֵ		��	0����ʼ���ɹ�
//						1����ʼ��ʧ��
u8 N58_Init(void)
{
	u8 i,num_csq=0;
	OSTimeDlyHMSM(0,0,10,0);
	if(N58_Send_AT_Cmd("AT","OK",NULL,100,5))							//���ڽ�ͨ���ԣ�������"OK"���������һ������֮��������
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("ATE0","OK",NULL,100,3))							//���÷��Ͳ����ԣ�������"OK"���������һ������֮��������
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CGSN","OK",NULL,100,3))							//ģ��IMEI��������"OK"���������һ������֮��������
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CGMR","OK",NULL,100,3))							//ģ��̼��汾��������"OK"���������һ������֮��������
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CPIN?","READY",NULL,100,5))							//SIM��״̬��������"READY"���������һ������֮��������
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CIMI","OK",NULL,100,2))							//SIM��IMSI��������"OK"���������һ������֮��������
	{
		return 1;
	}
	
	i=5;
	while(i--)							//�ź�ǿ��"������+CSQ: <signal>,<ber>,��0<signal<=31ʱ������һ��
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
	
	if(N58_Send_AT_Cmd("AT+CREG=2","OK",NULL,100,3))							//�����ϱ�����ע��С����Ϣ��������"OK"���������һ������֮��������
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CREG?","+CREG: 2,1","+CREG: 2,5",100,5))							//��ѯ����ע�����,������"+CREG: 2,1"����"+CREG: 2,5"�ֶΣ��������һ������֮��������
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CREG=0","OK",NULL,100,3))							//�����ϱ�����ע��С����Ϣ��������"OK"���������һ������֮��������
	{
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CGATT?","+CGATT: 1",NULL,100,5))							//�����ϱ�����ע��С����Ϣ��������"OK"���������һ������֮��������
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",NULL,100,3))					//����APN��������"OK"���������һ������֮��������
	{
		restart_flag=1;
		return 1;
	}
	
	if(N58_Send_AT_Cmd("AT+XGAUTH=1,1,\"gsm\",\"1234\"","OK",NULL,100,3))					//���������֤������������"OK"���������һ������֮��������
	{
		restart_flag=1;
		return 1;
	}
	
	N58_Fram_Record_Struct.InfAll=0;
	return 0;
}

//ppp����ͨ���������
//����ֵ		��	0�����ɹ�
//						1�����ʧ��
u8 N58_Ppp_Init(void)
{
	if(N58_Send_AT_Cmd("AT+XIIC=1","OK",NULL,100,3))							//����PPP���ţ�������"OK"���������һ������֮�������ţ�1s/�Σ����3�Σ�3�λ�����������
	{	
		restart_flag=1;
		return 1;
	}
	if(N58_Send_AT_Cmd("AT+XIIC?","+XIIC:    1",NULL,100,3))				//��ѯPPP���ӽ�����������������"+XIIC:	1",�������һ������֮ѭ����ѯ��1s/�Σ����30�Ρ�
	{
		restart_flag=1;
		return 1;
	}
	N58_Fram_Record_Struct.InfAll=0;
	return 0;
}

///HTTP GET ���󷽷�
//url			��Ŀ����ַ
//port		��Ŀ��˿�
//offset	���ֶ�����ʱ��ƫ�Ƶ�ַ
//size		���ֶ�����ʱÿ�����ش�С
//	��ʹ�÷ֶ�����ʱ��offset ,size ������Ϊ NULL 
//����ֵ		��1������ɹ�
//					0������ʧ��
//	ע����յ����ݲ�Ҫ�������ջ�������С
u8 *N58_Http_Get(u8 *url,u8 *port,u8 *offset,u8 *size)
{
	u8 cmd[40]={0},segmentDownload;
	u8 time=100,*data=0;
	u32 num_offset,num_size,num_length=0;
	sprintf((char *)cmd,"AT+HTTPPARA=url,%s",url);			//��������
	if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))
	{
		return NULL;
	}
	sprintf((char *)cmd,"AT+HTTPPARA=port,%s",port);		//���ö˿ں�
	if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))
	{
		return NULL;
	}
	if(N58_Send_AT_Cmd("AT+HTTPSETUP","OK",NULL,200,3))			//����HTTP����
	{
		restart_flag=1;
		return NULL;
	}
	if(offset&&size)
		segmentDownload=1;		
	else
		segmentDownload=0;
	if(!segmentDownload)																//���ֶ�����HTTP
	{
		sprintf((char *)cmd,"AT+HTTPACTION=0");
		if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))		//ִ��http get ����
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
		num_offset=(u32)atoi((const char *)offset);						//�ַ���תΪ����
		num_size=(u32)atoi((const char *)size);
		do																										//�ֶ�����HTTP
		{
			if(N58_Send_AT_Cmd("AT+HTTPSETUP","OK",NULL,200,3))			//����HTTP����
			{
				restart_flag=1;
				return NULL;
			}
			sprintf((char *)cmd,"AT+HTTPACTION=0,%d,%d",num_offset,num_size);
			if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))		//ִ��http get ����
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

//HTTP HEAD ���󷽷�
//url				��Ŀ����ַ
//port			��Ŀ��˿�
//����ֵ		��1������ɹ�
//						0������ʧ��
u8 *N58_Http_Head(u8 *url,u8 *port)
{
	u8 cmd[40]={0};
	u8 time=100,*data=0;
	sprintf((char *)cmd,"AT+HTTPPARA=url,%s",url);			//��������
	if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))
	{
		return NULL;
	}
	sprintf((char *)cmd,"AT+HTTPPARA=port,%s",port);		//���ö˿ں�
	if(N58_Send_AT_Cmd(cmd,(u8 *)"OK",NULL,100,3))
	{
		return NULL;
	}
	if(N58_Send_AT_Cmd((u8 *)"AT+HTTPSETUP",(u8 *)"OK",NULL,200,3))			//����HTTP����
	{
		restart_flag=1;
		return NULL;
	}
	if(N58_Send_AT_Cmd((u8 *)"AT+HTTPACTION=1",(u8 *)"OK",NULL,100,3))				//ִ��http get ����
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


//N58 ������������
//volume			��������С(0-6)
//speed				�������ٶ�(0-6)
//pitch				����ɫ(0-6)
//format			���������ݱ����ʽ(0-1),0:gbk���룻1:UTF-16����
//����ֵ			��0�����óɹ�
//							1������ʧ��
u8 N58_Tts_Set(u8 volume,u8 speed,u8 pitch,u8 format)
{
	u8 cmd[40]={0},cfg_flag=0,fmt_flag=0,res;
	sprintf((char *)cmd,"AT+TTSCFG=%d,%d,%d",volume,speed,pitch);			//������������
	cfg_flag=!N58_Send_AT_Cmd(cmd,"OK",NULL,100,3);						//���������������ã�������"OK"���������һ������֮�������ţ�1s/�Σ����3�Σ�3�λ�����������
	
	sprintf((char *)cmd,"AT+TTSFMT=%d",format);			//�������ݱ����ʽ
	fmt_flag=!N58_Send_AT_Cmd(cmd,"OK",NULL,100,3);
	if(cfg_flag&&fmt_flag)
		res=0;
	else if(cfg_flag)
	{
		sprintf((char *)cmd,"AT+TTSFMT=%d",format);			//�������ݱ����ʽ
		if(N58_Send_AT_Cmd(cmd,"OK",NULL,100,3))
		{
			restart_flag=1;
			res=1;
		}
	}
	else if(fmt_flag)
	{
		sprintf((char *)cmd,"AT+TTSCFG=%d,%d,%d",volume,speed,pitch);			//������������
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

//N58 ��������
//size				�����������ֽ�
//code				���������ݱ���
//����ֵ			��1�����ͳɹ�
//							0������ʧ��
u8 *N58_Tts_Play(u8 size, u8 *code)
{
	u8 cmd[40]={0},time=100,*data=0,second=0;
	u16 msecond=0,num=0;
	sprintf((char *)cmd,"AT+TTSPLAY=%d",size);			//������������
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


