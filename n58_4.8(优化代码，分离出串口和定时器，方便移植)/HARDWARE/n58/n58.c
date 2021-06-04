#include "n58.h"


struct STRUCT_USART_Fram N58_Fram_Record_Struct = { 0 };		//N58���ڽ��սṹ��
struct STRUCT_USART_Send N58_Send_Struct = { 0 };		//N58���ڷ��ͽṹ��



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
	"AT+TCPCLOSE=1",											//// 26
	"AT+TCPSETUP=1,v1-tml.lulucai.cn,18060",			//// 27
	"AT+IPSTATUS=1",											//// 28
	"AT+TCPSEND=1,10",										//// 29
	"AT+HTTPCLOSE",												//// 30
};

//����N58ģ������Ӧ��
u8 bN58_AT_Ack_List[][40]={
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
	"+TCPCLOSE: ERROR",										////  10
	"+TCPSETUP: 1,OK",										////  11
	"+IPSTATUS: 1,CONNECT,TCP,4096",			////  12
	"+TCPCLOSE: 1,OK",										////  13
	"+TCPSEND: 1,10",											////  14
	
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
	Command26,
	Command27,
	Command28,
	Command29,
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
	N58_printf("%s\r\n",bCmd);
	printf("%s\r\n",bCmd);
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
	printf("%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
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
		N58_printf("AT\r\n");
		printf("AT\r\n");
		delay_ms(1000);
	}
	for(i=0;i<sizeof(bN58_Cmd_List)/sizeof(bN58_Cmd_List[0]);i++)
	{
		N58_Send_Struct.bTask_Cmd_List[i]=bN58_Cmd_List[i];
		N58_Send_Struct.bTask_Ack_List[i]=bN58_Ack_List[i];
	}
	N58_Send_Struct.bResend_Count=3;
	N58_Send_Struct.bNow_Task=0;
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
	N58_Send_Struct.bNow_Task=0;
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
	N58_Send_Struct.bNow_Task=0;
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
	N58_Send_Struct.bNow_Task=0;
}

//HTTP HEAD ���󷽷�
//url				��Ŀ����ַ
//port			��Ŀ��˿�
void N58_Http_Post(u8 *bUrl,u8 *bPort,u8 bType,u8 *bCode)
{
	u8 i;
	u8 bN58_Cmd_List[4+1]={Command16,Command17,Command18,Command25,CommandNull};
	u8 bN58_Ack_List[4+1]={Command1,Command1,Command1,Command8,CommandNull};
	if(bType>4)
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
	N58_Send_Struct.bNow_Task=0;
}

//HTTP HEAD ���󷽷�
//url				��Ŀ����ַ
//port			��Ŀ��˿�
void N58_Tcp_Link(u8 *bUrl,u8 *bPort,u8 *bCode)
{
	u8 i;
	u8 bN58_Cmd_List[5+1]={Command26,Command27,Command28,Command29,Command26,CommandNull};
	u8 bN58_Ack_List[5+1]={Command10,Command11,Command12,Command8,Command13,CommandNull};
	sprintf((char *)bN58_AT_Cmd_List[Command27],"AT+TCPSETUP=1,%s,%s",bUrl,bPort);
	sprintf((char *)bN58_AT_Cmd_List[Command29],"AT+TCPSEND=1,%d",strlen((const char *)bCode));
	sprintf((char *)bN58_AT_Ack_List[Command14],"+TCPSEND: 1,%d",strlen((const char *)bCode));
	strcpy((char *)bN58_Data_Post,(const char *)bCode);
	for(i=0;i<sizeof(bN58_Cmd_List)/sizeof(bN58_Cmd_List[0]);i++)
	{
		N58_Send_Struct.bTask_Cmd_List[i]=bN58_Cmd_List[i];
		N58_Send_Struct.bTask_Ack_List[i]=bN58_Ack_List[i];
	}
	bN58_Data_List[0]=bN58_Data_Post;
	bN58_Data_List[1]=0;
	N58_Send_Struct.bResend_Count=3;
	N58_Send_Struct.bNow_Task=0;
}


