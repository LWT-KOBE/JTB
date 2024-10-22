#include "sys.h"
#include "usart.h"	
#include "string.h"
#include "stm32f10x_it.h" 
#include "app.h"
#include "exti.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 

 
#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
  
	
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(USART2, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART2);
}
#endif



u8 Uart1_Rx[UART1_RX_LEN]={0};
u8 Uart1_Tx[UART1_TX_LEN]={0};
u16 Uart1_Rx_length = 0;
u16 Uart1_Tx_length = 0;	 
	
	
u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.

u8 USART1_RX_counter = 100;			//串口接收数据计数
u8 USART1_RX_FINISH_FLAG = 0;		        //串口接收数据完成标志
u8 USART1_RX_Len = 0;				//串口接收数据长度
	
u8 Uart2_Rx[UART2_RX_LEN]={0};
u8 Uart2_Tx[UART2_TX_LEN]={0}; 
 
u16 Uart2_Rx_length = 0;
u16 Uart2_Tx_length = 0;
u8 SendDataBuf2[10];	


void uart_init(u32 bound) {
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX	  GPIOA.10初始化
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = bound;//串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART1, ENABLE);                    //使能串口1

}

void DMA_USART1_Configuration(void)
{
		NVIC_InitTypeDef NVIC_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		//USART1_DMA接收配置
		DMA_DeInit(DMA1_Channel5); 
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR); 
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart1_Rx; 
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 
		DMA_InitStructure.DMA_BufferSize = UART1_RX_LEN; 
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 	
		DMA_Init(DMA1_Channel5,&DMA_InitStructure); 
		DMA_Cmd(DMA1_Channel5,ENABLE);
		//USART2_DMA 发送 配置
		DMA_DeInit(DMA1_Channel4);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart1_Tx;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_BufferSize = UART1_TX_LEN;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel4,&DMA_InitStructure);
		DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
		DMA_Cmd(DMA1_Channel4, DISABLE);
		
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
}

void Uart1_Start_DMA_Tx(u16 size)
{
		DMA_SetCurrDataCounter(DMA1_Channel4,size);
		DMA_Cmd(DMA1_Channel4, ENABLE);
}
void DMA1_Channel4_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC4))
    {       
        DMA_ClearFlag(DMA1_FLAG_TC4);
        DMA_Cmd(DMA1_Channel4, DISABLE);
        USART_ITConfig(USART1, USART_IT_TC, ENABLE);        
    }
}

void usart1_send(uint8_t c)
{	
	USART_SendData(USART1, c);//库
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);
	USART_ClearFlag(USART1, USART_FLAG_TC);
}


void usart1_send_data(char *buf)
{
	while(*buf != '\0') 
	{
		usart1_send(*buf);
		buf++;
	}
}


void USART1_IRQHandler(void) 
{
//    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) 
//    { 
//			DMA_Cmd(DMA1_Channel5,DISABLE);
//			Uart1_Rx_length = USART1->SR; 
//			Uart1_Rx_length = USART1->DR; 
//			Uart1_Rx_length = UART1_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel5); 
//			
//			if(Uart1_Rx_length<UART1_RX_LEN)
//			{
//				//NFCNUM = Uart2_Rx[0]<<24 | Uart2_Rx[1]<<16 | Uart2_Rx[2]<<8 | Uart2_Rx[3];

//			}      
//			DMA1_Channel5->CNDTR = UART1_RX_LEN;
//			DMA_Cmd(DMA1_Channel5, ENABLE);
//    }
//		
//    if(USART_GetITStatus(USART1, USART_IT_TC)!= RESET)//当发送完成时进入串口1中断
//    {    
//			NVIC_ClearPendingIRQ(USART1_IRQn);//清除中断标志
//			USART_ClearITPendingBit(USART1,USART_IT_TC);//清除串口2发送完成标志标志
//    }		
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        Res =USART_ReceiveData(USART1);	//读取接收到的数据
		//USART_SendData(USART1,Res);
		USART_SendData(USART2,Res);
        if((USART_RX_STA&0x8000)==0)//接收未完成
        {
            if(USART_RX_STA&0x4000)//接收到了0x0d
            {
                if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
                else USART_RX_STA|=0x8000;	//接收完成了
            }
            else //还没收到0X0D
            {
                if(Res==0x0d)USART_RX_STA|=0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                    USART_RX_STA++;
                    if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收
                }
            }
        }
    }
}


