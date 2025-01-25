/*
 * main.h
 *
 *  Created on: Jan 25, 2025
 *      Author: sohan
 */

#ifndef MAIN_H_
#define MAIN_H_


#define MAX_TASKS				4
#define TASK_STACK_SIZE 		1024U
#define SCHED_STACK_SIZE		1024U

#define SRAM_START				0x20000000U
#define SRAM_SIZE				((128) * (1024))
#define SRAM_END				(SRAM_START + SRAM_SIZE)

#define TASK_1_STACK_START		(SRAM_END)
#define TASK_2_STACK_START		(SRAM_END) - (1 * (TASK_STACK_SIZE))
#define TASK_3_STACK_START		(SRAM_END) - (2 * (TASK_STACK_SIZE))
#define TASK_4_STACK_START		(SRAM_END) - (3 * (TASK_STACK_SIZE))
#define SCHED_STACK_START		(SRAM_END) - (4 * (TASK_STACK_SIZE))
#define TICK_HZ					1000U

#define HSI_CLOCK				1600000U
#define SYSTICK_TIM_CLK			HSI_CLOCK

#define DUMMY_XPSR 0x01000000U

#endif /* MAIN_H_ */
