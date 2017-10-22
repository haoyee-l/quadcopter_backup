/***********************************
---------Ӳ���ϵ���������:----------
�ӿڣ�
TXD  -->  PA9  (UART1-TXD)
RXD  -->  PA10 (UART1-RXD)
------------------------------------
***********************************/

#include "quad_uart.h"
#include <string.h>
#include <stdio.h>

char UART1RxData[50];
uint8_t UART1RxDataOffset;
char pidFresh = 0;

/**********************
       �ڲ�����
**********************/
/*uint8_t BYTE0(uint16_t data)
{
	return (uint8_t)(data&0x00FF);
}

uint8_t BYTE1(uint16_t data)
{
	return (uint8_t)(data>>8);
}*/


/**************************ʵ�ֺ���********************************************
*����ԭ��:		void Initial_UART1(u32 baudrate)
*��������:		��ʼ��UART
���������
		u32 baudrate   ����RS232���ڵĲ�����
���������û��	
*******************************************************************************/
void Initial_UART1(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* ʹ�� UART1 ģ���ʱ��  ʹ�� UART1��Ӧ�����Ŷ˿�PA��ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);
	/* ����UART1 �ķ�������
	����PA9 Ϊ�������  ˢ��Ƶ��50MHz
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
	/* 
	����UART1 �Ľ�������
	����PA10Ϊ�������� 
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 ,ENABLE);
	/* 
	UART1������:
	1.������Ϊ���ó���ָ�������� baudrate;
	2. 8λ����			  USART_WordLength_8b;
	3.һ��ֹͣλ			  USART_StopBits_1;
	4. ����żЧ��			  USART_Parity_No ;
	5.��ʹ��Ӳ��������	  USART_HardwareFlowControl_None;
	6.ʹ�ܷ��ͺͽ��չ���	  USART_Mode_Rx | USART_Mode_Tx;
	*/
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//Ӧ�����õ�UART1
	USART_Init(USART1, &USART_InitStructure); 
	//64M--57600--0x457
	//USART1->BRR = 0x457;
	USART1->CR1 |= 0x20;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//����UART1
	USART_Cmd(USART1, ENABLE);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void UART1_Put_Char(unsigned char DataToSend)
*��������:		RS232����һ���ֽ�
���������
		unsigned char DataToSend   Ҫ���͵��ֽ�����
