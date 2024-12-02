#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "stdio.h"
 
 u16 ADC_ConvertedValue[16] = {0};

 u8 TrainBasketID = 0;
 u16 KeyUpCount = 0;
 u8 KeyValue = 0;
 u8 KeyUpFlag = 0;
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	
////////////////////////////IN//////////////////////////////////
	
	

	
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//	急停按键
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);//
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//	拨码开关
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);//
			
////////////////////////////OUT//////////////////////////////////	
	


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//电源-
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIO_ResetBits(GPIOB,GPIO_Pin_4);
	GPIO_SetBits(GPIOB,GPIO_Pin_4); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//电源+
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6); 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_14);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//WIFI_WAKE
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_15); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;// WIFI_RST
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_12);//
	
	//GPIO_SetBits(GPIOB,GPIO_Pin_9); 


	
	
	
//====================ADC IO===========================

	//M2
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
	
	
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
/* Enable AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
//	TrainBasketID = TrainBasket;//获取车厢号
			
}




u8 g_boma_Key_CANID;
u8 gOpenledFlag =0;
u16 f_20ms = 0;



void led_Ring_Only_One(void)
{
	if(gOpenledFlag == TRUE)
	{
		if(f_20ms < 20)
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);
		}
		else if((f_20ms>20)&&(f_20ms<600))
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_7);
		}
		else if(f_20ms>600)
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_7);
			f_20ms = 0;
			gOpenledFlag = FALSE;
	
		}

	}
}


void ADC_Configuration(void)
{
	DMA_InitTypeDef   DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	//ADC config
	/* DMA Channel1 Configuration ----------------------------------------------*/
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)&ADC_ConvertedValue;
	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize         = 4;
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;

	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* ADC1 configuration ------------------------------------------------------*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 4;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel14 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);		//
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_55Cycles5);	

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}


//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按
	//按键1
//	if(key_up&&(KEY1==0 || KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0 || KEY5 == 0 || KEY6 == 0 || KEY7 == 0 || KEY8 == 0 || KEY9 == 0 || KEY10 == 0 || KEY11 == 0 || KEY12 == 0))
//	{
//		delay_ms(10);//去抖动 
//		key_up=0;
//		if(KEY1==0)return 1; 
//		if(KEY2==0)return 2; 
//		if(KEY3==0)return 3; 
//		if(KEY4==0)return 4; 
//		if(KEY5==0)return 5; 
//		if(KEY6==0)return 6; 
//		if(KEY7==0)return 7; 
//		if(KEY8==0)return 8; 
//		if(KEY9==0)return 9; 
//		if(KEY10==0)return 10; 
//		if(KEY11==0)return 11; 
//		if(KEY12==0)return 12; 
//	}else if(KEY1==1 && KEY2==1 && KEY3==1 && KEY4==1 && KEY5==1 && KEY6==1 && KEY7==1 && KEY8==1 && KEY9==1 && KEY10==1 && KEY11==1 && KEY12==1)key_up=1; 
	
	if(key_up&&(KEY1==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY1==0)return 1; 
		
	}else if(KEY1==1)key_up=1; 
	
	
 	return 0;// 无按键按下
}


void KeyScan(void)
{
	if(KEY1 == 0)
	{
		KeyUpCount++;	
		KeyUpFlag = 1;
	}
	else
	{
		if(KeyUpFlag ==1)
		{
			if(KeyUpCount >2 && KeyUpCount <=500)
			{
				KeyValue = 1;
			}
			else if(KeyUpCount >500 && KeyUpCount <=1500) 
			{
				KeyValue = 2;
			}
			else if(KeyUpCount >=2000) 
			{
				KeyValue = 3;
				KeyUpCount = 2000;
			}		
			
			KeyUpCount = 0;
			KeyUpFlag = 0;
		}
	}
	
}




