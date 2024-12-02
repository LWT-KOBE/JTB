#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "can.h"
#include "include.h" 


#define TrainBasket      (u8)((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)<<4)\
														 |(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)<<3)\
														 |(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)<<2)\
														 |(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)<<1)\
															|GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_12))



#define PowerZ PBout(4)   	
#define PowerF PBout(5) 

#define GLed PBout(12)   	
#define RLed PBout(13) 

#define KEY1 PAin(5) 
#define KEY2 PCin(13) 
#define KEY3 PAin(11) 
#define KEY4 PCin(14) 
#define KEY5 PAin(8) 
#define KEY6 PAin(4) 
#define KEY7 PBin(15) 
#define KEY8 PAin(5) 
#define KEY9 PBin(14) 
#define KEY10 PAin(6) 
#define KEY11 PBin(13) 
#define KEY12 PAin(7) 

#define HCIN PBin(7) 
#define HOIN PBin(5) 

#define QCIN PBin(6) 
#define QOIN PBin(4) 

#define Q_GDSW PAin(5) 
#define H_GDSW PAin(4) 

#define MFSW  PAin(1) 

#define ADC1_DR_Address    ((u32)0x4001244C)

#define KEY0_PRES 	1	//KEY0按下
#define KEY1_PRES	2	//KEY1按下
#define KEY2_PRES	3	//KEY2按下
#define WKUP_PRES   4	//KEY_UP按下(即WK_UP/KEY_UP)

void ADC_Configuration(void);
void KeyScan(void);
u8 KEY_Scan(u8 mode);
extern u16 ADC_ConvertedValue[16];

void KEY_Init(void);//IO初始化
void KEY_GET_CAN_ID(u8 *UT_CAN_ID);	

void led_Ring_Only_One(void);
extern u8 gOpenledFlag;
extern u16 f_20ms ;
extern u8 TrainBasketID;
extern u8 KeyValue;
extern u8 KeyUpFlag;

extern u8 key_flag;

#endif
