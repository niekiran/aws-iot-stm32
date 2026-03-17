/*
 * Copyright Amazon.com, Inc. and its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License. See the LICENSE accompanying this file
 * for the specific language governing permissions and limitations under
 * the License.
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include <assert.h>
#if defined( __ICCARM__) || defined(__GNUC__) || defined(__CC_ARM)
  #include <stdint.h>
  extern uint32_t SystemCoreClock;
#endif

#define configMINIMAL_STACK_SIZE             ( 128 )
#define configENABLE_FPU                      1
#define configTOTAL_HEAP_SIZE                ( ( size_t ) ( 40  * 1024 ) )
#define configMAX_PRIORITIES                 ( 32 )
#define configUSE_PREEMPTION                 1
#define configCPU_CLOCK_HZ                   ( SystemCoreClock )
#define configUSE_IDLE_HOOK                  0
#define configUSE_TICK_HOOK                  0
#define configUSE_16_BIT_TICKS               0
#define configUSE_RECURSIVE_MUTEXES          1
#define configUSE_MUTEXES                    1
#define configUSE_TIMERS                     1
#define configUSE_COUNTING_SEMAPHORES        1
#define configMAX_TASK_NAME_LEN              ( 12 )
#define configTIMER_TASK_PRIORITY            ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH             20
#define configTIMER_TASK_STACK_DEPTH         ( configMINIMAL_STACK_SIZE * 2 )
#define configSUPPORT_STATIC_ALLOCATION      1
#define configSTACK_DEPTH_TYPE               uint32_t
#define configTICK_RATE_HZ                   ( 1000 )

#define configCHECK_FOR_STACK_OVERFLOW       0

#define INCLUDE_vTaskPrioritySet             1
#define INCLUDE_uxTaskPriorityGet            1
#define INCLUDE_vTaskDelete                  1
#define INCLUDE_vTaskSuspend                 1
#define INCLUDE_xTaskDelayUntil              1
#define INCLUDE_vTaskDelay                   1
#define INCLUDE_xTaskGetIdleTaskHandle       1
#define INCLUDE_xTaskAbortDelay              1
#define INCLUDE_xQueueGetMutexHolder         1
#define INCLUDE_xTaskGetHandle               1
#define INCLUDE_uxTaskGetStackHighWaterMark  1
#define INCLUDE_uxTaskGetStackHighWaterMark2 1
#define INCLUDE_eTaskGetState                1
#define INCLUDE_xTimerPendFunctionCall       1
#define INCLUDE_xTaskGetSchedulerState       1
#define INCLUDE_xTaskGetCurrentTaskHandle    1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
  /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
  #define configPRIO_BITS           __NVIC_PRIO_BITS
#else
  #define configPRIO_BITS           4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY     0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY     ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY  ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler SVC_Handler
#define xPortPendSVHandler PendSV_Handler
#define xPortSysTickHandler SysTick_Handler
#endif
