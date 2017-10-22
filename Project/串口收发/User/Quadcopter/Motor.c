#include "motor.h"

#define MOTOR_MAX 600	//�����ܴ���639

extern struct MOTOR{
	int16_t motor_1;
	int16_t motor_2;
	int16_t motor_3;
	int16_t motor_4;
}motor;

/**
  * @brief  ��ʼ�������PWM��ʱ��.
	* @detail	����ʱ�ӣ�=PCLK2=64M
						
	* @resut	���úú�δ�������ɵ���TIM_Cmd(TIM3, ENABLE)������ʱ����
						ͨ��TIM3->CCRx���ı�ռ�ձ�.
  * @param  None
  * @retval None
  */
void MoterPWMTimerInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	TIM_OCInitTypeDef  TIM_OCInitStructure;  
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Prescaler = 0;													//TIMERʱ��Ԥ��Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 639;														//�Զ���װֵ//��ʱ�����ֵ==>PWMƵ��=����Ƶ��/(�Զ���װֵ+1)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;		//��Ӧͨ�����ʹ��
	TIM_OCInitStructure.TIM_Pulse = 0;															//ռ�ձ�
	TIM_OCInitStructure.TIM_OCPolarity =TIM_OCPolarity_High;				//Ĭ������ߵ�ƽ������16.7.20��֤����Ϲ�ģ�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	
	TIM3->CCR1 = 0;
	TIM3->CCR2 = 0;
	TIM3->CCR3 = 0;
	TIM3->CCR4 = 0;
  TIM_Cmd(TIM3, ENABLE);		//������λ�ÿ���
}

void TimerPWMControl(void)
{
	if(motor.motor_1 < 0)
		motor.motor_1 = 0;
	if(motor.motor_2 < 0)
		motor.motor_2 = 0;
	if(motor.motor_3 < 0)
		motor.motor_3 = 0;
	if(motor.motor_4 < 0)
		motor.motor_4 = 0;
	
	if(motor.motor_1 > MOTOR_MAX)
		motor.motor_1 = MOTOR_MAX;
	if(motor.motor_2 > MOTOR_MAX)
		motor.motor_2 = MOTOR_MAX;
	if(motor.motor_3 > MOTOR_MAX)
		motor.motor_3 = MOTOR_MAX;
	if(motor.motor_4 > MOTOR_MAX)
		motor.motor_4 = MOTOR_MAX;
	
	TIM3->CCR1 = motor.motor_1;
	TIM3->CCR2 = motor.motor_2;
	TIM3->CCR3 = motor.motor_3;
	TIM3->CCR4 = motor.motor_4;
}
