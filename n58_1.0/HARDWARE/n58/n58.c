#include "n58.h"

//向4g模块发送命令后，检测接收到的应答
//str：期待的应答结果
//返回值：0，没有得到期待的应答结果
//				其他，期待应答结果的位置（str的位置）
u8* n58_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)
	{
		USART2_RX_BUF[USART2_RX_STA&0X7fff]=0;
		printf("%s\r\n",USART2_RX_BUF);
		strx=strstr((const char*)USART2_RX_BUF,(const char *)str);
	}
	return (u8*)strx;
}
//向4g模块发送命令
//cmd：发送的命令字符串
//acd：期待的应答结果，如果为空，则表示不需要等待应答
//waittime：等待时间（单位：10ms)
//返回值：0，发送成功（得到了期待的应答结果）
//				1，发送失败
u8 n58_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0;
	USART2_RX_STA=0;
	u2_printf("%s\r\n",cmd);
	printf("%s\r\n",cmd);
	if(ack&&waittime)
	{
		while(--waittime)
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)
			{
				if(n58_check_cmd(ack)) break;
				USART2_RX_STA=0;
			}
		}
		if(waittime==0)res=1;
	}
	return res;
}
//向4g模块发送数据
//data：发送的数据（不需要添加回车了）
//acd：期待的应答结果，如果为空，则表示不需要等待应答
//waittime：等待时间（单位：10ms)
//返回值：0，发送成功（得到了期待的应答结果）
//				1，发送失败
u8 n58_send_data(u8 *data,u8 *ack,u16 waittime)
{
	u8 res=0;
	USART2_RX_STA=0;
	u2_printf("%s",data);
	printf("%s",data);
	if(ack&&waittime)
	{
		while(--waittime)
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)
			{
				if(n58_check_cmd(ack)) {
					USART2_RX_STA=0;
					break;
				}
				USART2_RX_STA=0;
			}
		}
		if(waittime==0)res=1;
	}
	return res;
}

//n58初始化程序
u8 n58_init(void)
{
	u8 i,num_csq=0;
	OSTimeDlyHMSM(0,0,10,0);
	if(n58_send_cmd("AT","OK",100))							//串口接通测试，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("ATE0","OK",100))							//设置发送不回显，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("ATE0","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CGSN","OK",100))							//模块IMEI，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CGSN","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CGMR","OK",100))							//模块固件版本，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CGMR","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CPIN?","READY",100))							//SIM卡状态，若返回"READY"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CPIN?","READY",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CIMI","OK",100))							//SIM卡IMSI，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CIMI","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	i=5;
	while(i--)							//信号强度"若返回+CSQ: <signal>,<ber>,且0<signal<=31时进行下一步
	{
		n58_send_cmd("AT+CSQ","+CSQ:",100);
		if(USART2_RX_STA&0X8000)
		{
			num_csq=atoi(strstr(strstr((const char*)USART2_RX_BUF,"+CSQ:"),":")+1);
			USART2_RX_STA=0;
			if(num_csq>0&&num_csq<=31) break;
		}
	}
	if(i<=0) return 1;
	
	if(n58_send_cmd("AT+CREG=2","OK",100))							//主动上报网络注册小区信息，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CREG=2","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CREG?","+CREG: 2,1",100)&&n58_send_cmd("AT+CREG?","+CREG: 2,5",100))							//查询网络注册情况,若返回"+CREG: 2,1"或者"+CREG: 2,5"字段，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(n58_send_cmd("AT+CREG?","+CREG: 2,1",100)&&n58_send_cmd("AT+CREG?","+CREG: 2,5",100)) break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CREG=0","OK",100))							//主动上报网络注册小区信息，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CREG=0","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CGATT?","+CGATT: 1",100))							//主动上报网络注册小区信息，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CGATT?","+CGATT: 1",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",100))					//设置APN，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",100)) break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+XGAUTH=1,1,\"gsm\",\"1234\"","OK",100))					//设置身份认证参数，若返回"OK"，则进行下一步，反之继续拨号
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+XGAUTH=1,1,\"gsm\",\"1234\"","OK",100)) break;
			if(i==1) return 1;
		}
	}
	
	return 0;
}

