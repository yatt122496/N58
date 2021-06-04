#ifndef __N58_H__
#define __N58_H__

#include <stdio.h>
#include "string.h"
#include "usart2.h"
#include "delay.h"
#include "includes.h"


typedef unsigned char u8;
typedef unsigned short u16;

u8* n58_check_cmd(u8 *str);
u8 n58_send_cmd(u8 *cmd,u8 *ack,u16 waittime);
u8 n58_send_data(u8 *data,u8 *ack,u16 waittime);
u8 n58_init(void);
u8 n58_ppp_init(void);
u8* n58_http_get(u8 *url,u8 *port,u8 *offset,u8 *size);
u8* n58_http_head(u8 *url,u8 *port);

extern void led1_task(void *pdata);
#define LED1_TASK_PRIO      		8
#define LED1_STK_SIZE  		    		64
extern OS_STK LED1_TASK_STK[LED1_STK_SIZE];

extern u8 WebTemp[1024];

#endif