/////////////////////////////// N58 ɨ�躯�� //////////////////////////////////////////////


void Scan_Send(void)
{
	if(!N58_Send_Struct.bInfBit.bSending_Flag)
	{
		if(N58_Send_Struct.bInfBit.bSend_Finish)
		{
			if(N58_Send_Struct.bInfBit.bFinish_state)
			{
				if(N58_Send_Struct.bData_Mode)
				{
//					printf("1N58_Send_Struct.bData_Mode=%d\r\n",N58_Send_Struct.bData_Mode);
//					for(i=0;i<sizeof(bN58_Data_List)/sizeof(bN58_Data_List[0])-1;i++)
//					{
//						bN58_Data_List[i]=bN58_Data_List[i+1];
//						bN58_Data_List[i]=bN58_Data_List[i+1];
//					}
				}
				else
				{
					if(N58_Send_Struct.bTask_Ack_List[N58_Send_Struct.bNow_Task]==8)
						N58_Send_Struct.bData_Mode=1;
					N58_Send_Struct.bNow_Task++;
//					for(i=0;i<sizeof(N58_Send_Struct.bTask_Cmd_List)/sizeof(N58_Send_Struct.bTask_Cmd_List[0])-1;i++)
//					{
//						N58_Send_Struct.bTask_Cmd_List[i]=N58_Send_Struct.bTask_Cmd_List[i+1];
//						N58_Send_Struct.bTask_Ack_List[i]=N58_Send_Struct.bTask_Ack_List[i+1];
//					}
				}
				N58_Send_Struct.bResend_Count=3;
			}
			else
			{
				//�����ﷵ����һ��
				N58_Send_Struct.bResend_Count--;
				delay_ms(1000);
			}
			N58_Send_Struct.bInfBit.bResponse_Flag=0;
			N58_Send_Struct.bInfBit.bSend_Finish=0;
			N58_Send_Struct.bInfBit.bRestart_Check=0;
			N58_Fram_Record_Struct.wInfAll=0;
		}
		else if(N58_Send_Struct.bTask_Cmd_List[N58_Send_Struct.bNow_Task]!=0||N58_Send_Struct.bData_Mode)
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
			printf("%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
			if((u8 *)strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bN58_AT_Ack_List[7]))
				bData=(u8 *)strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bN58_AT_Ack_List[7]);
			else if((u8 *)strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bN58_AT_Ack_List[14]))
				bData=(u8 *)strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bN58_AT_Ack_List[14]);
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
			bData=bN58_Check_Data((u8 *)bN58_AT_Ack_List[N58_Send_Struct.bTask_Ack_List[N58_Send_Struct.bNow_Task]]);
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
			printf("%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
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
		else if(N58_Send_Struct.bInfBit.bResponse_Flag==4)
		{
			bData=bN58_Check_Data((u8 *)bN58_AT_Ack_List[11]);
			if(bData)
			{
				N58_Send_Struct.bInfBit.bFinish_state=1;
				N58_Send_Struct.bInfBit.bSend_Finish=1;
				N58_Send_Struct.bInfBit.bSending_Flag=0;
			}
		}
		else
		{
			N58_Fram_Record_Struct.bData_RX_BUF[N58_Fram_Record_Struct.wInfBit.wFramLength ] = '\0';
			printf("REC:\r\n%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
			N58_Fram_Record_Struct.wInfAll = 0;	//���¿�ʼ�����µ����ݰ�
		}
		N58_Fram_Record_Struct.wInfAll=0;
	}
	else if(N58_Send_Struct.bInfBit.bSending_Flag||N58_Send_Struct.bData_Mode)
	{
		if(!N58_Send_Struct.bInfBit.bRestart_Check&&!bRestart_flag)
		{
			N58_Send_Struct.bInfBit.bRestart_Check=1;
			bRestart_Check_CNT=0;
		}
	}
}


