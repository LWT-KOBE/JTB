#include "app.h"
#include "can.h"
#include "key.h"
#include "iap_APP.h"
#include "motor.h"
CanRxMsg RxMessage;
CanTxMsg TxMessage;

u8 LED_Display1;
u8 LED_Display2;

u8 LastDeliverResult_Flag = 0;//0x01上报标志位
u8 Applictiao_ADDR_Flag = 0x00;//0x01申请地址  0x02接收到申请的地址    0x10上报投递结果 0x20--收到回复
u8 ApplicFlag = 0;
u32 DeliverResultACKFlag = 0;//投递结果上报应答标志位21个位代表21个车厢的应答

u32 DeliverAddr = 0;//APP下发的地址
u32 LastDeliverResult = 0;//上一轮投递结果
u8 DeliverTaskFlag = 0;//1--投递任务开始标志

u8 EnableApplicationAddr = 0;//0--disable 1--enable  指定装载位置

u16 HSpeedPocketNum = 0;//高速时投递格口号
u16 LSpeedPocketNum = 0;//低速时投递格口号
u16 CarSpeed = 0;//小火车速度

u8 DeliverMode = 0;//投递模式  0---挡片模式   1---RFID模式
u8 DeliverScussFlag = 15;//投递成功标志位  1--成功   0--失败
u8 DeliverActionFlag = 0;//投递动作标志位  1--执行动作  0--无
u16 Pocket_A_Count,Pocket_A_CountOld,Pocket_A_CountNew = 0;
u8 TrainStateOld,TrainStateNew,TrainState = 0;
u16 SwitchCount1 = 0;
u8 SwitchFlag1 = 0;
u8 SetDateFlag = 0;//设置标志位	
u32 APPSendADDRFlag = 0;//APP下发地址标志位  21个位代表21个车厢下发的地址
u16 OpendoorDelay = 0;
u16 OpendoorDelayCount = 0;
u8 OpendoorDelayFlag = 0;
u32 PocketCaseNum = 0;
u16 DisplayDelayCount = 0;
u8 DisplayFlag = 0;
u16 PulseMotor,PulseMotorOld = 0;
u16 PulseMotorCount = 0;
/////////////////////////////////设置参数/////////////////////////////////
u16 CaseNum = 77;//格口数量  ADDR 0-1
u8 Pocket4 = 12; //装载挡片 ADDR 2
u8 TrainBasketMaxNum = 11;//车厢数量  ADDR 3
u8 ScanCodeMode = 1;//0--手动扫码    1--自动扫码   ADDR 4

u16 RecycleCaseNum = 77;//回收格口  ADDR 15-16
/////////////////////////////////设置参数/////////////////////////////////


void DeliverTask (void)//挡片模式
{
//	if((PocketCount%2)==0)//低速挡片
//		LSpeedPocketNum = PocketCount/2;
//	else//高速挡片
//		HSpeedPocketNum = PocketCount/2 + 1;
	if(DeliverTaskFlag == 0)
	{
		OpendoorDelayFlag = 0;
		OpendoorDelayCount =0;
	}
		
	if(DeliverTaskFlag)//开始任务
	{	
////////////////特殊处理地址下发慢了去回收/////////////////////			
		if(PocketCount >= (CaseNum-2)*2)//
		{
			if(PocketCount > DeliverAddr*2)
				BasketErrorFlag |= 0x40;
		}				
////////////////特殊处理地址下发慢了去回收/////////////////////				
	
///////////////////////异常去回收框///////////////////////////////			
		if(TrainState != 1 && BasketErrorFlag != 0)
		{
			DeliverAddr = CaseNum - 1;
		}
///////////////////////异常去回收框///////////////////////////////			
			
		if(CarSpeed >= 125)//高速投递
		{
			if(DeliverAddr == HSpeedPocketNum)//
			{
				LastDeliverResult = HSpeedPocketNum;
				LastDeliverResult_Flag = 1;//在此处上报投递结果可能不准，挡片没数完可能是错的
				BasketFlagQ = 1;//开斗
				BasketFlagH = 1;			
				ert = 0;
				DeliverAddr = 0;
				DeliverTaskFlag = 0;
				DeliverActionFlag = 1;
			}
		}
		else if(CarSpeed >=95 && CarSpeed <125)
		{
			if(DeliverAddr == HSpeedPocketNum)//
			{	
				OpendoorDelayFlag =1;
				OpendoorDelay = 10/CarSpeed*1000+100;
				if(OpendoorDelayCount >= OpendoorDelay)
				{
					LastDeliverResult = HSpeedPocketNum;
					LastDeliverResult_Flag = 1;//在此处上报投递结果可能不准，挡片没数完可能是错的
					BasketFlagQ = 1;//开斗
					BasketFlagH = 1;			
					ert = 0;
					DeliverAddr = 0;
					DeliverTaskFlag = 0;
					DeliverActionFlag = 1;						
				}
			}				
		}	
		else if(CarSpeed >45 && CarSpeed <95)
		{
			if(DeliverAddr == LSpeedPocketNum)//
			{			
				LastDeliverResult = LSpeedPocketNum;
				LastDeliverResult_Flag =1;//在此处上报投递结果可能不准，挡片没数万可能是错的
				BasketFlagQ = 1;//开斗
				BasketFlagH = 1;
				DeliverTaskFlag = 0;				
				ert = 0;
				DeliverAddr = 0;
				DeliverActionFlag = 1;	
			}

		}			
		else if(CarSpeed <= 45)//低速投递
		{
			if(DeliverAddr == LSpeedPocketNum)//
			{
				if(PulseMotorCount >= 10)
				{
					LastDeliverResult = LSpeedPocketNum;
					LastDeliverResult_Flag =1;//在此处上报投递结果可能不准，挡片没数万可能是错的
					BasketFlagQ = 1;//开斗
					BasketFlagH = 1;
					DeliverTaskFlag = 0;				
					ert = 0;
					DeliverAddr = 0;
					DeliverActionFlag = 1;
				}					
			}  
		}
	
		if(LastDeliverResult == CaseNum-1)
			BasketErrorFlag &= ~0x40;//清除回收状态
	}	

}


