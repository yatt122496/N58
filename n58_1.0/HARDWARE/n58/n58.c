#include "n58.h"

//��4gģ�鷢������󣬼����յ���Ӧ��
//str���ڴ���Ӧ����
//����ֵ��0��û�еõ��ڴ���Ӧ����
//				�������ڴ�Ӧ������λ�ã�str��λ�ã�
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
//��4gģ�鷢������
//cmd�����͵������ַ���
//acd���ڴ���Ӧ���������Ϊ�գ����ʾ����Ҫ�ȴ�Ӧ��
//waittime���ȴ�ʱ�䣨��λ��10ms)
//����ֵ��0�����ͳɹ����õ����ڴ���Ӧ������
//				1������ʧ��
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
//��4gģ�鷢������
//data�����͵����ݣ�����Ҫ��ӻس��ˣ�
//acd���ڴ���Ӧ���������Ϊ�գ����ʾ����Ҫ�ȴ�Ӧ��
//waittime���ȴ�ʱ�䣨��λ��10ms)
//����ֵ��0�����ͳɹ����õ����ڴ���Ӧ������
//				1������ʧ��
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

//n58��ʼ������
u8 n58_init(void)
{
	u8 i,num_csq=0;
	OSTimeDlyHMSM(0,0,10,0);
	if(n58_send_cmd("AT","OK",100))							//���ڽ�ͨ���ԣ�������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("ATE0","OK",100))							//���÷��Ͳ����ԣ�������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("ATE0","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CGSN","OK",100))							//ģ��IMEI��������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CGSN","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CGMR","OK",100))							//ģ��̼��汾��������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CGMR","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CPIN?","READY",100))							//SIM��״̬��������"READY"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CPIN?","READY",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CIMI","OK",100))							//SIM��IMSI��������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CIMI","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	i=5;
	while(i--)							//�ź�ǿ��"������+CSQ: <signal>,<ber>,��0<signal<=31ʱ������һ��
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
	
	if(n58_send_cmd("AT+CREG=2","OK",100))							//�����ϱ�����ע��С����Ϣ��������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CREG=2","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CREG?","+CREG: 2,1",100)&&n58_send_cmd("AT+CREG?","+CREG: 2,5",100))							//��ѯ����ע�����,������"+CREG: 2,1"����"+CREG: 2,5"�ֶΣ��������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(n58_send_cmd("AT+CREG?","+CREG: 2,1",100)&&n58_send_cmd("AT+CREG?","+CREG: 2,5",100)) break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CREG=0","OK",100))							//�����ϱ�����ע��С����Ϣ��������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CREG=0","OK",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CGATT?","+CGATT: 1",100))							//�����ϱ�����ע��С����Ϣ��������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CGATT?","+CGATT: 1",100))break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",100))					//����APN��������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",100)) break;
			if(i==1) return 1;
		}
	}
	
	if(n58_send_cmd("AT+XGAUTH=1,1,\"gsm\",\"1234\"","OK",100))					//���������֤������������"OK"���������һ������֮��������
	{
		for(i=5;i>0;i--)
		{
			if(!n58_send_cmd("AT+XGAUTH=1,1,\"gsm\",\"1234\"","OK",100)) break;
			if(i==1) return 1;
		}
	}
	
	return 0;
}

//ppp����ͨ���������
u8 n58_ppp_init(void)
{
	u8 i;
	if(n58_send_cmd("AT+XIIC=1","OK",100))							//����PPP���ţ�������"OK"���������һ������֮�������ţ�1s/�Σ����3�Σ�3�λ�����������
	{
		for(i=3;i>0;i--)
		{
			if(!n58_send_cmd("AT+XIIC=1","OK",1000))break;
			if(i==1)return 1;
		}
	}
	
	if(n58_send_cmd("AT+XIIC?","+XIIC:    1",100))				//��ѯPPP���ӽ�����������������"+XIIC:	1",�������һ������֮ѭ����ѯ��1s/�Σ����30�Ρ�
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
///HTTP GET ���󷽷�
u8* n58_http_get(u8 *url,u8 *port,u8 *offset,u8 *size)
{
	u8 cmd[40]={0},segmentDownload;
	u8 *data=0,time=100;
	u32 num_offset,num_size,num_length=0;
	sprintf((char *)cmd,"AT+HTTPPARA=url,%s",url);			//��������
	while(n58_send_cmd(cmd,"OK",100));
	sprintf((char *)cmd,"AT+HTTPPARA=port,%s",port);		//���ö˿ں�
	while(n58_send_cmd(cmd,"OK",100));
	while(n58_send_cmd("AT+HTTPSETUP","OK",1000));			//����HTTP����
	if(offset&&size)
		segmentDownload=1;		
	else
		segmentDownload=0;
	if(!segmentDownload)																//���ֶ�����HTTP
	{
		sprintf((char *)cmd,"AT+HTTPACTION=0");
		while(n58_send_cmd(cmd,"OK",100));		//ִ��http get ����
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
		num_offset=(u32)atoi((const char *)offset);						//�ַ���תΪ����
		num_size=(u32)atoi((const char *)size);
		do																										//�ֶ�����HTTP
		{
			delay_ms(200);
			while(n58_send_cmd("AT+HTTPSETUP","OK",1000));			//����HTTP����
			sprintf((char *)cmd,"AT+HTTPACTION=0,%d,%d",num_offset,num_size);
			while(n58_send_cmd(cmd,"OK",100));		//ִ��http get ����
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
//HTTP HEAD ���󷽷�
u8* n58_http_head(u8 *url,u8 *port)
{
	u8 cmd[40]={0};
	u8 *data=0,time=100;
	sprintf((char *)cmd,"AT+HTTPPARA=url,%s",url);			//��������
	while(n58_send_cmd(cmd,"OK",100));
	sprintf((char *)cmd,"AT+HTTPPARA=port,%s",port);		//���ö˿ں�
	while(n58_send_cmd(cmd,"OK",100));
	while(n58_send_cmd("AT+HTTPSETUP","OK",1000));			//����HTTP����
	while(n58_send_cmd("AT+HTTPACTION=1","OK",100));				//ִ��http get ����
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

