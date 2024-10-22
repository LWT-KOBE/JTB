#ifndef __AD_H
#define __AD_H
#include "include.h"   

extern uint16_t AD_Value[19];
extern u16 adcSampleValue[2];

void AD_Init(void);
void ADC_Init_Config(void);
void  Adc_Init(void);
uint16_t ADC_Read_Temperature(void);
uint16_t lvbo(uint16_t AD_Value);
uint16_t MoveAverageFilter(uint16_t AD_value);
uint16_t KalmanFilter(uint16_t inData);

#endif
