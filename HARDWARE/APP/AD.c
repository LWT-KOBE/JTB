#include "stm32f10x.h"                  // Device header
#include "AD.h"
uint16_t AD_Value[19];					//定义用于存放AD转换结果的全局数组
u16 adcSampleValue[2];					//测量电池电压与stm32内部的基准电压数组
/**
  * 函    数：AD初始化
  * 参    数：无
  * 返 回 值：无
  */
void AD_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//开启ADC1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//开启GPIOA的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		//开启DMA1的时钟
	
	/*设置ADC时钟*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);						//选择时钟6分频，ADCCLK = 72MHz / 6 = 12MHz
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//将PA0、PA1、PA2、PA3、PA4、PA5、PA6、PA7引脚初始化为模拟输入
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//将PB0、PB1引脚初始化为模拟输入
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);					//将PC0、PC1、PC2、PC3、PC4、PC5引脚初始化为模拟输入
	
	/*规则组通道配置*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);	//规则组序列1的位置，配置为通道0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);	//规则组序列2的位置，配置为通道1
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);	//规则组序列3的位置，配置为通道2
	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);	//规则组序列4的位置，配置为通道3
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_55Cycles5);	//规则组序列5的位置，配置为通道4
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_55Cycles5);	//规则组序列6的位置，配置为通道5
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_55Cycles5);	//规则组序列7的位置，配置为通道6
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 8, ADC_SampleTime_55Cycles5);	//规则组序列8的位置，配置为通道7
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 9, ADC_SampleTime_55Cycles5);	 //规则组序列9的位置，配置为通道8
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 10, ADC_SampleTime_55Cycles5); //规则组序列10的位置，配置为通道9
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 11, ADC_SampleTime_55Cycles5);	//规则组序列11的位置，配置为通道10
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 12, ADC_SampleTime_55Cycles5);	//规则组序列12的位置，配置为通道11
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 13, ADC_SampleTime_55Cycles5);	//规则组序列13的位置，配置为通道12
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 14, ADC_SampleTime_55Cycles5);	//规则组序列14的位置，配置为通道13
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 15, ADC_SampleTime_55Cycles5);	//规则组序列15的位置，配置为通道14
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 16, ADC_SampleTime_55Cycles5); 	//规则组序列16的位置，配置为通道15
	
	/*ADC初始化*/
	ADC_InitTypeDef ADC_InitStructure;											//定义结构体变量
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;							//模式，选择独立模式，即单独使用ADC1
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//数据对齐，选择右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;			//外部触发，使用软件触发，不需要外部触发
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//连续转换，使能，每转换一次规则组序列后立刻开始下一次转换
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;								//扫描模式，使能，扫描规则组的序列，扫描数量由ADC_NbrOfChannel确定
	ADC_InitStructure.ADC_NbrOfChannel = 16;									//通道数，为16，扫描规则组的前16个通道
	ADC_Init(ADC1, &ADC_InitStructure);											//将结构体变量交给ADC_Init，配置ADC1
	
	/*DMA初始化*/
	DMA_InitTypeDef DMA_InitStructure;											//定义结构体变量
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;				//外设基地址，给定形参AddrA
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//外设数据宽度，选择半字，对应16位的ADC数据寄存器
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设地址自增，选择失能，始终以ADC数据寄存器为源
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value;					//存储器基地址，给定存放AD转换结果的全局数组AD_Value
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//存储器数据宽度，选择半字，与源数据宽度对应
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//存储器地址自增，选择使能，每次转运后，数组移到下一个位置
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//数据传输方向，选择由外设到存储器，ADC数据寄存器转到数组
	DMA_InitStructure.DMA_BufferSize = 16;										//转运的数据大小（转运次数），与ADC通道数一致
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//模式，选择循环模式，与ADC的连续转换一致
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//存储器到存储器，选择失能，数据由ADC外设触发转运到存储器
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;						//优先级，选择中等
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);								//将结构体变量交给DMA_Init，配置DMA1的通道1.DMA_B
	
	/*DMA和ADC使能*/
	DMA_Cmd(DMA1_Channel1, ENABLE);							//DMA1的通道1使能
	ADC_DMACmd(ADC1, ENABLE);								//ADC1触发DMA1的信号使能
	ADC_Cmd(ADC1, ENABLE);									//ADC1使能
	
	/*ADC校准*/
	ADC_ResetCalibration(ADC1);								//固定流程，内部有电路会自动执行校准
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);
	ADC_StartCalibration(ADC1);
	while (ADC_GetCalibrationStatus(ADC1) == SET);
	
	/*ADC触发*/
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//软件触发ADC开始工作，由于ADC处于连续转换模式，故触发一次后ADC就可以一直连续不断地工作
}

