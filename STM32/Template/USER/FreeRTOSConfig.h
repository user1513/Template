/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#include "sys.h"
#include "usart.h"
	
/* Ensure stdint is only used by the compiler, and not the assembler. */
#ifdef __ICCARM__
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif


	
/*ʹ����ռʽ������*/
#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				0
#define configUSE_TICK_HOOK				0
/*ϵͳ��Ƶ168MHz*/
#define configCPU_CLOCK_HZ				((unsigned long) 168000000 )
/*ʱ�ӽ���1KHz,��1ms*/
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
/*����ɹ��û�ʹ�õ�������ȼ������������������5��
��ô�û�����ʹ�õ����ȼ�����0,1,2,3,4�� ������5��*/
/*���ȼ���Խ�����ȼ�Խ��*/
#define configMAX_PRIORITIES			( 16 )

#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 130 )
#define configTOTAL_HEAP_SIZE			( ( size_t ) ( 40 * 1024 ) )
#define configMAX_TASK_NAME_LEN			( 16 )
#define configUSE_TRACE_FACILITY		1
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			0

#define configSUPPORT_DYNAMIC_ALLOCATION 1

/*ʹ�û����ź���,�Ƕ�ֵ�ź������Ӽ�*/
#define configUSE_MUTEXES				1

/*���ó�1ʹ�ܶ��м����ܣ��������������𵽶�����к��ź����������ó�0ȡ�����м�����*/
#define configUSE_QUEUE_SETS			1


#define configQUEUE_REGISTRY_SIZE		8
#define configCHECK_FOR_STACK_OVERFLOW	0
#define configUSE_RECURSIVE_MUTEXES		1
#define configUSE_MALLOC_FAILED_HOOK	0
#define configUSE_APPLICATION_TASK_TAG	0
#define configUSE_COUNTING_SEMAPHORES	1
#define configGENERATE_RUN_TIME_STATS	0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Software timer definitions. */
#define configUSE_TIMERS				1
#define configTIMER_TASK_PRIORITY		( 2 )
#define configTIMER_QUEUE_LENGTH		10
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1

/*
 * Cortex-M�ں�ʹ��8bit���������ȼ�������STM32ֻʹ���˸�4bit����ֵԽС�����ȼ�Խ�ߡ�
 * �����Ĵ�������д��ֵ���õ�ʱ���ǰ���8bit��д�ģ���������д��ʱ����Ҫ����ת������ʽΪ��
 * ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff)�����е�priority�����������õ����������ȼ�
 */
#ifdef __NVIC_PRIO_BITS
	/* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
	#define configPRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       		4        /* 15 priority levels */
#endif

/*============================================== SysTick�ж����ȼ����� ============================================*/
/*
 * �����Ĵ�������д��ֵ���õ�ʱ���ǰ���8bit��д�ģ���������д��ʱ����Ҫ����ת������ʽΪ��
 * ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff)�����е�priority�����������õ����������ȼ������������ʽ֮��õ�����
 * ���������꣺configKERNEL_INTERRUPT_PRIORITY
 * SysTick�����ȼ�����һ������Ϊ��ͣ���0xf �������������ϵͳ��ʵʱ��Ӧ���������������ⲿ�жϿ��Լ�ʱ�ĵõ���Ӧ��
 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			0xf
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/*===========================================�����ε��ж����ȼ�����====================================================*/
/*
 * ��������STM32������Ĵ���basepri�Ĵ�����ֵ�����������жϣ�������basepriֵ�����ȼ����жϽ���ȫ�����Ρ�basepriֻ��4bit��Ч��
 * Ĭ��ֻΪ0����ȫ���ж϶�û�б����Ρ�configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY����Ϊ��5����˼�����ж����ȼ�����5���ж϶������Ρ�
 * �������úõ����ȼ�д���Ĵ�����ʱ���ǰ���8bit��д�ģ���������д��ʱ����Ҫ����ת������ʽΪ��
 * ((priority << (8 - __NVIC_PRIO_BITS)) & 0xff)�����е�priority�����������õ����������ȼ������������ʽ֮��õ��������������꣺
 * configMAX_SYSCALL_INTERRUPT_PRIORITY
 *
 * ��FreeRTOS�У����ж���ͨ������basepri�Ĵ�����ʵ�ֵģ��ص����ж������õ�basepri��ֵ������С��basepriֵ��
 * �ж�FreeRTOS�ǹز����ģ��������ĺô��ǿ���ϵͳ����߿�����Ϊ�Ŀ�����Щ�ǳ���Ҫ���жϲ��ܱ��رգ��ڽ�Ҫ�Ĺ�ͷ���뱻��Ӧ��
 * ����UCOS�У����ж���ͨ������PRIMASK��ʵ�ֵģ�PRIMASK��һ����1�Ķ�����λ��д1����ܳ���NMI��Ӳ fault�������жϡ���UCOS�ر�
 * �ж�֮�󣬼�ʹ������ϵͳ����Ƶķǳ��������ж����˶�����������Ӧ����Ӵ����ж��ӳٵ�ʱ�䣬������������صĳ��ϣ��Ǻ������ͦ���ء�
 * ���UCOS�Ĺ��жϵ���ƣ�FreeRTOS��������Ե����Ի��ܶࡣ
 *
 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5
 
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
	
/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
//#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }	
//����
#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
#define configASSERT(x) if((x)==0) vAssertCalled(__FILE__,__LINE__)	
	
/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
//#define xPortSysTickHandler SysTick_Handler

#endif /* FREERTOS_CONFIG_H */

