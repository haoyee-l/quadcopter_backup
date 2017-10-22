#include "nRF24L01.h"

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

void nRF24L01_XC1_Timer4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	TIM_OCInitTypeDef  TIM_OCInitStructure;  
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);  
 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;													//TIMERʱ��Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 3;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 2;
	TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
  TIM_Cmd(TIM4, ENABLE);
}

void nRF24L01_IRQ_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//SPI��ʼ������
void SPI2_Init(void)
{
	RCC->APB2ENR|=1<<3;       //PORTBʱ��ʹ�� 	 
	RCC->APB1ENR|=1<<14;      //SPI2ʱ��ʹ�� 
		   
	//����ֻ���SPI�ڳ�ʼ��
	GPIOB->CRH &= 0x000FFFFF; 
	GPIOB->CRH |= 0xBBB00000;		//PB13.14.15���� 	    
	GPIOB->ODR |= 0x70000000;   //PB13.14.15����
		
	SPI2->CR1|=0<<10;					//ȫ˫��ģʽ	
	SPI2->CR1|=1<<9;					//���nss����
	SPI2->CR1|=1<<8;  

	SPI2->CR1|=1<<2; 					//SPI����
	SPI2->CR1|=0<<11;					//8bit���ݸ�ʽ	
	//��24L01Ҫ���� CPHA=0;CPOL=0;
	SPI2->CR1|=0<<1;						//CPOL=0ʱ����ģʽ��SCKΪ1 
	//SPI2->CR1|=1<<1; 					//����ģʽ��SCKΪ1 CPOL=1			   
	SPI2->CR1|=0<<0;					//��һ��ʱ�ӵ��½���,CPHA=1 CPOL=1	   
	SPI2->CR1|=7<<3;					//Fsck=Fcpu/256
	SPI2->CR1|=0<<7;					//MSBfirst   
	SPI2->CR1|=1<<6;					//SPI�豸ʹ��
	SPI2_ReadWriteByte(0xff);	//��������		 
}

//SPI �ٶ����ú���
//SpeedSet:
//SPI_SPEED_2   2��Ƶ   (SPI 36M@sys 72M)
//SPI_SPEED_8   8��Ƶ   (SPI 9M@sys 72M)
//SPI_SPEED_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256��Ƶ (SPI 281.25K@sys 72M)
void SPI2_SetSpeed(uint8_t  SpeedSet)
{
	SPI2->CR1&=0xFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI_SPEED_2)//����Ƶ
	{
		SPI2->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
	}else if(SpeedSet==SPI_SPEED_8)//�˷�Ƶ 
	{
		SPI2->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
	}else if(SpeedSet==SPI_SPEED_16)//ʮ����Ƶ
	{
		SPI2->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
	}else			 	 //256��Ƶ
	{
		SPI2->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz ����ģʽ
	}
	SPI2->CR1|=1<<6; //SPI�豸ʹ��	  
}

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint8_t	SPI2_ReadWriteByte(uint8_t  TxData)
{
	//while(SPI2->SR & 0x01)
	//	SPI2->DR;
	
	//if(SPI2->SR & 0x02)//�ȴ���������			  
		SPI2->DR=TxData;	 	  //����һ��byte 
	
	while((SPI2->SR & 0x80));
	//while(SPI2->SR & 0x01) //�ȴ�������һ��byte    						    
		return SPI2->DR;          //�����յ�������
	//return 0;
}


/************************************************************************
nRF24L01��غ���
************************************************************************/
void nRF24L01_SPI2_Init(void)
{
	RCC->APB2ENR |= 1<<3;				//ʹ��PORTB��ʱ��
	RCC->APB2ENR |= 1<<2;    		//ʹ��PORTA��ʱ��
	GPIOA->CRH &= 0xFFFFF0FF;		//PA10���
	GPIOB->CRH &= 0xFFF0FFFF;		//PB12���
	GPIOA->CRH |= 0x00000300;
	GPIOB->CRH |= 0x00030000; 
	GPIOA->ODR |= 1<<10;	  //PA10���1
	GPIOB->ODR |= 1<<12;	  //PB12���1
	
	GPIOA->CRH &= 0xFFFF0FFF;		//PA11����ģʽ/�жϽ�
	
	nRF24L01_XC1_Timer4_Init();
	nRF24L01_IRQ_Init();

	SPI2_Init();    						//��ʼ��SPI
	nRF24L01_CE_LOW;			 			//ʹ��24L01
	nRF24L01_CSN_HIGH;					//SPIƬѡȡ��
}

