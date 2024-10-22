/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "include.h"

 
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
 
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
uint32_t Touch_Len_Cnt = 0;
uint16_t Power_Ctrl_Cnt = 0;

void SysTick_Handler(void)
{
	Delay_ms_Cnt++;
	if(Delay_ms_Cnt >= 1000)
	{
		Delay_ms_Cnt = 0;
		Delay_s_Cnt++;
		if(Delay_s_Cnt >= 0xffff)
		{
			Delay_s_Cnt = 0;
		}
	}
#ifdef TEST_0_100_LED_TUBE_
	Test_100Km_Timer++;
	if(Test_100Km_Timer >= 0xffff)
		Test_100Km_Timer = 0;
#endif
//	Touch_Len_Cnt++;
//	if(Touch_Len_Cnt >= 0xffffffff)
//	{
//		Touch_Len_Cnt = 0;
//	}
	Power_Ctrl_Cnt++;
	if(Power_Ctrl_Cnt >= 10000)
	{
		Power_Ctrl_Cnt = 0;
		
	}
	if(Power_Ctrl_Cnt % 5000 == 0)
	{
		
	}
	
	if(Power_Ctrl_Cnt % 1000 == 0)
	{
		
	}
	if(Power_Ctrl_Cnt % 500 == 0)
	{
		
	}
	if(Power_Ctrl_Cnt % 250 == 0)
	{
		
	}
	if(Power_Ctrl_Cnt % 50 == 0)
	{
		
	}
	if(Power_Ctrl_Cnt % 20 == 0)
	{

	}
	

}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
