#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "n58.h"


const u8 *bVoice_code_gbk[]={
	"BBB6D3ADB3CBD7F8D6D0B9FACCE5D3FDB2CAC6B1CFFACADBB3B5A3ACCEA2D0C5C9A8C3E8B2CAC6B1BBFAC9CFB5C4B6FECEACC2EBBCB4BFC9B9BAC2F2D6D0B9FACCE5D3FDB2CAC6B1",
	"C9A8C2EBBFC9D2D4B8F8CAD6BBFAD6C7C4DCBFECCBD9B3E4B5E7",
	"C7EBC4FAC9A8C3E8B1BEB2CAC6B1BBFAB6FECEACC2EBBDF8D0D0D6C7C4DCBFECCBD9B3E4B5E7",
	"C4FABAC3A3BAC4FAC3E6C7B0B5C4CAC7D6D0B9FACCE5D3FDB2CAC6B1D7D4D6FACFFACADBBBFAA3ACCFD6D4DAC4FABFC9D2D4D1A1B9BAD6D0B9FACCE5D3FDB2CAC6B1A3ACB9BAC2F2CDEAB3C9BAF3C1A2BCB4BFAABDB1",
	"BADCB1A7C7B8A3ACB2CAC6B1D2D1CADBCDEA",
	"C4FAD2D1D6A7B8B6CDEAB3C9A3ACC7EBC4CDD0C4B5C8B4FD",
	"CDF8C2E7C1B4BDD3B3ACCAB1A3ACD2D1CDCBBFEEA3ACC4FABFC9D4DA31B7D6D6D3BAF3D4D9B4CEB3A2CAD4A1A3C8E7D3D0CECACCE2C7EBD4DAD0A1B3CCD0F2D2B3C3E6C1AACFB5CADBBAF3B7FECEF1",
	"C7EBBDABBAECC9ABCBAEBEA7C0ADB8CBCFF2B3B5CEB2B7BDCFF2D3C3C1A6C0ADB3F6",
	
};
const u8 *bVoice_chinese[]={
	"��ӭ�����й�������Ʊ���۳���΢��ɨ���Ʊ���ϵĶ�ά�뼴�ɹ����й�������Ʊ",
	"ɨ����Ը��ֻ����ܿ��ٳ��",
	"����ɨ�豾��Ʊ����ά��������ܿ��ٳ��",
	"���ã�����ǰ�����й�������Ʊ���������ۻ�������������ѡ���й�������Ʊ��������ɺ���������",
	"�ܱ�Ǹ����Ʊ������",
	"����֧����ɣ������ĵȴ�",
	"�������ӳ�ʱ�����˿������1���Ӻ��ٴγ��ԡ�������������С����ҳ����ϵ�ۺ����",
	"�뽫��ɫˮ��������β������������",
	
};

void Send_Cmd(void);
void Scan_Usart1(void);
void Scan_Usart2(void);
void Scan_Restart(void);
void Scan_Send(void);

/************************************************
 ALIENTEK��ӢSTM32������ʵ��8
 ��ʱ���ж�ʵ��  
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
	
int main(void)
{
	
	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	USART2_Init(115200);
 	LED_Init();			     //LED�˿ڳ�ʼ��
	TIM3_Int_Init(14999,14399);//5Khz�ļ���Ƶ�ʣ�������5000Ϊ1s��ÿ��������ʱ 14400/72M=0.2ms
	
  while(1)
	{
		Scan_Send();
		Scan_Restart();
		Scan_Usart1();
		Scan_Usart2();
		
		LED0=!LED0;
	}
}	 

void Scan_Usart1(void)
{
	u16 wLen;
	static u8 i=0;
	if(USART_RX_STA&0x8000)
	{
		wLen=USART_RX_STA&0x3fff;
		USART_RX_BUF[wLen]=0;
		N58_USART("%s\r\n",USART_RX_BUF);//���뻻��
		USART_RX_STA=0;
		if(N58_Send_Struct.bInfBit.bSending_Flag||N58_Send_Struct.bData_Mode)
		{
			
		}
		else if(strcmp((const char *)USART_RX_BUF,"K0")==0)
		{
			N58_Init();
		}
		else if(strcmp((const char *)USART_RX_BUF,"K1")==0)
		{
			N58_Ppp_Init();
		}
		else if(strcmp((const char *)USART_RX_BUF,"K2")==0)
		{
			N58_Http_Get((u8 *)"www.baidu.com",(u8 *)"80",(u8 *)"0",(u8 *)"512");
		}
		else if(strcmp((const char *)USART_RX_BUF,"K3")==0)
		{
			N58_Http_Head((u8 *)"www.baidu.com",(u8 *)"80");
		}
		else if(strcmp((const char *)USART_RX_BUF,"K4")==0)
		{
			N58_Tts_Set(3,3,3,0);
		}
		else if(strcmp((const char *)USART_RX_BUF,"K5")==0)
		{
			PC_USART("���������������£�\r\n%s\r\n",bVoice_chinese[i]);
			N58_Tts_Play( strlen((const char *)bVoice_code_gbk[i] ) / 2 , (u8 *)bVoice_code_gbk[i] );
			i++;
			if( i > ( sizeof(bVoice_code_gbk)/sizeof(bVoice_code_gbk[0]) - 1 ) )
				i=0;
		}
		else if(strcmp((const char *)USART_RX_BUF,"K6")==0)
		{
			N58_Http_Post((u8 *)"apk.vita-course.com/darts/nibp/calibrate",(u8 *)"80",1,(u8 *)"12");
		}
		memset((char*)USART_RX_BUF,0x00,wLen);//���ڽ��ջ�������0
	}
}

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

void Scan_Usart2(void)
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
//			if(!strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,"OK"))
//				bData=bN58_Check_Data((u8 *)bN58_AT_Ack_List[9]);
			PC_USART("%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
			bData=(u8 *)(strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bN58_AT_Ack_List[9])
				||strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bN58_AT_Ack_List[7]));
			if(bData)
			{
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
//	else if(N58_Send_Struct.bInfBit.bResend_Flag)
//	{
//		N58_Send_Struct.bInfBit.bFinish_state=0;
//		N58_Send_Struct.bInfBit.bSend_Finish=1;
//		N58_Send_Struct.bInfBit.bSending_Flag=0;
//		N58_Send_Struct.bInfBit.bResend_Flag=0;
//		N58_Send_Struct.bInfBit.bRestart_Check=0;
//	}
}
