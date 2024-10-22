#ifndef _TIMER_
#define _TIMER_

#include "include.h"

//extern u8 tt;
extern u16 g_sum_timer2;
extern float time;
void GetAdcAverage(void);
void time2_init(u16 arr,u16 psc);

void TIM3_PWM_Init(u16 arr,u16 psc);

void OC1_Cmd(FunctionalState NewState);
void OC2_Cmd(FunctionalState NewState);
void OC3_Cmd(FunctionalState NewState);
void OC4_Cmd(FunctionalState NewState);
extern u32 ADCsum1;
extern u16 SendCanDelay,Timer10msDelay;
#endif

