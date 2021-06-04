#include "n58.h"


struct STRUCT_USART_Fram N58_Fram_Record_Struct = { 0 };		//N58串口接收结构体
struct STRUCT_USART_Send N58_Send_Struct = { 0 };		//N58串口发送结构体



//定义发送的命令
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

//定义N58模块命令应答
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

//定义N58数据模式数组
u8 *bN58_Data_List[10]={0};

//定义N58网址
u8 bN58_Data_Post[80]={0};

u8 bN58_Data[1024]={0};

//定义枚举数组
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
	
//向4g模块发送AT指令
//cmd					：发送的指令
//acd1				：期待的应答结果，如果为 NULL，则表示不需要等待应答
//acd2				：期待的应答结果，如果为 NULL，则表示不需要等待应答
//time				：等待时间（单位：10ms)
//send_count	：失败重发次数
//返回值	：0，发送成功（得到了期待的应答结果）
//					1，发送失败
void N58_Send_AT_Cmd(u8 *bCmd,u8 *bAck)
{
	N58_Fram_Record_Struct.wInfAll = 0;	//从新开始接收新的数据包
	N58_printf("%s\r\n",bCmd);
	printf("%s\r\n",bCmd);
}


//查找接收缓冲区字符串
//str				:需要查找的字符串
//返回值		：  		0，没有找到该字符串
//						 其他值，返回该字符串所在位置的指针
u8 *bN58_Check_Data(u8 *bStr)
{
	char *bStrx=0;
	if((strcmp((const char *)N58_Fram_Record_Struct.bData_RX_BUF,"\r\n")==0)||(strcmp((const char *)N58_Fram_Record_Struct.bData_RX_BUF,"")==0))
		return 0;
	printf("%s\r\n", N58_Fram_Record_Struct.bData_RX_BUF);
	bStrx=strstr((const char*)N58_Fram_Record_Struct.bData_RX_BUF,(const char *)bStr);
	return (u8 *)bStrx;
}


//n58初始化程序
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

//ppp基础通信条件检测
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

///HTTP GET 请求方法
//url			：目标网址
//port		：目标端口
//offset	：分段下载时的偏移地址
//size		：分段下载时每段下载大小
//	不使用分段下载时，offset ,size 都设置为 NULL 
//	注意接收的内容不要超过接收缓冲区大小
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

//HTTP HEAD 请求方法
//url				：目标网址
//port			：目标端口
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

//HTTP HEAD 请求方法
//url				：目标网址
//port			：目标端口
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

//HTTP HEAD 请求方法
//url				：目标网址
//port			：目标端口
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


/////////////////////////////// N58 扫描函数 //////////////////////////////////////////////


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
				//从这里返回上一步
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
			N58_Fram_Record_Struct.wInfAll = 0;	//从新开始接收新的数据包
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
		printf("\r\n正在重启\r\n");
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


/////////////////////////////// USART2 串口初始化 //////////////////////////////////////////////

//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u8 bN58_USART_RX_CHECK=0;
u8 bN58_USART_RX_CNT=0;
u16 bRestart_Check_CNT=0;
u8 bRestart_flag=0;

void N58_USART_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(cN58_USART, USART_IT_RXNE) != RESET)//接收到数据
	{
		res =USART_ReceiveData(cN58_USART);		
		if(N58_Fram_Record_Struct.wInfBit.wFramLength < cRX_BUF_MAX_LEN - 1 )		//还可以接收数据
		{
			bN58_USART_RX_CNT=0;
			if(N58_Fram_Record_Struct.wInfAll==0)bN58_USART_RX_CHECK=1;
			N58_Fram_Record_Struct.bData_RX_BUF[N58_Fram_Record_Struct.wInfBit.wFramLength++]=res;		//记录接收到的值	 
		}else 
		{
			N58_Fram_Record_Struct.wInfBit.wFramFinishFlag=1;					//强制标记接收完成
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
			N58_Fram_Record_Struct.wInfBit.wFramFinishFlag = 1;	//标记接收完成
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


/////////////////////////////// USART2 串口发送 //////////////////////////////////////////////

//串口,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
u8 bN58_Data_TX_BUF[cRX_BUF_MAX_LEN]={0};
void N58_printf(char* fmt,...)
{
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char *)bN58_Data_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)bN58_Data_TX_BUF);		//此次发送数据的长度
	for(j=0;j<i;j++)							//循环发送数据
	{
	  while(USART_GetFlagStatus(cN58_USART,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	  USART_SendData(cN58_USART,bN58_Data_TX_BUF[j]); 
	}
}


