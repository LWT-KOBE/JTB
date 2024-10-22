#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义

#define UART2_RX_LEN     1024+50
#define UART2_TX_LEN     256
#define UART1_RX_LEN     90
#define UART1_TX_LEN     256
#define USART1_MAX_RECV_LEN		100				//最大接收缓存字节数
extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8 USART1_RX_counter;			        //串口接收数据计数

extern u8 USART1_RX_FINISH_FLAG;		        //串口接收数据完成标?
extern u8 USART1_RX_Len;				//串口接收数据长度
extern u32 WiFidelaycounter;

extern u8 Uart2_Tx[UART2_TX_LEN];

void uart_init(u32 bound);
void usart1_send(uint8_t c);
void usart1_send_data(char *buf);
void DMA_USART1_Configuration(void);
void uart2_init(u32 bound);
void usart2_send(uint8_t c);
void usart2_send_data(char *buf);
void u2_SendArray(uint8_t *Array, uint16_t Length);
void DMA_USART2_Configuration(void);
void DMA1_Channel4_IRQHandler(void);
void Uart2_Start_DMA_Tx(u16 size);
void WIFIdataSend(u8 DataLen);

#define USART_WIFI_RX_NUM                            (uc8)100
#endif


