#ifndef __MOTOR_H
#define __MOTOR_H	 
#include "sys.h"



void MotorH_Forward (u16 Speed);
void MotorH_Back (u16 Speed);


void MotorQ_Forward (u16 Speed);
void MotorQ_Back (u16 Speed);

void MotorH_Stop(void);
void MotorQ_Stop(void);

void Basket_CloseQ (u16 speed);
void Basket_CloseH (u16 speed);

void Basket_Close (u16 speed);

void Basket_Open (u16 speed);
void Basket_OpenQ (u16 speed);
void Basket_OpenH (u16 speed);
void CheckBasketState(void);
void BasketContral (void);
void ErrorCheck (void);

extern u32 MOTOR1_CUR ;
extern u32 MOTOR2_CUR ;
extern u8 BasketStateQ;
extern u8 BasketStateH;
//extern u8 key1state;
extern u8 BasketFlagQ;
extern u8 BasketFlagH;
extern u8 BasketErrorFlag;

extern u16 iQ,jQ;
extern u16 iH,jH;
extern u16 iQOP,jQOP;
extern u16 iHOP,jHOP;

#endif