void WIFIdataSend(u8 DataLen) 
{
	u8 i=0;
	
////////////////HEAD/////////////////	
	SendDataBuf2[0] = 0xfc;
	SendDataBuf2[1] = 1;
	SendDataBuf2[2] = DataLen;

////////////////V////////////////////	
	
	SendDataBuf2[1+2] = CageNumber>>8;
	SendDataBuf2[2+2] = CageNumber;
	
////////////////TAIL/////////////////	
	SendDataBuf2[3+DataLen] = 0x11;
	SendDataBuf2[4+DataLen] = 0xfc;		
	
	
		for(i=0;i<DataLen+5;i++)
		{
			while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
			USART_SendData(USART1,SendDataBuf2[i]);          
		}	
}


void uart2_init(u32 bound)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART1，GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    USART_DeInit(USART2);  //复位串口1

    //USART2_TX   PA.2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2

    //USART2_RX	  PA.3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3
    
    //配置串口
   USART_InitStructure.USART_BaudRate = bound;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
   USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
   USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

   USART_Init(USART2, &USART_InitStructure);//配置串口2
   USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//使能串口接收中断  
   //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);//串口发送中断在发送数据时开启
   USART_Cmd(USART2, ENABLE);//使能串口3

   //串口中断配置
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//允许USART2中断
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//中断等级
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);                
}









void DMA_USART2_Configuration(void)
{
		NVIC_InitTypeDef NVIC_InitStructure;
		DMA_InitTypeDef DMA_InitStructure;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
		//USART1_DMA接收配置
		DMA_DeInit(DMA1_Channel6); 
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR); 
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart2_Rx; 
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; 
		DMA_InitStructure.DMA_BufferSize = UART2_RX_LEN; 
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; 
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; 
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; 
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh; 
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; 	
		DMA_Init(DMA1_Channel6,&DMA_InitStructure); 
		DMA_Cmd(DMA1_Channel6,ENABLE);
		//USART2_DMA 发送 配置
		DMA_DeInit(DMA1_Channel7);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Uart2_Tx;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
		DMA_InitStructure.DMA_BufferSize = UART2_TX_LEN;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA1_Channel7,&DMA_InitStructure);
		DMA_ITConfig(DMA1_Channel7,DMA_IT_TC,ENABLE);
		DMA_Cmd(DMA1_Channel7, DISABLE);
		
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
}

void usart2_send(uint8_t c)
{	
	USART_SendData(USART2, c);//库
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
	USART_ClearFlag(USART2, USART_FLAG_TC);
}


void usart2_send_data(char *buf)
{
	while(*buf != '\0') 
	{
		usart2_send(*buf);
		buf++;
	}
}


void u2_SendArray(uint8_t *Array, uint16_t Length)	//发送一个8位的数组
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		usart2_send(Array[i]);
	}
}

void Uart2_Start_DMA_Tx(u16 size)
{
		DMA_SetCurrDataCounter(DMA1_Channel7,size);
		DMA_Cmd(DMA1_Channel7, ENABLE);
}
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_FLAG_TC7))
    {       
        DMA_ClearFlag(DMA1_FLAG_TC7);
        DMA_Cmd(DMA1_Channel7, DISABLE);
        USART_ITConfig(USART2, USART_IT_TC, ENABLE);        
    }
}

u8 Res;

void USART2_IRQHandler(void)
{
//    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) 
//    { 
//			DMA_Cmd(DMA1_Channel6,DISABLE);
//			Uart1_Rx_length = USART2->SR; 
//			Uart1_Rx_length = USART2->DR; 
//			Uart1_Rx_length = UART1_RX_LEN - DMA_GetCurrDataCounter(DMA1_Channel6); 
//			if(Uart1_Rx_length<UART1_RX_LEN)
//			{

//				
//			}
//			

//			
//			DMA1_Channel6->CNDTR = UART1_RX_LEN;
//			DMA_Cmd(DMA1_Channel6, ENABLE);
//    }
//		
//    if(USART_GetITStatus(USART2, USART_IT_TC)!= RESET)//当发送完成时进入串口1中断
//    {    
//			NVIC_ClearPendingIRQ(USART2_IRQn);//清除中断标志
//			USART_ClearITPendingBit(USART2,USART_IT_TC);//清除串口2发送完成标志标志
//    }		
		
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //Check if data is received //判断是否接收到数据
	{
		Res = USART_ReceiveData(USART2);	//读取接收到的数据
		USART_SendData(USART1,Res);
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
				}
				else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}
		
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	}
}




#endif	

