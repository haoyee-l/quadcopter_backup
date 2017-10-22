#include "nRF24L01.h"

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

/************************************************************************
SPI������غ���
************************************************************************/
//SPI��ʼ������
void SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1,ENABLE);
		   
	//����ֻ���SPI�ڳ�ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		
	SPI1->CR1|=0<<10;					//ȫ˫��ģʽ	
	SPI1->CR1|=1<<9;					//���nss����
	SPI1->CR1|=1<<8;  

	SPI1->CR1|=1<<2; 					//SPI����
	SPI1->CR1|=0<<11;					//8bit���ݸ�ʽ	
	//��24L01Ҫ���� CPHA=0;CPOL=0;
	SPI1->CR1|=0<<1;						//CPOL=0ʱ����ģʽ��SCKΪ1 
	//SPI1->CR1|=1<<1; 					//����ģʽ��SCKΪ1 CPOL=1			   
	SPI1->CR1|=0<<0;					//��һ��ʱ�ӵ��½���,CPHA=1 CPOL=1	   
	SPI1->CR1|=7<<3;					//Fsck=Fcpu/256
	SPI1->CR1|=0<<7;					//MSBfirst   
	SPI1->CR1|=1<<6;					//SPI�豸ʹ��
	SPI1_ReadWriteByte(0xff);	//��������		 
	
}

//SPI �ٶ����ú���
//SpeedSet:
//SPI_SPEED_2   2��Ƶ   (SPI 36M@sys 72M)
//SPI_SPEED_8   8��Ƶ   (SPI 9M@sys 72M)
//SPI_SPEED_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPI_SPEED_256 256��Ƶ (SPI 281.25K@sys 72M)
void SPI1_SetSpeed(uint8_t  SpeedSet)
{
	SPI1->CR1&=0xFFC7;//Fsck=Fcpu/256
	if(SpeedSet==SPI_SPEED_2)//����Ƶ
	{
		SPI1->CR1|=0<<3;//Fsck=Fpclk/2=36Mhz	
	}else if(SpeedSet==SPI_SPEED_8)//�˷�Ƶ 
	{
		SPI1->CR1|=2<<3;//Fsck=Fpclk/8=9Mhz	
	}else if(SpeedSet==SPI_SPEED_16)//ʮ����Ƶ
	{
		SPI1->CR1|=3<<3;//Fsck=Fpclk/16=4.5Mhz
	}else			 	 //256��Ƶ
	{
		SPI1->CR1|=7<<3; //Fsck=Fpclk/256=281.25Khz ����ģʽ
	}
	SPI1->CR1|=1<<6; //SPI�豸ʹ��	  
}

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint8_t	SPI1_ReadWriteByte(uint8_t  TxData)
{
	uint8_t tmp;
	tmp = tmp;
	while(SPI1->SR & 1<<0)
		tmp = SPI1->DR;
	
	if(SPI1->SR & 1<<1)//�ȴ���������			  
		SPI1->DR=TxData;	 	  //����һ��byte 
	
	while((SPI1->SR&1<<7));
	while((SPI1->SR & 1<<0)) //�ȴ�������һ��byte    						    
		return SPI1->DR;          //�����յ�������
	return 0;
}


/************************************************************************
nRF24L01��غ���
************************************************************************/
void nRF24L01_SPI1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);    		//ʹ��PORTA��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	nRF24L01_CSN_LOW;
	nRF24L01_CE_LOW;
	
	nRF24L01_CSN_HIGH;
	nRF24L01_CE_HIGH;
	
	
	GPIOB->CRL &= 0xF0FFFFFF;		//PA11����ģʽ/�жϽ�
	
	
	SPI1_Init();    						//��ʼ��SPI
	nRF24L01_CE_LOW;			 			//ʹ��24L01
	nRF24L01_CSN_HIGH;					//SPIƬѡȡ��
}

uint8_t nRF24L01_Check(void)
{
	uint8_t buf[5]={0xA5,0xA5,0xA5,0xA5,0xA5};
	uint8_t i;

	SPI1_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	nRF24L01_Write_Buf(nRF24L01_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.
	//BUG
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

//SPIд�Ĵ���
//reg:ָ���Ĵ�����ַ
//value:д���ֵ
uint8_t nRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
	nRF24L01_CSN_LOW;                 	//ʹ��SPI����
	status =SPI1_ReadWriteByte(reg);		//���ͼĴ����� 
	SPI1_ReadWriteByte(value);      		//д��Ĵ�����ֵ
	nRF24L01_CSN_HIGH;                  //��ֹSPI����	   
	return(status);       							//����״ֵ̬
}

//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
uint8_t nRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;	    
 	nRF24L01_CSN_LOW;// = 0;          //ʹ��SPI����		
	SPI1_ReadWriteByte(reg);   //���ͼĴ�����
	reg_val=SPI1_ReadWriteByte(0xFF);//��ȡ�Ĵ�������
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
	status = SPI1_ReadWriteByte(reg);				//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)
		status = SPI1_ReadWriteByte(*pBuf++); 					//д������	 
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
	status=SPI1_ReadWriteByte(reg);						//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)
		pBuf[u8_ctr] = SPI1_ReadWriteByte(0xFF);//��������
	nRF24L01_CSN_HIGH;						      			//�ر�SPI����
	return status;      										  //���ض�����״ֵ̬
}
		   
//����nRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
uint8_t nRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t state;
 	SPI1_SetSpeed(SPI_SPEED_8);															//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	nRF24L01_CE_LOW;
  nRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	//д���ݵ�TX BUF  32���ֽ�
 	nRF24L01_CE_HIGH;																				//��������	   
	while(nRF24L01_IRQ != 0);																	//�ȴ��������
	state=nRF24L01_Read_Reg(STATUS);  											//��ȡ״̬�Ĵ�����ֵ	   
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
	SPI1_SetSpeed(SPI_SPEED_8); 														//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	state=nRF24L01_Read_Reg(STATUS);  											//��ȡ״̬�Ĵ�����ֵ    	 
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+STATUS, state); 						//���TX_DS��MAX_RT�жϱ�־
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

	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+EN_AA,0x01);    					//ʹ��ͨ��0���Զ�Ӧ��    
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+EN_RXADDR,0x01);					//ʹ��ͨ��0�Ľ��յ�ַ  	 
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RF_CH,0);	     					//����RFͨ��Ƶ��		  
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RF_SETUP,0x07);					//����TX�������,0db����,2Mbps,���������濪��   
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+CONFIG, 0x0f);						//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	nRF24L01_CE_HIGH;																			//CEΪ��,�������ģʽ 
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
	//nRF24L01_Write_Buf(nRF24L01_WRITE_REG+RX_ADDR_P0,(uint8_t *)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+EN_AA,0x00);     //ʹ��ͨ��0���Զ�Ӧ��    
	//nRF24L01_Write_Reg(nRF24L01_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+SETUP_RETR,0x0F);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RF_CH,40);       //����RFͨ��Ϊ40
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+RF_SETUP,0x07);  //����TX�������,0db����,2Mbps,���������濪��   
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	nRF24L01_CE_HIGH;//CEΪ��,10us����������
	STM32_nRF24L01_delay_us(100);
}

void STM32_nRF24L01_delay_us(uint16_t j)
{
	uint32_t i;
	for(;j>0;j--)
		for(i=29;i>0;i--);
}
