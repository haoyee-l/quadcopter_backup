#include "nRF24L01.h"

const uint8_t TX_ADDRESS[TX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ
const uint8_t RX_ADDRESS[RX_ADR_WIDTH]={0x34,0x43,0x10,0x10,0x01}; //���͵�ַ

/************************************************************************
SPI������غ���
************************************************************************/
//SPI��ʼ������
void SPI1_Init(void)
{	 
	RCC->APB2ENR|=1<<2;       //PORTAʱ��ʹ�� 	 
	RCC->APB2ENR|=1<<12;      //SPI1ʱ��ʹ�� 
		   
	//����ֻ���SPI�ڳ�ʼ��
	GPIOA->CRL&=0x000FFFFF; 
	GPIOA->CRL|=0xBBB00000;		//PA5.6.7����
	GPIOA->ODR|=0x7<<5;    		//PA5.6.7����
		
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
	uint8_t  retry=0;				 
	while((SPI1->SR&1<<1)==0)//�ȴ���������	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=TxData;	 	  //����һ��byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //�ȴ�������һ��byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //�����յ�������				    
}


/************************************************************************
nRF24L01��غ���
************************************************************************/
void nRF24L01_SPI1_Init(void)
{
	RCC->APB2ENR |= 1<<2;    		//ʹ��PORTA��ʱ��
	GPIOA->CRL &= 0xFFFFFF00;		//PA0.1���
	GPIOA->CRL |= 0x00000033; 
	GPIOA->ODR |= 0x00000003;	  //PA0.1 ���1
	
	GPIOA->CRL &= 0xFFFFF0FF;		//PA2����ģʽ
	
	
	SPI1_Init();    						//��ʼ��SPI
	NRF24L01_CE_LOW;			 			//ʹ��24L01
	NRF24L01_CSN_HIGH;					//SPIƬѡȡ��
}

uint8_t NRF24L01_Check(void)
{
	uint8_t buf[5]={0xA5,0xA5,0xA5,0xA5,0xA5};
	uint8_t i;
	SPI1_SetSpeed(SPI_SPEED_8); //spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   	 
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.	
	NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ  
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
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
	uint8_t status;	
	NRF24L01_CSN_LOW;                 	//ʹ��SPI����
	status =SPI1_ReadWriteByte(reg);		//���ͼĴ����� 
	SPI1_ReadWriteByte(value);      		//д��Ĵ�����ֵ
	NRF24L01_CSN_HIGH;                  //��ֹSPI����	   
	return(status);       							//����״ֵ̬
}

//��ȡSPI�Ĵ���ֵ
//reg:Ҫ���ļĴ���
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
	uint8_t reg_val;	    
 	NRF24L01_CSN_LOW;// = 0;          //ʹ��SPI����		
	SPI1_ReadWriteByte(reg);   //���ͼĴ�����
	reg_val=SPI1_ReadWriteByte(0xFF);//��ȡ�Ĵ�������
	NRF24L01_CSN_HIGH;// = 1;          //��ֹSPI����		    
	return(reg_val);           //����״ֵ̬
}

//��ָ��λ��дָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,u8_ctr;	    
 	NRF24L01_CSN_LOW;									      //ʹ��SPI����
	status = SPI1_ReadWriteByte(reg);				//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
	for(u8_ctr=0; u8_ctr<len; u8_ctr++)
		SPI1_ReadWriteByte(*pBuf++); 					//д������	 
	NRF24L01_CSN_HIGH;											//�ر�SPI����
	return status;													//���ض�����״ֵ̬
}

//��ָ��λ�ö���ָ�����ȵ�����
//reg:�Ĵ���(λ��)
//*pBuf:����ָ��
//len:���ݳ���
//����ֵ,�˴ζ�����״̬�Ĵ���ֵ 
uint8_t NRF24L01_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t status,u8_ctr;	       
	NRF24L01_CSN_LOW;				            			//ʹ��SPI����
	status=SPI1_ReadWriteByte(reg);						//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬   	   
 	for(u8_ctr=0;u8_ctr<len;u8_ctr++)
		pBuf[u8_ctr] = SPI1_ReadWriteByte(0xFF);//��������
	NRF24L01_CSN_HIGH;						      			//�ر�SPI����
	return status;      										  //���ض�����״ֵ̬
}
		   