void Send_Cmd(void)
{
	if(N58_Send_Struct.bData_Mode)
		N58_Send_AT_Cmd(bN58_Data_List[0],bN58_AT_Ack_List[1]);
	else
		N58_Send_AT_Cmd((u8 *)bN58_AT_Cmd_List[N58_Send_Struct.bTask_Cmd_List[N58_Send_Struct.bNow_Task]],
			(u8 *)bN58_AT_Ack_List[N58_Send_Struct.bTask_Ack_List[N58_Send_Struct.bNow_Task]]);
	N58_Send_Struct.bInfBit.bSending_Flag=1;
	if(strstr((const char *)bN58_AT_Cmd_List[N58_Send_Struct.bTask_Cmd_List[N58_Send_Struct.bNow_Task]],"AT+CSQ"))
		N58_Send_Struct.bInfBit.bResponse_Flag=2;
	else if(strstr((const char *)bN58_AT_Cmd_List[N58_Send_Struct.bTask_Cmd_List[N58_Send_Struct.bNow_Task]],"AT+HTTPACTION="))
		N58_Send_Struct.bInfBit.bResponse_Flag=3;
	else if(strstr((const char *)bN58_AT_Cmd_List[N58_Send_Struct.bTask_Cmd_List[N58_Send_Struct.bNow_Task]],"AT+TCPSETUP="))
		N58_Send_Struct.bInfBit.bResponse_Flag=4;
	else
		N58_Send_Struct.bInfBit.bResponse_Flag=1;
}

void Scan_Restart(void)
{
	u8 i;
	if(bRestart_flag)
	{
		printf("\r\n��������\r\n");
		N58_Send_Struct.bInfAll=0;
		N58_Send_Struct.bResend_Count=3;
		for(i=0;i<sizeof(N58_Send_Struct.bTask_Cmd_List)/sizeof(N58_Send_Struct.bTask_Cmd_List[0]);i++)
		{
			N58_Send_Struct.bTask_Cmd_List[i]=0;
			N58_Send_Struct.bTask_Ack_List[i]=0;
		}
		bRestart_flag=0;
		N58_Send_Struct.bData_Mode=0;
		N58_Send_Struct.bNow_Task=0;
	}
}


/////////////////////////////// USART2 ���ڳ�ʼ�� //////////////////////////////////////////////

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u8 bN58_USART_RX_CHECK=0;
u8 bN58_USART_RX_CNT=0;
u16 bRestart_Check_CNT=0;
u8 bRestart_flag=0;

void N58_USART_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(cN58_USART, USART_IT_RXNE) != RESET)//���յ�����
	{
		res =USART_ReceiveData(cN58_USART);		
		if(N58_Fram_Record_Struct.wInfBit.wFramLength < cRX_BUF_MAX_LEN - 1 )		//�����Խ�������
		{
			bN58_USART_RX_CNT=0;
			if(N58_Fram_Record_Struct.wInfAll==0)bN58_USART_RX_CHECK=1;
			N58_Fram_Record_Struct.bData_RX_BUF[N58_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			N58_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//ǿ�Ʊ�ǽ������
		}
	}
}

void N58_USART_SCAN(void)
{
	if(bN58_USART_RX_CHECK)
	{
		bN58_USART_RX_CNT++;
		if(bN58_USART_RX_CNT==10)
		{
			N58_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//��ǽ������
			bN58_USART_RX_CNT=0;
			bN58_USART_RX_CHECK=0;	
		}
	}
}

void N58_Restart_Check_Scan(void)
{
	if(N58_Send_Struct.bInfBit.bRestart_Check)
	{
		bRestart_Check_CNT++;
		if(bRestart_Check_CNT==2999)
		{
			bRestart_flag=1;
			bRestart_Check_CNT=0;
			N58_Send_Struct.bInfBit.bRestart_Check=0;
		}
	}
}


/////////////////////////////// USART2 ���ڷ��� //////////////////////////////////////////////

//����,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
u8 bN58_Data_TX_BUF[cRX_BUF_MAX_LEN]={0};
void N58_printf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bN58_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bN58_Data_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(cN58_USART,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	  USART_SendData(cN58_USART,bN58_Data_TX_BUF[j]); 
	}
}