uint8_t nRF24L01_Check(void)
{
	uint8_t buf[5]={0xA5,0xA5,0xA5,0xA5,0xA5};
	uint8_t i;
	SPI2_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	nRF24L01_Write_Buf(nRF24L01_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	nRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
	for(i=0;i<5;i++)
	{
		if(buf[i]!=0xA5)
			break;
	}		
	if(i!=5)
		return 1;//���24L01����	
	return 0;		 //��⵽24L01
}

//��ȡ״̬�Ĵ���
uint8_t nRF24L01_Get_State(void)
{
	uint8_t reg_val;
 	nRF24L01_CSN_LOW;
	reg_val=SPI2_ReadWriteByte(0xFF);
	nRF24L01_CSN_HIGH; 
	return(reg_val);
}

//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
uint8_t nRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
	nRF24L01_CSN_LOW;                 	//ʹ��SPI����
	status =SPI2_ReadWriteByte(reg);		//���ͼĴ����� 
	SPI2_ReadWriteByte(value);      		//д��Ĵ�����ֵ
	nRF24L01_CSN_HIGH;                  //��ֹSPI����	   
	return(status);       							//����״ֵ̬
}

//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
uint8_t nRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;	    
 	nRF24L01_CSN_LOW;// = 0;          //ʹ��SPI����		
	SPI2_ReadWriteByte(reg);   //���ͼĴ�����
	reg_val=SPI2_ReadWriteByte(0xFF);//��ȡ�Ĵ�������
	nRF24L01_CSN_HIGH;// = 1;          //��ֹSPI����		    
	return(reg_val);           //����״ֵ̬
}

//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
uint8_t nRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,u8_ctr;	    
 	nRF24L01_CSN_LOW;									      //ʹ��SPI����
	status = SPI2_ReadWriteByte(reg);				//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)
		SPI2_ReadWriteByte(*pBuf++); 					//д������	 
	nRF24L01_CSN_HIGH;											//�ر�SPI����
	return status;													//���ض�����״ֵ̬
}

//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
uint8_t nRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,u8_ctr;	       
	nRF24L01_CSN_LOW;				            			//ʹ��SPI����
	status=SPI2_ReadWriteByte(reg);						//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)
		pBuf[u8_ctr] = SPI2_ReadWriteByte(0xFF);//��������
	nRF24L01_CSN_HIGH;						      			//�ر�SPI����
	return status;      										  //���ض�����״ֵ̬
}
		   
//����nRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
uint8_t nRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t state;   
	nRF24L01_CE_LOW;
  nRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	//д���ݵ�TX BUF  32���ֽ�
 	nRF24L01_CE_HIGH;																				//��������	   
	while(nRF24L01_IRQ != 0);																	//�ȴ��������
	nRF24L01_CSN_LOW;
	state=SPI2_ReadWriteByte(0xFF);
	nRF24L01_CSN_HIGH;  
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+STATUS, state); 						//���TX_DS��MAX_RT�жϱ�־
	if(state & MAX_TX)																				//�ﵽ����ط�����
	{
		nRF24L01_Write_Reg(FLUSH_TX,0xFF);										//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(state & TX_OK)																				//�������
	{
		return TX_OK;
	}
	return 0xff;																						//����ԭ����ʧ��
}

//����nRF24L01����һ������
//txbuf:��������׵�ַ
//����ֵ:0��������ɣ��������������
uint8_t nRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;
	nRF24L01_CSN_LOW;
	state=SPI2_ReadWriteByte(0xFF);
	nRF24L01_CSN_HIGH;
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+STATUS, state); 	//���TX_DS��MAX_RT�жϱ�־
	if(state & RX_OK)																				//���յ�����
	{
		nRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH);//��ȡ����
		nRF24L01_Write_Reg(FLUSH_RX,0xff);										//���RX FIFO�Ĵ��� 
		return 0; 
	}
	return 1;																								//û�յ��κ�����
}

//�ú�����ʼ��nRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void nRF24L01_RX_Mode(void)
{
	nRF24L01_CE_LOW;	  
	nRF24L01_Write_Buf(nRF24L01_WRITE_REG+RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH);//дRX�ڵ��ַ

	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+EN_AA,0x00);    					//ʹ��ͨ��0���Զ�Ӧ��    
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+EN_RXADDR,0x01);					//ʹ��ͨ��0�Ľ��յ�ַ  	 
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RF_CH,40);	     					//����RFͨ��Ƶ��		  
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RF_SETUP,0x07);					//����TX�������,0db����,2Mbps,���������濪��   
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+CONFIG, 0x3f);						//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	nRF24L01_CE_HIGH;																			//CEΪ��,�������ģʽ 
	//nRF24L01_IRQ_Init();
	//delay_ms(1);
}

//�ú�����ʼ��nRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void nRF24L01_TX_Mode(void)
{
	nRF24L01_CE_LOW;	    
	nRF24L01_Write_Buf(nRF24L01_WRITE_REG+TX_ADDR,(uint8_t *)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
	nRF24L01_Write_Buf(nRF24L01_WRITE_REG+RX_ADDR_P0,(uint8_t *)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RF_SETUP,0x07);  //����TX�������,0db����,1Mbps,���������濪��   
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	nRF24L01_CE_HIGH;//CEΪ��,10us����������
}
