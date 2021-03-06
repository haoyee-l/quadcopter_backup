/***********************************
---------硬件上的引脚连接:----------
接口：
TXD  -->  PA9  (UART1-TXD)
RXD  -->  PA10 (UART1-RXD)
------------------------------------
***********************************/

#include "quad_uart.h"
#include <string.h>

/**********************
       内部函数
**********************/
/*uint8_t BYTE0(uint16_t data)
{
	return (uint8_t)(data&0x00FF);
}

uint8_t BYTE1(uint16_t data)
{
	return (uint8_t)(data>>8);
}*/


/**************************实现函数********************************************
*函数原型:		void Initial_UART1(u32 baudrate)
*功　　能:		初始化UART
输入参数：
		u32 baudrate   设置RS232串口的波特率
输出参数：没有	
*******************************************************************************/
void Initial_UART1(uint32_t baudrate)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* 使能 UART1 模块的时钟  使能 UART1对应的引脚端口PA的时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  	 /* 配置UART1 的发送引脚
	 配置PA9 为复用输出  刷新频率50MHz
	  */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);    
  	/* 
	  配置UART1 的接收引脚
	  配置PA10为浮地输入 
	 */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	/* 
	  UART1的配置:
	  1.波特率为调用程序指定的输入 baudrate;
	  2. 8位数据			  USART_WordLength_8b;
	  3.一个停止位			  USART_StopBits_1;
	  4. 无奇偶效验			  USART_Parity_No ;
	  5.不使用硬件流控制	  USART_HardwareFlowControl_None;
	  6.使能发送和接收功能	  USART_Mode_Rx | USART_Mode_Tx;
	 */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//应用配置到UART1
	USART_Init(USART1, &USART_InitStructure); 
	//启动UART1
  	USART_Cmd(USART1, ENABLE);
}

/**************************实现函数********************************************
*函数原型:		void UART1_Put_Char(unsigned char DataToSend)
*功　　能:		RS232发送一个字节
输入参数：
		unsigned char DataToSend   要发送的字节数据
输出参数：没有	
*******************************************************************************/
void UART1_Put_Char(unsigned char DataToSend)
{
	//将要发送的字节写到UART1的发送缓冲区
	USART_SendData(USART1, (unsigned char) DataToSend);
	//等待发送完成
  	while (!(USART1->SR & USART_FLAG_TXE));
}

/**************************实现函数********************************************
*函数原型:		void Uart1_Put_Buf(uint8_t *tx_buffer,uint8_t count)
*功　　能:		UART发送缓冲区tx_buffer中的数据
输入参数：
		*tx_buffer   		缓冲区起始地址
		count						数据字节长度
输出参数：没有	
*******************************************************************************/
void Uart1_Put_Buf(uint8_t *tx_buffer,uint8_t count)
{
	uint8_t i;
	for(i=0;i<count;i++)
	{
		 UART1_Put_Char(tx_buffer[i]);
	}
}

/**************************实现函数********************************************
*函数原型:		UART1_ReportIMU(int16_t yaw,int16_t pitch,int16_t roll,int16_t tempr)
*功　　能:		向上位机发送经过解算后的姿态数据
输入参数：
		int16_t yaw 经过解算后的航向角度。单位为0.1度 0 -> 3600  对应 0 -> 360.0度
		int16_t pitch 解算得到的俯仰角度，单位 0.1度。-900 - 900 对应 -90.0 -> 90.0 度
		int16_t roll  解算后得到的横滚角度，单位0.1度。 -1800 -> 1800 对应 -180.0  ->  180.0度
		int16_t tempr 温度 。 单位0.1摄氏度

输出参数：没有	
*******************************************************************************/
void UART1_ReportIMU(int16_t pitch,int16_t roll,int16_t yaw,int16_t tempr)
{
 	uint8_t tx_buffer[16],count = 0,sum = 0,
					i;
	uint16_t temp;
	
	tx_buffer[count++]=0xAA;
	tx_buffer[count++]=0xAA;
	tx_buffer[count++]=0x01;
	tx_buffer[count++]=0;
	
	temp = (int)(roll*100);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(pitch*100);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(yaw*100);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(tempr*100);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(tempr*100);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
		
	tx_buffer[count++]=0xA1;					//加锁状态
	
	tx_buffer[3] = count-4;
	
	for(i=0;i<count;i++)
		sum += tx_buffer[i];
		
	tx_buffer[count++]=sum;

	Uart1_Put_Buf(tx_buffer,count);

}

/**************************实现函数********************************************
*函数原型:		void UART1_ReportMotion(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,
					int16_t hx,int16_t hy,int16_t hz)
*功　　能:		向上位机发送当前传感器的输出值
输入参数：
	int16_t ax  加速度 X轴ADC输出 范围 ：一个有符号整型
	int16_t ay  加速度 Y轴ADC输出 范围 ：一个有符号整型
	int16_t az  加速度 Z轴ADC输出 范围 ：一个有符号整型
	int16_t gx  陀螺仪 X轴ADC输出 范围 ：一个有符号整型
	int16_t gy  陀螺仪 Y轴ADC输出 范围 ：一个有符号整型
	int16_t gz  陀螺仪 Z轴ADC输出 范围 ：一个有符号整型
	int16_t hx  磁罗盘 X轴ADC输出 范围 ：一个有符号整型
	int16_t hy  磁罗盘 Y轴ADC输出 范围 ：一个有符号整型
	int16_t hz  磁罗盘 Z轴ADC输出 范围 ：一个有符号整型
	
输出参数：没有	
*******************************************************************************/
void UART1_ReportMotion(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,
					int16_t hx,int16_t hy,int16_t hz)
{
 	unsigned int temp=0xaF+9;
	char ctemp;
	UART1_Put_Char(0xa5);
	UART1_Put_Char(0x5a);
	UART1_Put_Char(14+8);
	UART1_Put_Char(0xA2);

	if(ax<0)ax=32768-ax;
	ctemp=ax>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=ax;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(ay<0)ay=32768-ay;
	ctemp=ay>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=ay;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(az<0)az=32768-az;
	ctemp=az>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=az;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(gx<0)gx=32768-gx;
	ctemp=gx>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gx;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(gy<0)gy=32768-gy;
	ctemp=gy>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gy;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
//-------------------------
	if(gz<0)gz=32768-gz;
	ctemp=gz>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=gz;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(hx<0)hx=32768-hx;
	ctemp=hx>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hx;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(hy<0)hy=32768-hy;
	ctemp=hy>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hy;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	if(hz<0)hz=32768-hz;
	ctemp=hz>>8;
	UART1_Put_Char(ctemp);
	temp+=ctemp;
	ctemp=hz;
	UART1_Put_Char(ctemp);
	temp+=ctemp;

	UART1_Put_Char(temp%256);
	UART1_Put_Char(0xaa);
}

void PrintChar(char *strings)
{
	uint8_t len,i;
	len = strlen(strings);
	for(i=0;i<len;i++)
		{
			UART1_Put_Char(strings[i]);
		}
}

//------------------------------------------------------
/*void USART1_IRQHandler(void)
{
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
  {   
    // Write one byte to the transmit data register 
//S    USART_SendData(USART1, TxBuffer[TxCounter++]);                    

    // Clear the USART1 transmit interrupt 
    USART_ClearITPendingBit(USART1, USART_IT_TXE); 

//S    if(TxCounter == count)
    {
      // Disable the USART1 Transmit interrupt 
      USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }    
  }

}*/


//------------------End of File----------------------------