void CheckCarCanCmd(void)
{
	if(CAN_ID == (0x53))
	{
//		if(RxRAM0[7] != 0)
//		{
//			TrainStateOld = TrainStateNew;
//			TrainStateNew = TrainState;
//			CarSpeed = RxRAM0[4];
//			TrainState = RxRAM0[5];		
//		}
		if(RxRAM0[7] == 0)//设置参数
		{
			CaseNum = RxRAM0[1] <<8 | RxRAM0[2];
			Pocket4 = RxRAM0[6];
			TrainBasketMaxNum = RxRAM0[5];
			ScanCodeMode = RxRAM0[0];//扫码模式
	//		SetDateFlag = 1;//设置标志位		
		}
		
		else if(RxRAM0[7] == 1)//车厢接收命令
		{
			if(RxRAM0[1] == TrainBasket)
			{
				if(RxRAM0[0] == 3)//屏蔽车厢
					BasketErrorFlag |= 0x20;
				else if(RxRAM0[0] == 4)//解除屏蔽车厢
					BasketErrorFlag &= ~0x20;
				else if(RxRAM0[0] == 5)//去回收框
				{
					BasketErrorFlag |= 0x40;		
					DeliverAddr = CaseNum-1;//回收格口设置
					iQ = 0;jQ = 0;iH = 0;jH = 0;iQOP = 0;jQOP = 0;iHOP = 0;jHOP = 0;//开关斗超时检测清0
					DeliverTaskFlag = 1; 
				}					
			}
			
			DeliverResultACKFlag = RxRAM0[4]<<16 | RxRAM0[5]<<8 | RxRAM0[6];
			if(((DeliverResultACKFlag >> (TrainBasket-1)) & 0x01) == 0x01)
			{
				Applictiao_ADDR_Flag &= ~0xf0;//先清除上报标志位
				Applictiao_ADDR_Flag |= 0x20;//收到上报结果应答标志位		
			}						
		}
	}
	
	
	if(CAN_ID == (0x52))//车头ID
	{
		TrainStateOld = TrainStateNew;
		TrainStateNew = TrainState;	
		TrainState = RxRAM0[1];
		APPSendADDRFlag = RxRAM0[1]<<16 | RxRAM0[2]<<8 | RxRAM0[3];
		CarSpeed = RxRAM0[6];
		Pocket_A_Count = RxRAM0[7];
		
		PulseMotorOld = PulseMotor;
		PulseMotor = RxRAM0[0];
		if(PulseMotorOld != RxRAM0[0])
			PulseMotorCount++;
		
		
		if(TrainState == 0x01)//自检清除任务和投递地址
		{	
			DeliverTaskFlag = 0;
			DeliverAddr = 0;
			Applictiao_ADDR_Flag &= ~0x0f;//清除标志位
			Applictiao_ADDR_Flag |= 0x02;//收到下发地址	
			LastDeliverResult = 0;//0814
		}	
		else if(TrainState == 0)
		{
			PocketCount = 0;//722
			CageNumber = 0;//0814
			Applictiao_ADDR_Flag &= ~0x0f;//清除标志位
			Applictiao_ADDR_Flag |= 0x02;//收到下发地址
			DeliverTaskFlag = 0;
			DeliverAddr = 0;	
			HSpeedPocketNum = 0;
			LSpeedPocketNum = 0;
		}
		
		if(ScanCodeMode == 0)
		{		
			if(Pocket_A_Count >= (Pocket4 + TrainBasket-1) && Pocket_A_Count <= (Pocket4 + TrainBasket +1))//允许申请地址区域  NFCNUM-->RxRAM0[7]  装载点位也是申请地址
				EnableApplicationAddr =1;
			else if(Pocket_A_Count > (Pocket4 + TrainBasket +2))
			{
				EnableApplicationAddr =0;
				Applictiao_ADDR_Flag &= ~0x0f;//清除申请标志位	
				Applictiao_ADDR_Flag |= 0x03;	
			}
			
			if((Applictiao_ADDR_Flag & 0x0f) == 0x01 || (Applictiao_ADDR_Flag & 0x0f) == 0x03)//需要考虑再次回到装载区申请地址是否被上一次下发的记录覆盖（车头在非下发状态时已清除地址）
			{	
				if((RxRAM0[2] & 0x03) == 1)//收到车头的地址
				{
					if(RxRAM0[3] == TrainBasket)
					{
						LastDeliverResult = 0;//0814
						DeliverAddr =  RxRAM0[4]<<8 | RxRAM0[5];//获取投递地址、
						Applictiao_ADDR_Flag &= ~0x0f;//清除标志位
						Applictiao_ADDR_Flag |= 0x02;//收到下发地址
						MFSW_Flag = 0;//取消申请地址状态
						DeliverTaskFlag = 1;	//投递任务形成标志
						iQ = 0;jQ = 0;iH = 0;jH = 0;iQOP = 0;jQOP = 0;iHOP = 0;jHOP = 0;//开关斗超时检测清0
					}
				}
				
			}
				
//			if((RxRAM0[0] & 0x0c) == 0x04)//上报结果收到应答信号
//			{
//				Applictiao_ADDR_Flag &= ~0xf0;//先清除上报标志位
//				Applictiao_ADDR_Flag |= 0x20;//收到上报结果应答标志位						
//			}			
		}
		
		else//自动扫码
		{
			if(Pocket_A_Count >= (4 + TrainBasket+1) && Pocket_A_Count <= (Pocket4 + TrainBasket + TrainBasketMaxNum+3))//自动扫码接收投递地址区域
		//	if(TrainState >1 && TrainState <6)
			{
				if((Applictiao_ADDR_Flag & 0x0f) == 0x03 || (Applictiao_ADDR_Flag & 0x0f) == 0x00)
				{	
					if((RxRAM0[2] & 0x03) == 1)//收到车头的地址
					{
						if(RxRAM0[3] == TrainBasket)
						{
							LastDeliverResult = 0;//0814
							Applictiao_ADDR_Flag &= ~0x0f;//清除申请标志位
							Applictiao_ADDR_Flag |= 0x02;//收到下发地址						
							DeliverAddr =  RxRAM0[4]*256+RxRAM0[5];//获取投递地址、
							DeliverTaskFlag = 1;	//投递任务形成标志
							iQ = 0;jQ = 0;iH = 0;jH = 0;iQOP = 0;jQOP = 0;iHOP = 0;jHOP = 0;//开关斗超时检测清0	
						}
					}
				}
			}
			
			
/////////////////////自动扫码//////////////////////////	
			if(TrainState == 6)
			{
				Applictiao_ADDR_Flag &= ~0x0f;//清除申请标志位	
				Applictiao_ADDR_Flag |= 0x03;
			}
//			else if(TrainState == 5)
//			{
//				Applictiao_ADDR_Flag &= ~0xf0;//先清除上报标志位
//				Applictiao_ADDR_Flag |= 0x20;//收到上报结果应答标志位	
//				PocketCount = 0;
//				LSpeedPocketNum = 0;
//				HSpeedPocketNum = 0;	
//				iQ = 0;jQ = 0;iH = 0;jH = 0;iQOP = 0;jQOP = 0;iHOP = 0;jHOP = 0;//开关斗超时检测清0				
//			}
			
/////////////////////自动扫码//////////////////////////					
		}

		
		if((RxRAM0[0] & 0x0c) == 0x04)//上报结果收到应答信号
		{
			Applictiao_ADDR_Flag &= ~0xf0;//先清除上报标志位
			Applictiao_ADDR_Flag |= 0x20;//收到上报结果应答标志位						
		}
			
		if(Pocket_A_Count == (1 + TrainBasket)) // 上报投递结果 是准确结果
		{
			Applictiao_ADDR_Flag &= ~0xf0;//先清除收到上报结果标志位
			Applictiao_ADDR_Flag |= 0x10;//上报结果标志位  
		//	DeliverActionFlag = 0;0809
		}	
	
		else if(Pocket_A_Count == (2 + TrainBasket)) // 上报投递结果 是准确结果
		{
			Applictiao_ADDR_Flag &= ~0xf0;//先清除收到上报结果标志位
			Applictiao_ADDR_Flag |= 0x10;//上报结果标志位  
		//	DeliverActionFlag = 0;0809
		}			
		
		else if(Pocket_A_Count == (Pocket4 + TrainBasket-1))//挡片计数清0 
		{	
//			Applictiao_ADDR_Flag &= ~0xf0;//先清除上报标志位  //0809注释 因为上报有时不成功
//			Applictiao_ADDR_Flag |= 0x20;//收到上报结果应答标志位	
			PocketCount = 0;
			CageNumber = 0;//0814
			LSpeedPocketNum = 0;
			HSpeedPocketNum = 0;	
			iQ = 0;jQ = 0;iH = 0;jH = 0;iQOP = 0;jQOP = 0;iHOP = 0;jHOP = 0;//开关斗超时检测清0				
		}		
		
		if(TrainState == 6)
		{		
			Applictiao_ADDR_Flag &= ~0xf0;//先清除上报标志位
			Applictiao_ADDR_Flag |= 0x20;//收到上报结果应答标志位			
		}		
//		if(TrainState == 3)
//		{
//			if(Pocket_A_Count == (2 + TrainBasket))//判断挡片是否漏数
//			{
//				PocketCaseNum = CaseNum*2;
//				if(PocketCount == PocketCaseNum)
//				{
//					BasketErrorFlag &= ~0x10;		
//				}
//				else//挡片错误
//				{
//					BasketErrorFlag |= 0x10;
//				}		
//			}	
//		}
		

		
	
	}
	
	
	
	

//		{
//			if(RxRAM0[7] == (StionStop + TrainBasket) || RxRAM0[7] == (2 + TrainBasket))//判断挡片是否漏数
//			{
//				if(PocketCount == CaseNum*2)
//				{
//					BasketErrorFlag &= ~0x10;
//				}
//				else//挡片错误
//				{
//					BasketErrorFlag |= 0x10;
//				}		
//			}
//		}
//		
//		if(RxRAM0[7] >= (3 + TrainBasket + 1) && RxRAM0[7] <= (Pocket4 + TrainBasket))//挡片计数清0
//		{
//			PocketCount = 0;
//		}		
	
	
	

	
	
	else if(CAN_ID == 0x62)//车厢升级方式
	{
		if(RxRAM0[0] == 0xfa)
		{
			if(RxRAM0[1] ==1)
				UpdateModoe = 1;
			else if(RxRAM0[1] ==0)
				UpdateModoe = 0;
		}
		else if(RxRAM0[0] == 0xfb)
		{
			if(RxRAM0[1] ==1)
				UpdataFlag = 1; 
			else if(RxRAM0[1] ==0)
				UpdataFlag = 0;
			
//			if(RxRAM0[2] ==1)
//				systeam_ReStart();  
		}
		
	}
//	else if(CAN_ID == 0x53)//
//	{
//		if(RxRAM0[0] == 1)
//		{
//			LastDeliverResult_Flag = 0;
//		}
//		
//		
//	}
	
}




void Display (void)
{
	
//	if(CanLiveFlag ==0)//CAN不在线
	{
//		buf = TrainBasketID/10%10;
//		buf1 = TrainBasketID%10;	
//		buf2 = TrainBasketID/100;
		LED_Tube_Choose_DisPlay1(gShowNumberData[1], gShowAlphabetData[LED_Display1], gShowNumberData[LED_Display2]);			
	}

}



void ReadE2promData(void)//读取设置参数
{
	CaseNum = EEPROM_Read_u16(0);
	Pocket4 = EEPROM_Read_Byte(2);
	TrainBasketMaxNum = EEPROM_Read_Byte(3);
	ScanCodeMode = EEPROM_Read_Byte(4);
	
	if(CaseNum < 10 || CaseNum == 0xff || Pocket4 >120 || Pocket4 == 0 || TrainBasketMaxNum == 0  || TrainBasketMaxNum == 0xff || ScanCodeMode >1)
	{
		BasketErrorFlag |= 0x10;
	}
//	RecycleCaseNum = EEPROM_Read_u16(15);

}





