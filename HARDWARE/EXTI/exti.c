#include "exti.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x_exti.h"
//#include "vofa.h"
#include "usart.h"
#include "stm32f10x_usart.h"
#include "stdlib.h"
#include "motor.h"
#include "app.h"
u32 PocketCountQ = 0;
u32 PocketCountH = 0;
u32 PocketCount = 0;
u16 CageNumber = 0;//实时格口号
u32 NFCNUM = 0;
u8 MFSW_Flag = 0;//0--无  1--检测到投放物品
u8 CheckDirction = 0;//0--前进 1--后退
u8 Q_GDSW_Flag = 0;
u8 H_GDSW_Flag = 0;
u8 H_GDSW_Flag_Count;
u8 test1 = 0;
//外部中断0服务程序
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	//GPIOA5 中断线以及中断初始化配置   下降沿触发 //Q_GDSW  前光电
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5); 
//	EXTI_InitStructure.EXTI_Line=EXTI_Line5;	//
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器



//// //GPIOA4	  中断线以及中断初始化配置 下降沿触发 //H_GDSW  后光电
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
//	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	
//	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	
 //GPIOA1	  中断线以及中断初始化配置 下降沿触发 //MFSW  漫反射
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
	EXTI_InitStructure.EXTI_Line=EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		
	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器
	
	
// //GPIOC15	  中断线以及中断初始化配置 下降沿触发 //按键
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource15);
//	EXTI_InitStructure.EXTI_Line=EXTI_Line15;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;		
//	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能Q_GDSW前光电所在的外部中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级1， 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure); 
	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//使能H_GDSW后光电所在的外部中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级1， 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure); 	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//使能MFSW漫反射所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级1， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 	


//	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键所在的外部中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级1， 
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure); 

}

//外部中断5服务程序  Q_GDSW前光电
//void EXTI9_5_IRQHandler(void)
//{
////	delay_ms(10);
//	if(Q_GDSW == 0)
//	{
//		Q_GDSW_Flag = 1;
//		My_Sprintf(0,"QIAN = %d\r\n", 1);
//		if(H_GDSW_Flag)
//		{
//			//PocketCount--;
//			H_GDSW_Flag = 0;
//			Q_GDSW_Flag =0;
//			My_Sprintf(0,"NO = %d\r\n", 1);
//		}
//	}
//	EXTI_ClearITPendingBit(EXTI_Line5); //清除LINE0上的中断标志位  
//}

//外部中断4服务程序  H_GDSW后光电
void EXTI4_IRQHandler(void)
{

	
	EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE0上的中断标志位  
}
 
//外部中断1服务程序  MFSW漫反射
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1)!=RESET)			//标志位被值位（产生中断）
	{	
		if(ScanCodeMode == 0)
		{
			delay_ms(2);
			if(MFSW == 0 && TrainState != 0) 
			{
				if(BasketErrorFlag == 0)//故障不申请地址
				{
					if(EnableApplicationAddr == 1)
					{
						if(DeliverTaskFlag == 0 && TrainState != 1)
						{
							test1 ++; 
							Applictiao_ADDR_Flag &= ~0x0f;//
							Applictiao_ADDR_Flag |= 0x01;//申请地址	
						}			
					}
				}
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE0上的中断标志位 
	}		
}



//外部中断15服务程序  MFSW漫反射
void EXTI15_10_IRQHandler(void)
{
//	if(KEY1 == 0)
//	{
//		iQ = 0;jQ = 0;iH = 0;jH = 0;iQOP = 0;jQOP = 0;iHOP = 0;jHOP = 0;//超时检测清0		
//		if(BasketFlagQ ==0)
//			BasketFlagQ = 1;
//		else if(BasketFlagQ ==2)
//			BasketFlagQ = 3;

//		if(BasketFlagH ==0)
//			BasketFlagH = 1;
//		else if(BasketFlagH ==2)
//			BasketFlagH = 3;
//	}
	EXTI_ClearITPendingBit(EXTI_Line15); //清除LINE0上的中断标志位 
}








 
