#include "system_init.h"

/**
  * @brief  ����ϵͳʱ��.
	* @detail	�����ⲿ����16M��PLL4��Ƶ��64MHz
						SYSCLK = 64M
						HCLK = SYSCLK = 64M
						PCLK2 = HCLK = 64M
						PCLK1 = HCLK/2 = 32M
  * @param  None
  * @retval None
  */
void SysClockInit(void)
{
	uint8_t HSEStartUpStatus;
  RCC_DeInit();
  RCC_HSEConfig(RCC_HSE_ON);
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if(HSEStartUpStatus == SUCCESS)
  {
    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
		
    //FLASH_SetLatency(FLASH_Latency_2);
    //FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_4);
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

/**
  * @brief  ����Systick��ʱ������ʹ��
	* @detail	ʱ��Դ��HCLK/8 = 8MHz
						SysTick->LOAD = 24000:���3ms
  * @param  None
  * @retval None
  */
void SystickInit(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	SysTick->LOAD = 24000;
	NVIC_SetPriority(SysTick_IRQn, 0x00);
	SysTick->VAL = 0; 
	SysTick->CTRL |= 0x02;
}

/**
  * @brief  ʹ��Systick��ʱ��.
	* @detail	
  * @param  None
  * @retval None
  */
void SystickEnable(void)
{
	SysTick->CTRL |= 0x01;
}
