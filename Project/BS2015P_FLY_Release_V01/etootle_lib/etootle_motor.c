/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "etootle_motor.h" 
//
extern float bs004_fly_m1,bs004_fly_m2,bs004_fly_m3,bs004_fly_m4;
signed short bs004_fly_m1_out=0,bs004_fly_m2_out=0,bs004_fly_m3_out=0,bs004_fly_m4_out=0;	
//
extern float bs004_angle_cur_pitch,bs004_angle_cur_roll,bs004_angle_cur_yaw;
extern float bs004_angle_last_pitch,bs004_angle_last_roll,bs004_angle_last_yaw;
//
unsigned int bs004_motor_pwm_period=0;
unsigned int BS004_Motor_Scale=0;
unsigned int Motor_BS004_M1=0,Motor_BS004_M2=0,Motor_BS004_M3=0,Motor_BS004_M4=0;
//
void BS004_MOTOR_PWM_ON(void)		
{
	GPIO_SetBits(BS004_MOTOR_PWM_PORT, BS004_MOTOR_PWM_M1);			//Բ�㲩ʿ:��PWM���
	GPIO_SetBits(BS004_MOTOR_PWM_PORT, BS004_MOTOR_PWM_M2);			//Բ�㲩ʿ:��PWM���
	GPIO_SetBits(BS004_MOTOR_PWM_PORT, BS004_MOTOR_PWM_M3);			//Բ�㲩ʿ:��PWM���
	GPIO_SetBits(BS004_MOTOR_PWM_PORT, BS004_MOTOR_PWM_M4);			//Բ�㲩ʿ:��PWM���
}
void BS004_MOTOR_PWM_OFF(void)		
{
	GPIO_ResetBits(BS004_MOTOR_PWM_PORT, BS004_MOTOR_PWM_M1);		//Բ�㲩ʿ:�ر�PWM���
	GPIO_ResetBits(BS004_MOTOR_PWM_PORT, BS004_MOTOR_PWM_M2);		//Բ�㲩ʿ:�ر�PWM���
	GPIO_ResetBits(BS004_MOTOR_PWM_PORT, BS004_MOTOR_PWM_M3);		//Բ�㲩ʿ:�ر�PWM���
	GPIO_ResetBits(BS004_MOTOR_PWM_PORT, BS004_MOTOR_PWM_M4);		//Բ�㲩ʿ:�ر�PWM���
}

//===============================================================

void BS004_Motor_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MOTOR_PWM_M1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //Բ�㲩ʿ:����PWM�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  //Բ�㲩ʿ:����PWM��Ϊ���
  GPIO_Init(BS004_MOTOR_PWM_PORT, &GPIO_InitStructure); 
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MOTOR_PWM_M2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //Բ�㲩ʿ:����PWM�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  //Բ�㲩ʿ:����PWM��Ϊ���
  GPIO_Init(BS004_MOTOR_PWM_PORT, &GPIO_InitStructure);
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MOTOR_PWM_M3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //Բ�㲩ʿ:����PWM�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  //Բ�㲩ʿ:����PWM��Ϊ���
  GPIO_Init(BS004_MOTOR_PWM_PORT, &GPIO_InitStructure);
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MOTOR_PWM_M4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //Բ�㲩ʿ:����PWM�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  //Բ�㲩ʿ:����PWM��Ϊ���
  GPIO_Init(BS004_MOTOR_PWM_PORT, &GPIO_InitStructure);
	//	
	BS004_MOTOR_PWM_OFF();															//Բ�㲩ʿ:����PWM�����Ϊ��
}

