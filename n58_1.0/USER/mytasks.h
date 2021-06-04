#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "usart2.h"
#include "n58.h"

extern void start_task(void *pdata);
#define START_TASK_PRIO      		10
