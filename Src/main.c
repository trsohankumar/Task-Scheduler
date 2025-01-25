/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#include<stdio.h>
#include<stdint.h>
#include"main.h"

void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);
void init_systick_timer(uint32_t tick_hz);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
__attribute__((naked)) void switch_sp_to_psp(void);
void init_task_stack(void);


uint32_t psp_of_tasks[MAX_TASKS] = {TASK_1_STACK_START, TASK_2_STACK_START, TASK_3_STACK_START, TASK_4_STACK_START};
uint32_t task_handlers[MAX_TASKS];

int main(void)
{

	// initialize the scheduler
	init_scheduler_stack(SCHED_STACK_START);

	// initialize all the stack framses
	init_task_stack();

	init_systick_timer(TICK_HZ);


	switch_sp_to_psp();


	task1_handler();

    /* Loop forever */
	for(;;);
}


void task1_handler(void) {

	for(;;){

		printf("This is Task1\n");
	}
}

void task2_handler(void) {

	for(;;){

		printf("This is Task2\n");
	}
}

void task3_handler(void) {

	for(;;){

		printf("This is Task3\n");
	}
}

void task4_handler(void) {

	for(;;){

		printf("This is Task4\n");
	}
}


void init_systick_timer(uint32_t tick_hz) {

	// accessing the systic registers
	uint32_t * pSYST_RVR = (uint32_t *)0xE000E014;
	uint32_t * pSYST_CSR = (uint32_t *)0xE000E010;

	uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz) - 1;


	// clear the value of SVR
	*pSYST_RVR &= ~(0x00FFFFFFFF);

	// load the value into the SVR register
	*(pSYST_RVR) |= count_value;

	// Set the clock to the system timer from the processor clock
	*pSYST_CSR |= (1 << 2);
	// Enable exception setting TICKINT  so that exception handler is called
	*pSYST_CSR |= (1 << 1);
	// Enable the counter
	*pSYST_CSR |= (1 << 0);
}

__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack)
{

	// accessing MSP through inline assembly
	__asm volatile("MSR MSP, %0": : "r" (sched_top_of_stack) : );

	// go back to main
	__asm volatile("BX LR");
}


void init_task_stack(void) {

	// initialize 4 tasks
	uint32_t *psp;

	for (int i = 0; i < MAX_TASKS ; i++)
	{
		psp = (uint32_t *) psp_of_tasks[i];

		// full descending stack so we decrement the pointer first
		(*psp)--;

		// first value is xPsr
		*psp = DUMMY_XPSR;


		// for PC
		psp--;
		*psp = (uint32_t *)task_handlers[i];


		// for LR
		psp--;
		*psp = 0xFFFFFFFD;


		for (int j = 0; j < 13 ; j++) {

			psp--;
			*psp = 0;
		}


		psp_of_tasks[i] = (uint32_t)psp;


	}
}

uint8_t current_task = 0;

uint32_t get_psp_value(void) {

	return psp_of_tasks[current_task];
}

__attribute__((naked)) void switch_sp_to_psp(void) {

	// save LR value to prevent corruption
	__asm volatile ("PUSH {LR}");
	// initialize the psp with TASK 1 stack start address

	__asm volatile("BL get_psp_value");

	// the intial stack address is now stored in RO
	__asm volatile ("MSR PSP, R0");

	// restore LR value
	__asm volatile ("POP {LR}");

	// change SP to PSP using Control Register
	__asm volatile ("MOV R0, 0x02");
	__asm volatile ("MSR CONTROL, R0");

	// return to main
	__asm volatile("BX LR");

}

void save_psp_value(uint32_t current_psp_value)
{

	psp_of_tasks[current_task] = current_psp_value;
}

void update_next_task(void){

	// simulate the next task using round robin
	current_task++;
	current_task %= MAX_TASKS;
}

__attribute__((naked)) void SysTick_Handler(void) {

	// save the context of the current task

	// get the current task PSP value
	__asm volatile ("MRS R0, PSP");

	// using that PSP to store Stack frame 2 (R4 to R11)
	__asm volatile ("STMDB R0!, {R4-R11}");

	// save LR value to prevent corruption
	__asm volatile ("PUSH {LR}");

	// save the current value of PSP
	__asm volatile("BL save_psp_value");

	// Retrieve the context of the next task
	__asm volatile("BL update_next_task");

	// get its past PSP value
	__asm volatile("BL get_psp_value");

	// Retreive SF2
	__asm volatile("LDM R0!, {R4-R11}");

	// update PSP and exit
	__asm volatile("MSR PSP, R0");

	// restore LR value
	__asm volatile ("POP {LR}");


	__asm volatile ("BX LR");
}