���������û��	
*******************************************************************************/
void UART1_Put_Char(unsigned char DataToSend)
{
	//��Ҫ���͵��ֽ�д��UART1�ķ��ͻ�����
	USART_SendData(USART1, (unsigned char) DataToSend);
	//�ȴ��������
  	while (!(USART1->SR & USART_FLAG_TXE));
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void Uart1_Put_Buf(uint8_t *tx_buffer,uint8_t count)
*��������:		UART���ͻ�����tx_buffer�е�����
���������
		*tx_buffer   		��������ʼ��ַ
		count						�����ֽڳ���
���������û��	
*******************************************************************************/
void Uart1_Put_Buf(uint8_t *tx_buffer,uint8_t count)
{
	uint8_t i;
	for(i=0;i<count;i++)
	{
		 UART1_Put_Char(tx_buffer[i]);
	}
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		UART1_ReportIMU(int16_t yaw,int16_t pitch,int16_t roll,int16_t tempr)
*��������:		����λ�����;�����������̬����
���������
		int16_t yaw ���������ĺ���Ƕȡ���λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
		int16_t pitch ����õ��ĸ����Ƕȣ���λ 0.1�ȡ�-900 - 900 ��Ӧ -90.0 -> 90.0 ��
		int16_t roll  �����õ��ĺ���Ƕȣ���λ0.1�ȡ� -1800 -> 1800 ��Ӧ -180.0  ->  180.0��
		int16_t tempr �¶� �� ��λ0.1���϶�

���������û��	
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
		
	tx_buffer[count++]=0xA1;					//����״̬
	
	tx_buffer[3] = count-4;
	
	for(i=0;i<count;i++)
		sum += tx_buffer[i];
		
	tx_buffer[count++]=sum;

	Uart1_Put_Buf(tx_buffer,count);

}

void UART1_ReportGyro(int16_t gyroX, int16_t gyroY, int16_t gyroZ)
{
	uint8_t tx_buffer[16],count = 0,sum = 0,
					i;
	uint16_t temp;
	
	tx_buffer[count++]=0xAA;
	tx_buffer[count++]=0xAA;
	tx_buffer[count++]=0x02;
	tx_buffer[count++]=0;
	
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	
	temp = (int)(gyroX);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(gyroY);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(gyroZ);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	
	tx_buffer[3] = count-4;
	
	for(i=0;i<count;i++)
		sum += tx_buffer[i];
		
	tx_buffer[count++]=sum;

	Uart1_Put_Buf(tx_buffer,count);
}

/**************************ʵ�ֺ���********************************************
*����ԭ��:		void UART1_ReportMotion(int16_t ax,int16_t ay,int16_t az,int16_t gx,int16_t gy,int16_t gz,
					int16_t hx,int16_t hy,int16_t hz)
*��������:		����λ�����͵�ǰ�����������ֵ
���������
	int16_t ax  ���ٶ� X��ADC��� ��Χ ��һ���з�������
	int16_t ay  ���ٶ� Y��ADC��� ��Χ ��һ���з�������
	int16_t az  ���ٶ� Z��ADC��� ��Χ ��һ���з�������
	int16_t gx  ������ X��ADC��� ��Χ ��һ���з�������
	int16_t gy  ������ Y��ADC��� ��Χ ��һ���з�������
	int16_t gz  ������ Z��ADC��� ��Χ ��һ���з�������
	int16_t hx  ������ X��ADC��� ��Χ ��һ���з�������
	int16_t hy  ������ Y��ADC��� ��Χ ��һ���з�������
	int16_t hz  ������ Z��ADC��� ��Χ ��һ���з�������
	
���������û��	
*******************************************************************************/
void UART1_ReportMotion(int16_t accel[3], int16_t gyro[3])
{
 	uint8_t tx_buffer[16],count = 0,sum = 0,
					i;
	uint16_t temp;
	
	tx_buffer[count++]=0xAA;
	tx_buffer[count++]=0xAA;
	tx_buffer[count++]=0x02;
	tx_buffer[count++]=0;
	
	temp = (int)(accel[0]);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(accel[1]);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(accel[2]);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(gyro[0]);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(gyro[1]);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	temp = (int)(gyro[2]);
	tx_buffer[count++]=BYTE1(temp);
	tx_buffer[count++]=BYTE0(temp);
	
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	tx_buffer[count++] = 0;
	
	tx_buffer[3] = count-4;
	
	for(i=0;i<count;i++)
		sum += tx_buffer[i];
		
	tx_buffer[count++]=sum;

	Uart1_Put_Buf(tx_buffer,count);
}

/*void UAR1_ReportMotor(void)
{
	char cdata[10];
	sprintf(cdata,"%d",motor.motor_1);
	PrintChar("M1:");
	PrintChar(cdata);
	PrintChar("\n");
	
	sprintf(cdata,"%d",motor.motor_2);
	PrintChar("M2:");
	PrintChar(cdata);
	PrintChar("\n");
	
	sprintf(cdata,"%d",motor.motor_3);
	PrintChar("M3:");
	PrintChar(cdata);
	PrintChar("\n");
	
	sprintf(cdata,"%d",motor.motor_4);
	PrintChar("M4:");
	PrintChar(cdata);
	PrintChar("\n\n");
}*/

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
void USART1_IRQHandler(void)
{
	char tmp;
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_TXE);
	}
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		tmp = USART_ReceiveData(USART1);
		if( tmp == 'A' )
			UART1RxDataOffset = 0;
		else if( tmp == 'E' )
			pidFresh = 1;
		else
		{
			UART1RxData[UART1RxDataOffset] = tmp;
			UART1RxDataOffset++;
		}
  }

}


//------------------End of File----------------------------