//ppp基础通信条件检测
u8 n58_ppp_init(void)
{
	u8 i;
	if(n58_send_cmd("AT+XIIC=1","OK",100))							//进行PPP拨号，若返回"OK"，则进行下一步，反之继续拨号，1s/次，最多3次，3次还不行则重启
	{
		for(i=3;i>0;i--)
		{
			if(!n58_send_cmd("AT+XIIC=1","OK",1000))break;
			if(i==1)return 1;
		}
	}
	
	if(n58_send_cmd("AT+XIIC?","+XIIC:    1",100))				//查询PPP连接建立情况，若回码包含"+XIIC:	1",则进行下一步，反之循环查询，1s/次，最多30次。
	{
		for(i=30;i>0;i--)
		{
			if(!n58_send_cmd("AT+XIIC?","+XIIC:    1",1000))break;
			if(i==1)return 1;
		}
		return 0;
	}
	return 0;
}
///HTTP GET 请求方法
u8* n58_http_get(u8 *url,u8 *port,u8 *offset,u8 *size)
{
	u8 cmd[40]={0},segmentDownload;
	u8 *data=0,time=100;
	u32 num_offset,num_size,num_length=0;
	sprintf((char *)cmd,"AT+HTTPPARA=url,%s",url);			//设置域名
	while(n58_send_cmd(cmd,"OK",100));
	sprintf((char *)cmd,"AT+HTTPPARA=port,%s",port);		//设置端口号
	while(n58_send_cmd(cmd,"OK",100));
	while(n58_send_cmd("AT+HTTPSETUP","OK",1000));			//建立HTTP连接
	if(offset&&size)
		segmentDownload=1;		
	else
		segmentDownload=0;
	if(!segmentDownload)																//不分段请求HTTP
	{
		sprintf((char *)cmd,"AT+HTTPACTION=0");
		while(n58_send_cmd(cmd,"OK",100));		//执行http get 请求
		USART2_RX_STA=0;
		while(--time)
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)
			{
				data=n58_check_cmd("+HTTPRECV:");
				USART2_RX_STA=0;
				if(data)
				{
//					strcpy(WebTemp,data);
				}
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
			delay_ms(200);
			while(n58_send_cmd("AT+HTTPSETUP","OK",1000));			//建立HTTP连接
			sprintf((char *)cmd,"AT+HTTPACTION=0,%d,%d",num_offset,num_size);
			while(n58_send_cmd(cmd,"OK",100));		//执行http get 请求
			num_offset+=num_size;
			USART2_RX_STA=0;
			while(--time)
			{
				delay_ms(10);
				if(USART2_RX_STA&0X8000)
				{
					data=n58_check_cmd("+HTTPRECV:");
					if(!num_length)
						num_length=atoi(strstr(strstr((const char*)USART2_RX_BUF,"Content-Range:"),"/")+1);
					USART2_RX_STA=0;
					if(data)
					{
						strcpy(WebTemp,data);
					}
					break;
				}
			}
		}while((num_offset)<(num_length-1));
	}
	return data;
}
//HTTP HEAD 请求方法
u8* n58_http_head(u8 *url,u8 *port)
{
	u8 cmd[40]={0};
	u8 *data=0,time=100;
	sprintf((char *)cmd,"AT+HTTPPARA=url,%s",url);			//设置域名
	while(n58_send_cmd(cmd,"OK",100));
	sprintf((char *)cmd,"AT+HTTPPARA=port,%s",port);		//设置端口号
	while(n58_send_cmd(cmd,"OK",100));
	while(n58_send_cmd("AT+HTTPSETUP","OK",1000));			//建立HTTP连接
	while(n58_send_cmd("AT+HTTPACTION=1","OK",100));				//执行http get 请求
	USART2_RX_STA=0;
	while(--time)
	{
		delay_ms(10);
		if(USART2_RX_STA&0X8000)
		{
			data=n58_check_cmd("+HTTPRECV:");
			USART2_RX_STA=0;
			if(data)
			{
				strcpy(WebTemp,data);
			}
			break;
		}
	}
	return data;
}

