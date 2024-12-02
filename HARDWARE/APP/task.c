#include "stm32f10x.h"
#include "task.h"
#include "include.h" 

//初始化车辆遥控器结构体
remote_control RC = {0,1,0,0,0,0,0};

//遥控指令函数
void remote_control_command(void){
	
	if(TIM_GetFlagStatus(TIM2,TIM_IT_Update)==SET)//
	{	
		
		//如果蓝灯亮起说明wifi没连接上
		if(PBout(14) == 0){
			RC.num ++;
		}else{
			RC.num = 0;
		}

			
		
			
		
		
		//检测wifi掉线是否超过12秒
		if(RC.num >= 12000){
			//超过12秒拉低模块RST脚
			PAout(12) = 0;
			
			//两秒后拉高模块RST脚完成重启
			if(RC.num > 14000){
				RC.num = 0;
				PAout(12) = 1;
			}
			
			
		}
		
		//心跳计数
		RC.time ++;
		
		//计时4秒发送心跳
		if(RC.time >= 4000){
			Uart2_Tx[0] = 0xAA;
			Uart2_Tx[1] = 0x55;
			Uart2_Tx[2] = 0x01;
			Uart2_Tx[3] = 0x55;
			Uart2_Tx[4] = 0xAA;
			u2_SendArray(Uart2_Tx,5);
			RC.time = 0;
			memset(Uart2_Tx, 0, sizeof(Uart2_Tx));
		}
		
		//按键检测
		key_flag = KEY_Scan(0);
		
		//判断按键发送内容
		switch (key_flag)
        {
			//没有按键按下的时候清除发送数组
        	case 0:
			memset(Uart2_Tx, 0, sizeof(Uart2_Tx));
        		break;
			
			//急停按钮按下
        	case 1:
				if(PAin(1) == 1){
					Uart2_Tx[0] = 0xfe;
					Uart2_Tx[1] = 0xb2;
					Uart2_Tx[2] = 0x03;
					Uart2_Tx[3] = 0xFF;
					Uart2_Tx[4] = 0x01;
					Uart2_Tx[5] = 0x01;
					Uart2_Tx[6] = Uart2_Tx[3] ^ Uart2_Tx[4] ^ Uart2_Tx[5];
					Uart2_Tx[7] = 0xef;
				}else{
					Uart2_Tx[0] = 0xfe;
					Uart2_Tx[1] = 0xb2;
					Uart2_Tx[2] = 0x03;
					Uart2_Tx[3] = 0xFF;
					Uart2_Tx[4] = 0x02;
					Uart2_Tx[5] = 0x01;
					Uart2_Tx[6] = Uart2_Tx[3] ^ Uart2_Tx[4] ^ Uart2_Tx[5];
					Uart2_Tx[7] = 0xef;
				}
				
				
				u2_SendArray(Uart2_Tx,8);
        		break;
				
			default:
				break;
        }
		
		
		
	}
}

void TIM2_IRQHandler(void)
{
	{
		

		remote_control_command();
		

		
		 
		TIM_ClearFlag(TIM2,TIM_IT_Update);
	}
}

