#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//#define LED0 PAout(8)	// PA8
//#define LED1 PDout(2)	// PD2	


#define LED3 PBout(3)	// PB2	
void LED_Init(void);//初始化
void ADC_Configuration(void);

extern u16 ADC_ConvertedValue[16];
#define ADC1_DR_Address    ((u32)0x4001244C)

#define BLINK_ON GPIO_SetBits(GPIOB, GPIO_Pin_14) //尾灯开
#define BLINK_OFF GPIO_ResetBits(GPIOB, GPIO_Pin_14)
		
#define		car_go_led_on  GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define		car_go_led_off  GPIO_ResetBits(GPIOB, GPIO_Pin_15)

#define		car_back_led_on  GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define		car_back_led_off  GPIO_ResetBits(GPIOA, GPIO_Pin_8)

#endif