void BS004_Motor_PWM_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MOTOR_PWM_M1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(BS004_MOTOR_PWM_PORT, &GPIO_InitStructure); 
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MOTOR_PWM_M2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(BS004_MOTOR_PWM_PORT, &GPIO_InitStructure);
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MOTOR_PWM_M3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(BS004_MOTOR_PWM_PORT, &GPIO_InitStructure);
	//
	GPIO_InitStructure.GPIO_Pin = BS004_MOTOR_PWM_M4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(BS004_MOTOR_PWM_PORT, &GPIO_InitStructure);
	//	
	//Բ�㲩ʿ:PWMƵ��=BS004_MOTOR_PWM_CLK_36MHZ/��BS004_MOTOR_PWM_PERIOD+1)
	TIM_TimeBaseStructure.TIM_ClockDivision = BS004_MOTOR_PWM_SYSCLK_DIV;
	TIM_TimeBaseStructure.TIM_Prescaler = BS004_MOTOR_PWM_CLK_72MHZ;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = bs004_motor_pwm_period; 
  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);								//Բ�㲩ʿ:����PWM���ں�Ƶ��			
	//
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Motor_BS004_M3;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;         
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM8, &TIM_OCInitStructure);											//Բ�㲩ʿ:����PWMռ�ձ�		
	//
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Motor_BS004_M2;
  TIM_OC2Init(TIM8, &TIM_OCInitStructure);											//Բ�㲩ʿ:����PWMռ�ձ�		
	//
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Motor_BS004_M4;
  TIM_OC3Init(TIM8, &TIM_OCInitStructure);											//Բ�㲩ʿ:����PWMռ�ձ�		
	//
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Motor_BS004_M1;
  TIM_OC4Init(TIM8, &TIM_OCInitStructure);											//Բ�㲩ʿ:����PWMռ�ձ�		
	//
	BS004_Motor_NVIC_Configuration();					//Բ�㲩ʿ:����PWM�ж����ȼ�	
	TIM_Cmd(TIM8, ENABLE);										//Բ�㲩ʿ:����PWM
	TIM_CtrlPWMOutputs(TIM8,ENABLE);          //Բ�㲩ʿ:����PWM���
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("finish to init motor device.");					//Բ�㲩ʿ:��ʼ��PWM IO
	BS004_COM1_Send_Str_Tail();	
}

void BS004_Motor_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;			//Բ�㲩ʿ:����PWM�ж����ȼ�	
	//
	NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//===============================================================
void BS004_Motor_Interrupt(void)					//Բ�㲩ʿ:PWM�жϺ���
{	
	TIM_SetCompare4(TIM8,Motor_BS004_M1);		//Բ�㲩ʿ:����PWMռ�ձ�  
	TIM_SetCompare2(TIM8,Motor_BS004_M2);	  //Բ�㲩ʿ:����PWMռ�ձ�  
	TIM_SetCompare1(TIM8,Motor_BS004_M3);   //Բ�㲩ʿ:����PWMռ�ձ�      
	TIM_SetCompare3(TIM8,Motor_BS004_M4);		//Բ�㲩ʿ:����PWMռ�ձ�  	    
}

signed short BS004_Motor_Speed_Scale(float motor_speed_input)
{
	float motor_speed_output;
	//
	if(motor_speed_input>BS004_FLY_MAX_OUT)
		motor_speed_output=BS004_FLY_MAX_OUT;
	else if(motor_speed_input<BS004_FLY_MIN_OUT)
		motor_speed_output=BS004_FLY_MIN_OUT;
	else 
		motor_speed_output=motor_speed_input;
	return motor_speed_output;
}

void BS004_Motor_Reset(void)
{
	bs004_fly_m1=0;
	bs004_fly_m2=0;	
	bs004_fly_m3=0;
	bs004_fly_m4=0;
	//
	bs004_angle_cur_pitch=0;
	bs004_angle_cur_roll=0;
	bs004_angle_cur_yaw=0;
	bs004_angle_last_pitch=0;
	bs004_angle_last_roll=0;	
	bs004_angle_last_yaw=0;		
	//
	Motor_BS004_M1=BS004_FLY_MIN_OUT;
	Motor_BS004_M2=BS004_FLY_MIN_OUT;
	Motor_BS004_M3=BS004_FLY_MIN_OUT;
	Motor_BS004_M4=BS004_FLY_MIN_OUT;
	//
	TIM_SetCompare4(TIM8,0);		//Բ�㲩ʿ:����PWMռ�ձ�  
	TIM_SetCompare2(TIM8,0);	  //Բ�㲩ʿ:����PWMռ�ձ�  
	TIM_SetCompare1(TIM8,0);   //Բ�㲩ʿ:����PWMռ�ձ�      
	TIM_SetCompare3(TIM8,0);		//Բ�㲩ʿ:����PWMռ�ձ� 
}






