#include "motor.h"
#include "timer.h"
#include "app.h"

u32 MOTOR1_CUR = 0;
u32 MOTOR2_CUR = 0;
u8 BasketStateH = 0;//0---关闭  1---打开  15---半开  255---故障
u8 BasketStateQ = 0;//0---关闭  1---打开  15---半开	255---故障

//u8 key1state = 0;

u8 BasketFlagH = 0;//按键切换状态位  1--开斗   3关斗
u8 BasketFlagQ = 0;//按键切换状态位  1--开斗   3关斗
u16 iii,jjj = 0;
u16 iQ,jQ = 0;
u16 iH,jH = 0;
u16 iQOP,jQOP = 0;
u16 iHOP,jHOP = 0;

u8 BasketErrorFlag = 0;//0--无错误  bit0--车斗异常、半开超时   bit1--手动置错   bit2--关斗超时   bit3--开斗超时   
											 //	bit4--24C02错误  bit5--屏蔽车厢、手动置错  bit6--回收状态  bit7--CAN状态
void MotorH_Forward (u16 Speed)
{	
	PAout(7) = 0;
	OC2_Cmd(DISABLE);
//	delay_ms(1);
	OC1_Cmd(ENABLE);
	TIM_SetCompare1(TIM3,Speed);	
}

void MotorH_Back (u16 Speed)
{	
	Speed = 7200-Speed;
	PAout(6) = 0;
	OC1_Cmd(DISABLE);
//	delay_ms(1);
	OC2_Cmd(ENABLE);
	TIM_SetCompare2(TIM3,Speed);	
}


void MotorQ_Forward (u16 Speed)
{	
	OC4_Cmd(DISABLE);
	PBout(1) = 0;
//	delay_ms(1);
	OC3_Cmd(ENABLE);
	TIM_SetCompare3(TIM3,Speed);	
}

void MotorQ_Back (u16 Speed)
{	
	Speed = 7200-Speed;
	OC3_Cmd(DISABLE);
	PBout(0) = 0;
//	delay_ms(1);
	OC4_Cmd(ENABLE);
	TIM_SetCompare4(TIM3,Speed);	
}


void MotorH_Stop(void)
{	
	
	PAout(6) = 0;
	PAout(7) = 0;	
	OC1_Cmd(DISABLE);
	OC2_Cmd(DISABLE);	
}

void MotorQ_Stop(void)
{	
	PBout(0) = 0;
	PBout(1) = 0;
	OC3_Cmd(DISABLE);
	OC4_Cmd(DISABLE);	
}


void Basket_CloseQ (u16 speed)
{			
	if(BasketStateQ == 15)
	{
		iQ++;
		if(iQ<0x1e0)
			MotorQ_Back(5500);	
		
		else if(iQ >=0x1e0)//关斗超时 
		{
			MotorQ_Stop();
			BasketErrorFlag |= 0x04;
			iQ = 0x4F0;
		}		
	}
	if(QOIN == 0)
	{
		jQ++;
		if(jQ<0x1e0)
			MotorQ_Back(4000);
		else if(jQ >=0x1e0)//关斗超时
		{
			MotorQ_Stop();
			BasketErrorFlag |= 0x04;
			jQ = 0x4F0;
			
		}		
	}
	if(QCIN == 0) 
	{
		MotorQ_Stop();		
		BasketFlagQ = 0;
		BasketErrorFlag &= ~0x04;
	}
}

void Basket_CloseH (u16 speed)
{	
	if(BasketStateH == 15)
	{
		iH++;
		if(iH<0x1e0)
		{
			MotorH_Back(5500);
			My_Sprintf(0,"6 = %d\r\n", 6);
		}
		else if(iH >=0x1e0)//关斗超时
		{
			MotorH_Stop();
			BasketErrorFlag |= 0x04;
			iH = 0x4F0;
			My_Sprintf(0,"7 = %d\r\n", 7);
		}		
	}		
	if(HOIN == 0)
	{
		jH++;
		if(jH < 0x1e0)
		{
			MotorH_Back(4000);
			My_Sprintf(0,"8 = %d\r\n", 8);
		}			
		else if(jH >=0x1e0)//关斗超时
		{
			MotorH_Stop();
			BasketErrorFlag |= 0x04;
			jH = 0x4F0;
			My_Sprintf(0,"9 = %d\r\n", 9);
		}		
	}
	if(HCIN == 0) 
	{
		MotorH_Stop();		
		BasketFlagH = 0;
		BasketErrorFlag &= ~0x04;
		My_Sprintf(0,"10 = %d\r\n", 10);
	}
}

