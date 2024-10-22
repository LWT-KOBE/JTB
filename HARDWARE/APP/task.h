#ifndef __TASK_H
#define __TASK_H

#include "include.h" 

typedef struct  
{
	u8 car_status;						//遥控的状态
	u8 car_num;							//遥控的车辆
	u8 low_battery;						//低电量状态
	u8 function_display;				//功能显示状态
	u8 digital_display;					//数字显示
	int low_battery_num;				//低电压维持时间
	float control_idle_time;			//未按下遥控器的时间
	float battery_voltage;				//电池电压
	float reference_voltage;			//内部基准电压
	
}remote_control;

extern remote_control RC;

#endif // __BSP_EEPROM_H
