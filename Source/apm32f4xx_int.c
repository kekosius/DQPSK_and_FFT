#include "main.h"
#include "apm32f4xx_int.h"

void NMI_Handler(void)
{
}


void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

void SVC_Handler(void)
{
}


void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}

void TMR2_IRQHandler(void)
{
    TMR2_EventCallback();
}

void TMR3_IRQHandler(void)
{
	TMR3_EventCallback();
}

void TMR4_IRQHandler(void)
{
	TMR4_EventCallback();
}

void EINT1_IRQHandler(void)
{
	KEY1_IRQHandler();
} 
