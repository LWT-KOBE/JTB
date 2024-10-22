#include "include.h"   
#include "iap_APP.h"
#include "motor.h"
#include "SEGGER_RTT.h"
#include "app.h"
#include "stm32f10x_pwr.h"
u8 key_flag = 0;
u8 ert = 0;
u16 qwe = 0; 
u8 delaynum1;
u8 delaynum2;
u8 InStionFlag = 0;


                      
int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级	
	delay_init();	    			//延时函数初始化
	KEY_Init();						// IO口配置

	g_sum_timer2 = 0;
	TM1620_Config();
	TM1620_init();
	uart_init(115200);				//初始化串口1
	uart2_init(115200);				//初始化串口2
	time2_init(99, 719); //1MS  
	DMA_USART2_Configuration();
	//AD_Init();
	Adc_Init();
	//DMA_USART1_Configuration();
//	delay_ms(1000);
//	usart2_send_data("AT+CWJAP=\"dd\",\"dd66668888\",\"s.y\"\r\n");
//	delay_ms(1000);
//	usart2_send_data("AT+CIPSTART=\"UDP\",\"192.168.0.105\",1000,1002\r\n");
//	delay_ms(5000);
//	usart2_send_data("AT+CIPMODE=1\r\n");
//	delay_ms(2000);
//	usart2_send_data("AT+CIPSEND\r\n");
	/* 使能电源管理单元的时钟,必须要使能时钟才能进入待机模式 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);
	while(1)  
	{		
		//数码管显示
		Display();

//	
//	/* 如果120秒后没有按键按下则进入待机模式 */
	if(RC.control_idle_time >= 120000){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);
		/*清除WU状态位*/
		PWR_ClearFlag (PWR_FLAG_WU);
		
		/* 使能WKUP引脚的唤醒功能 ，使能PA0*/
		PWR_WakeUpPinCmd (ENABLE);
		/* 进入待机模式 */
		PWR_EnterSTANDBYMode();
		RC.control_idle_time = 120005;
	}
	
	
////	/* 如果120秒后没有按键按下则进入待机模式 */
//	if(time >= 120000){
//		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);
//		/*清除WU状态位*/
//		PWR_ClearFlag (PWR_FLAG_WU);
//		
//		/* 使能WKUP引脚的唤醒功能 ，使能PA0*/
//		PWR_WakeUpPinCmd (ENABLE);
//		/* 进入待机模式 */
//		PWR_EnterSTANDBYMode();
//		time = 120005;
//	}

	
	
	}		
}


