#ifndef __APP_H
#define __APP_H
#include "stm32f10x.h"


#define StionStop 					 1
extern u8 LastDeliverResult_Flag;
extern u8 Applictiao_ADDR_Flag;
extern u8 TrainStateOld,TrainStateNew,TrainState;
extern u32 DeliverAddr,LastDeliverResult;
extern u8 DeliverTaskFlag;
extern u16 HSpeedPocketNum;
extern u16 LSpeedPocketNum;
extern u8 DeliverMode,DeliverScussFlag;
extern u8 DeliverActionFlag;
extern u16 CaseNum;
extern u8 EnableApplicationAddr;
extern u8 ApplicFlag;
void DeliverTask (void);
void CheckCarCanCmd(void);
void Display (void);
void ReadE2promData(void);//读取设置参数
extern u8 ert;
extern u8 ScanCodeMode;
extern u8 Pocket4;
extern u16 Pocket_A_Count,Pocket_A_CountOld,Pocket_A_CountNew;
extern u16 SwitchCount1;
extern u8 SwitchFlag1;
extern u16 RecycleCaseNum;
extern u8 TrainBasketMaxNum;
extern u8 SetDateFlag;
extern u32 DeliverResultACKFlag;
extern u32 APPSendADDRFlag;
extern u16 OpendoorDelay;
extern u16 OpendoorDelayCount;
extern u8 OpendoorDelayFlag;
extern u16 DisplayDelayCount;
extern u8 DisplayFlag;
extern u16 PulseMotor,PulseMotorOld;
extern u16 PulseMotorCount;




#endif