void Basket_OpenH (u16 speed)
{		
	if(BasketStateH == 15)
	{
		iHOP++;
		if(iHOP < 0x1e0)
			MotorH_Forward(2000);
		else if(iHOP >=0x1e0)//开斗超时
		{
			MotorH_Stop();
			BasketErrorFlag |= 0x08;
			iHOP = 0x4F0;
		}	
	}
	if(HCIN == 0 )
	{
		jHOP++;
		if(jHOP < 0x1e0)
			MotorH_Forward(1400);
		else if(jHOP >=0x1e0)//开斗超时
		{
			MotorH_Stop();
			BasketErrorFlag |= 0x08;
			jHOP = 0x4F0;
		}		
	}
	if(HOIN == 0)//此处是否清除开斗超时故障？
	{
		MotorH_Stop();
		BasketFlagH = 2;
		BasketErrorFlag &= ~0x08;
	}
}

 
void Basket_OpenQ (u16 speed)
{	
	if(BasketStateQ == 15)//半开状态--> 完全打开
	{
		iQOP++;
		if(iQOP < 0x1e0)
		{
			MotorQ_Forward(2000);
			My_Sprintf(0,"1 = %d\r\n", 1);
		}
		else if(iQOP >= 0x1e0)//开斗超时
		{
			MotorQ_Stop();
			BasketErrorFlag |= 0x08;
			iQOP = 0x4F0;
			My_Sprintf(0,"2 = %d\r\n", 2);
		}
	}
	if(QCIN == 0 )//完全关闭-->打开
	{
		jQOP++;
		if(jQOP < 0x1e0)
		{
			MotorQ_Forward(1400);	
			My_Sprintf(0,"3 = %d\r\n", 3);
		}
		if(jQOP >= 0x1e0)//开斗超时
		{
			MotorQ_Stop();
			BasketErrorFlag |= 0x08;
			jQOP = 0x4F0;
			My_Sprintf(0,"4 = %d\r\n", 4);
		}			
	}
	if(QOIN == 0)//关闭状态位防止抱死
	{
		MotorQ_Stop();
		BasketFlagQ = 2;
		BasketErrorFlag &= ~0x08;
		My_Sprintf(0,"5 = %d\r\n", 5);
	}					
}

 
void CheckBasketState(void)
{	
////////////////车斗姿态检测/////////////////		
		if(QCIN == 0 && QOIN == 1)
		{
			BasketStateQ = 0;//0---关闭  1---打开
			BasketErrorFlag &= ~0x03;
		}
		else if(QCIN == 1 && QOIN == 0)
		{
			BasketStateQ = 1;	
			BasketErrorFlag &= ~0x03;
		}
		else if(QCIN == 1 && QOIN == 1)
			BasketStateQ = 15;
		else
			BasketStateQ = 0xff;
		
		
		
		if(HCIN == 0 && HOIN == 1)
			BasketStateH = 0;
		else if(HCIN == 1 && HOIN == 0)
			BasketStateH = 1;
		
		else if(HCIN == 1 && HOIN == 1)
			BasketStateH = 15;
		else
			BasketStateH = 0xff;				
}

void BasketContral (void)
{
//		if(KEY1 == 0)
//		{
//			if(BasketFlagQ ==0)
//				BasketFlagQ = 1;
//			else if(BasketFlagQ ==2)
//				BasketFlagQ = 3;

//			if(BasketFlagH ==0)
//				BasketFlagH = 1;
//			else if(BasketFlagH ==2)
//				BasketFlagH = 3;							
//		}	
	
		if(KEY1 == 0)
			{iQ = 0;jQ = 0;iH = 0;jH = 0;iQOP = 0;jQOP = 0;iHOP = 0;jHOP = 0;My_Sprintf(0,"11 = %d\r\n", 11);}//超时检测清0 
	//	if(BasketErrorFlag == 0)
		{	
			if(BasketFlagQ ==1)
			{			
				Basket_OpenQ(1);		
			}
			else if(BasketFlagQ == 3)
			{
				Basket_CloseQ(1);								
			}		
			
			if(BasketFlagH ==1)
			{
				Basket_OpenH(1);		
			}
			else if(BasketFlagH == 3)
			{
				Basket_CloseH(1);								
			}	
		}		
}

void ErrorCheck (void)
{
		if(BasketStateQ == 0xff || BasketStateH == 0xff)//error
		{			
			BasketErrorFlag |= 0x01;
		}		
		if(BasketStateQ == 15)//半开状态持续报警灯亮
		{
			iii++;
			if(iii >= 0x7530)
			{
				if(BasketStateQ == 15)
					BasketErrorFlag |= 0x01;
			}
		}
		else if(BasketStateQ == 1 || BasketStateQ == 0)
		{
			iii = 0;	
		}
		
		if(BasketStateH == 15)
		{
			jjj++;
			if(jjj >= 0x7530)
			{
				if(BasketStateH == 15)
					BasketErrorFlag |= 0x01;
			}
		}	
		else if(BasketStateH == 1 || BasketStateH == 0)
		{
			jjj = 0;	
		}
		
		if(BasketErrorFlag)
			RLed = 0;
		else
			RLed = 1;
		
		if(DeliverTaskFlag == 1)
			GLed = 0;
		else
			GLed = 1;
		
		if(CanLiveFlag ==0)//CAN不在线
			BasketErrorFlag |= 0x80;
		else
			BasketErrorFlag &= ~0x80;			
}








