#include "exti.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x_exti.h"
#include "vofa.h"
#include "encoder.h"
#include "State.h"
#include "usart.h"
#include "stm32f10x_usart.h"
#include "stdlib.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序
void EXTIX_Init(void)
{
 
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

	//GPIOE.2 中断线以及中断初始化配置   下降沿触发
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);

	EXTI_InitStructure.EXTI_Line=EXTI_Line13;	//KEY2
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器



 //GPIOE.3	  中断线以及中断初始化配置 下降沿触发 //KEY1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource14);
	EXTI_InitStructure.EXTI_Line=EXTI_Line14;
	EXTI_Init(&EXTI_InitStructure);	  	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器




	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			//使能按键WK_UP所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 



 
}

//外部中断0服务程序 
void EXTI15_10_IRQHandler(void)
{
	u8 i = 0;
	u32 Databuffe[6];
//		zeroFlag = 0;
//		zeroCount++;
//		if(zeroCount == 1) 
//		Wheel_Data.Distance_travelled = 0;
	//	MOTOR_ENA_B = 0;
		
		Wheel_Data.encoder_CNT= Read_Encoder(4);
		if(Wheel_Data.encoder_CNT > 0)
			Wheel_Data.Sum_CNT = Wheel_Data.encoder_CNT +	Read_EncoderFlag*65536;
		else
			Wheel_Data.Sum_CNT = Wheel_Data.encoder_CNT -	Read_EncoderFlag*65536;		
			
		Wheel_Data.Distance_travelled = (abs(Wheel_Data.Sum_CNT));// 	里程	
		
		OldTrainMileage = TrainMileage;
		TrainMileage = Wheel_Data.Distance_travelled;
		Databuffe[0] = 0xfa;
		Databuffe[1] = (TrainMileage-OldTrainMileage)>>24;
		Databuffe[2] = (TrainMileage-OldTrainMileage)>>16;
		Databuffe[3] = (TrainMileage-OldTrainMileage)>>8;
		Databuffe[4] = (TrainMileage-OldTrainMileage);
		Databuffe[5] = 0xfb;
	
		for(i=0;i<6;i++)
		{		
			while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
			USART_SendData(USART1,Databuffe[i]); 		
		}

//	printf("TrainMileage:%d\r\n",TrainMileage-OldTrainMileage);
	EXTI_ClearITPendingBit(EXTI_Line13); //清除LINE0上的中断标志位  
	EXTI_ClearITPendingBit(EXTI_Line14); //清除LINE0上的中断标志位
}
 

 
