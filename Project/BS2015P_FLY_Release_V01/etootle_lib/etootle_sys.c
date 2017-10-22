/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "etootle_sys.h"
#include "etootle_main.h"
//
extern unsigned int system_led_timer_counter,system_timer_1ms_event,system_timer_counter;
unsigned int bs004_sys_timer_period=999;
extern unsigned char BS004_IMU_Output,BS004_Motor_Lock;
//

void BS004_RCC_Configuration(void)		//Բ�㲩ʿ:����ϵͳʱ��
{
  ErrorStatus HSEStartUpStatus;
	//
	NVIC_DeInit();
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div2);
    RCC_PCLK1Config(RCC_HCLK_Div2);
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
}

void BS004_NVIC_Configuration(void)				//Բ�㲩ʿ:�ж�����
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		//Բ�㲩ʿ:ʹ�ܴ���
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);			//Բ�㲩ʿ:ʹ�ܵ�ѹ���
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);				//Բ�㲩ʿ:ʹ�ܵ�ѹDMA
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);			//Բ�㲩ʿ:ʹ��ϵͳʱ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);			//Բ�㲩ʿ:ʹ�ܵ������
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//Բ�㲩ʿ:ʹ��SWD������
	//
	NVIC_RESETPRIMASK();																			//Բ�㲩ʿ:ʹ���ж�
}

void BS004_SYS_LED_Configuration(void)											//Բ�㲩ʿ:LED����
{
	unsigned char i;
	GPIO_InitTypeDef GPIO_InitStructure;
	//

	GPIO_InitStructure.GPIO_Pin = BS004_Bootloader_USB_CON;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BS004_Bootloader_USB_CON_PORT, &GPIO_InitStructure);
	GPIO_SetBits(BS004_Bootloader_USB_CON_PORT, BS004_Bootloader_USB_CON); 	
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MCU_LED;					//Բ�㲩ʿ:����ʹ�õ�LED��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;   		//Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(BS004_MCU_LED_PORT, &GPIO_InitStructure); 
	BS004_MCU_LED_ON();
	//
	for(i=0;i<30;i++)				//Բ�㲩ʿ:��������LED
	{
		BS004_MCU_LED_OFF();
		BS004_Long_Delay(300000);
		BS004_MCU_LED_ON();
		BS004_Long_Delay(300000);
	}
}

void BS004_SYS_Timer_Configuration(void)		
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	//
	TIM_TimeBaseStructure.TIM_ClockDivision = BS004_SYS_TIMER_SYSCLK_DIV;
	TIM_TimeBaseStructure.TIM_Prescaler = BS004_SYS_TIMER_CLK_1MHZ;
	TIM_TimeBaseStructure.TIM_Period = bs004_sys_timer_period;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);		//Բ�㲩ʿ:����PWM���ں�Ƶ��:72*1K/72M=1mS			
	//
	BS004_SYS_NVIC_Configuration();
	TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1,TIM_IT_Update, ENABLE);					//Բ�㲩ʿ:���ж�
	TIM_Cmd(TIM1, ENABLE);													  //Բ�㲩ʿ:����PWM
}

void BS004_SYS_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;			//Բ�㲩ʿ:����PWM�ж����ȼ�	
	//
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void BS004_SYS_Timer_Interrupt(void)					//Բ�㲩ʿ:PWM�жϺ���
{	
	system_timer_1ms_event=1;
}

void BS004_MCU_LED_OFF(void)		
{
	GPIO_SetBits(BS004_MCU_LED_PORT, BS004_MCU_LED);		//Բ�㲩ʿ:Ϩ��LED
}
void BS004_MCU_LED_ON(void)		
{
	GPIO_ResetBits(BS004_MCU_LED_PORT, BS004_MCU_LED);	 //Բ�㲩ʿ:����LED
}
//
void BS004_SYS_LED_TWINKLE(void)										   //Բ�㲩ʿ:��˸LED
{
	if(system_led_timer_counter>1000) system_led_timer_counter=0;
	if(system_led_timer_counter==500) 
	{
		BS004_MCU_LED_OFF();
		BS004_MOTOR_LED_ON();
	}
	else if(system_led_timer_counter==1000) 
	{
		BS004_MCU_LED_ON();
		BS004_MOTOR_LED_OFF();
	}
	//
	if(system_timer_counter>20) 
	{
		system_timer_counter=0;
		if(BS004_IMU_Output && BS004_Motor_Lock) ANBT_SEND_DMP_EULER_DATA(); 
	}	
}

void BS004_Long_Delay(unsigned int nCount) 		  //Բ�㲩ʿ:��ʱ����
{
	for(; nCount != 0; nCount--);
}