//����NRF24L01����һ������
//txbuf:�����������׵�ַ
//����ֵ:�������״��
uint8_t NRF24L01_TxPacket(uint8_t *txbuf)
{
	uint8_t state;
 	SPI1_SetSpeed(SPI_SPEED_8);															//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	NRF24L01_CE_LOW;
  NRF24L01_Write_Buf(WR_TX_PLOAD, txbuf, TX_PLOAD_WIDTH);	//д���ݵ�TX BUF  32���ֽ�
 	NRF24L01_CE_HIGH;																				//��������	   
	while(NRF24L01_IRQ != 0);																	//�ȴ��������
	state=NRF24L01_Read_Reg(STATUS);  											//��ȡ״̬�Ĵ�����ֵ	   
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS, state); 						//���TX_DS��MAX_RT�жϱ�־
	if(state & MAX_TX)																				//�ﵽ����ط�����
	{
		NRF24L01_Write_Reg(FLUSH_TX,0xFF);										//���TX FIFO�Ĵ��� 
		return MAX_TX; 
	}
	if(state & TX_OK)																				//�������
	{
		return TX_OK;
	}
	return 0xff;																						//����ԭ����ʧ��
}

//����NRF24L01����һ������
//txbuf:��������׵�ַ
//����ֵ:0��������ɣ��������������
uint8_t NRF24L01_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;		    							   
	SPI1_SetSpeed(SPI_SPEED_8); 														//spi�ٶ�Ϊ9Mhz��24L01�����SPIʱ��Ϊ10Mhz��   
	state=NRF24L01_Read_Reg(STATUS);  											//��ȡ״̬�Ĵ�����ֵ    	 
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+STATUS, state); 						//���TX_DS��MAX_RT�жϱ�־
	if(state & RX_OK)																				//���յ�����
	{
		NRF24L01_Read_Buf(RD_RX_PLOAD, rxbuf, RX_PLOAD_WIDTH);//��ȡ����
		NRF24L01_Write_Reg(FLUSH_RX,0xff);										//���RX FIFO�Ĵ��� 
		return 0; 
	}	   
	return 1;																								//û�յ��κ�����
}

//�ú�����ʼ��NRF24L01��RXģʽ
//����RX��ַ,дRX���ݿ��,ѡ��RFƵ��,�����ʺ�LNA HCURR
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
void NRF24L01_RX_Mode(void)
{
	NRF24L01_CE_LOW;	  
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0, (uint8_t *)RX_ADDRESS, RX_ADR_WIDTH);//дRX�ڵ��ַ

	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);    					//ʹ��ͨ��0���Զ�Ӧ��    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01);					//ʹ��ͨ��0�Ľ��յ�ַ  	 
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,0);	     					//����RFͨ��Ƶ��		  
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);//ѡ��ͨ��0����Ч���ݿ�� 	    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x07);					//����TX�������,0db����,2Mbps,���������濪��   
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG, 0x0f);						//���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ 
	NRF24L01_CE_HIGH;																			//CEΪ��,�������ģʽ 
}

//�ú�����ʼ��NRF24L01��TXģʽ
//����TX��ַ,дTX���ݿ��,����RX�Զ�Ӧ��ĵ�ַ,���TX��������,ѡ��RFƵ��,�����ʺ�LNA HCURR
//PWR_UP,CRCʹ��
//��CE��ߺ�,������RXģʽ,�����Խ���������		   
//CEΪ�ߴ���10us,����������.	 
void NRF24L01_TX_Mode(void)
{
	NRF24L01_CE_LOW;	    
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG+TX_ADDR,(uint8_t *)TX_ADDRESS,TX_ADR_WIDTH);//дTX�ڵ��ַ 
	NRF24L01_Write_Buf(NRF24L01_WRITE_REG+RX_ADDR_P0,(uint8_t *)RX_ADDRESS,RX_ADR_WIDTH); //����TX�ڵ��ַ,��ҪΪ��ʹ��ACK	  

	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_AA,0x01);     //ʹ��ͨ��0���Զ�Ӧ��    
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+EN_RXADDR,0x01); //ʹ��ͨ��0�Ľ��յ�ַ  
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+SETUP_RETR,0x1a);//�����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_CH,0);       //����RFͨ��Ϊ40
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+RF_SETUP,0x07);  //����TX�������,0db����,2Mbps,���������濪��   
	NRF24L01_Write_Reg(NRF24L01_WRITE_REG+CONFIG,0x0e);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,���������ж�
	NRF24L01_CE_HIGH;//CEΪ��,10us����������
}
