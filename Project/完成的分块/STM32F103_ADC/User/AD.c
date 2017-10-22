#include "handleAD.h"

uint32_t ADCData[4];

/**
  * @brief  ��ʼ��ADC����ʼת��
	* @detail	
  * @param  None
  * @retval None
  */
void ADC12_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	ADC_Port_Init();
	ADC_DMA_Init();
	
	ADC1->CR2 &= 0xFE;					//�ر�ADC
	
	ADC1->CR1 |= 0x120;		//SCAN + EOCIE
	ADC1->CR2 &= 0x800;		//�����Ҷ���
	ADC1->CR2 |= 0x01;		//����ת��ģʽ
	//ADC1->CR2 |= 0x0100;	//ʹ��DMA
	ADC1->SMPR2 &= 0x00;		//����ʱ��1.5����
	ADC1->SQR1 |= 0x300000;
	ADC1->SQR3 |= 0x8062;	//ת��˳��2301���ֱ��������ң������º�����
	ADCData[0] = 22;
	ADC1->CR2 |= 0x01;
	adc_delay();
	ADC1->CR2 |= 0x08;					//���ø�λУ׼
	while((ADC1->CR2)&0x08);		//�ȴ���λ���
	ADC1->CR2 |= 0x04;					//����CALλ
	while((ADC1->CR2)&0x04);		//�ȴ�У׼���
	//ADC1->CR2 |= 0x01;
	//ADC1->CR2 |= 0x400000;
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void ADC_Port_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void ADC_DMA_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = 0x4001244C;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCData;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 4;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void adc_delay(void)
{
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
}