void  Adc_Init(void)
{ 	
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;	
    ADC_InitTypeDef ADC_InitStructure; 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PC1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1 

	ADC_Cmd(ADC1, DISABLE); 
	DMA_Cmd(DMA1_Channel1, DISABLE);
	ADC_DMACmd(ADC1, DISABLE);
	ADC_DeInit(ADC1);
	DMA_DeInit(DMA1_Channel1);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	                   // 使能 ADC1  通道时钟.
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);                                          // 设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M.

	//* PC0 作为模拟通道输入引脚.

	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adcSampleValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 2;
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	                       // ADC工作模式:ADC1和ADC2工作在独立模式.
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	                           // 模数转换工作在单通道模式.
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	                       // 模数转换工作在单次转换模式.
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	       // 转换由软件而不是外部触发启动.
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	                   // ADC数据右对齐.
	ADC_InitStructure.ADC_NbrOfChannel = 2;	                                   // 顺序进行规则转换的ADC通道的数目.
	ADC_Init(ADC1, &ADC_InitStructure);	                                       // 根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器.   

	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);	//ADC1,ADC通道,采样时间为239.5周期	  			    
	ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 2, ADC_SampleTime_55Cycles5);	//ADC1,ADC通道,采样时间为239.5周期	  			    
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_Cmd(ADC1, ENABLE);	                                                   // 使能指定的 ADC1.
	
	ADC_ResetCalibration(ADC1);	                                               // 使能复位校准 . 
	while (ADC_GetResetCalibrationStatus(ADC1));	                           // 等待复位校准结束.
	ADC_StartCalibration(ADC1);	                                               // 开启AD校准.
	while (ADC_GetCalibrationStatus(ADC1));	                                   // 等待校准结束.
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_TempSensorVrefintCmd(ENABLE);//打开内部参照电压
}
                        

uint16_t ADC_Read_Temperature(void) {
    // 启动 ADC 转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    // 等待转换结束
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    // 读取 ADC 值
    return ADC_GetConversionValue(ADC1);
}

//冒泡滤波
uint16_t lvbo(uint16_t AD_Value){
	uint8_t i,j,n;
  uint8_t k = 0;
	uint16_t temp = 0,output = 0;
	//定义一个数组存放传入的AD值
	static uint16_t arr[10] = {0};
	//累加十次
	for(k = 0;k < 10;k++){
		arr[k] = AD_Value;
	}
	//进行冒泡排序
			for(i = 0;i < 10;i++){
			for(j = 0;j < i - 1;j++){
				if(arr[j] > arr[j + 1]){
					temp = arr[j];
					arr[j] = arr[j + 1];
					temp = arr[j + 1];
				}
			}
		}
	//清除接收值
	temp = 0;
	//去除大小端
	for(i = 2;i < 8;i++){
		temp += arr[i];
	}
	//取平均值
	temp = temp / 6;
	
//	output = (uint16_t)(0.42*temp + output * 0.58);
	
	return temp;
}

//一阶低通滤波
void pass_filter(){
	
}


//滑动平均值滤波
uint16_t MoveAverageFilter(uint16_t AD_value){
	uint16_t sum = 0,curNum = 0;
	uint16_t value_buf[50] = {0};
	if(curNum < 50)
    {
        value_buf[curNum] = AD_value;
        sum += value_buf[curNum];
			  curNum++;
        return sum/curNum;
    }
    else
    {
        sum -= sum/50;
        sum += AD_value;
        return sum/50;
    }
	
}

//卡尔曼滤波
uint16_t KalmanFilter(uint16_t inData)
{
	static float prevData = 0;                                 //先前数值
	static float p = 10, q = 0.025, r = 0.001, kGain = 0;      // q控制误差  r控制响应速度 

	p = p + q;
	kGain = p / ( p + r );                                     //计算卡尔曼增益
	inData = prevData + ( kGain * ( inData - prevData ) );     //计算本次滤波估计值
	p = ( 1 - kGain ) * p;                                     //更新测量方差
	prevData = inData;
	return inData;                                             //返回滤波值
}

