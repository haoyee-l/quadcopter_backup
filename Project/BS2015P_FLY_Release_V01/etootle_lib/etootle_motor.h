/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include <math.h>
#include <stdlib.h>	 
#include "stm32f10x_it.h"
#include "etootle_bluetooth.h"
//
#define BS004_MOTOR_PWM_M1			GPIO_Pin_9		  //PA9(M1)
#define BS004_MOTOR_PWM_M2			GPIO_Pin_7		//PB7(M2)
#define BS004_MOTOR_PWM_M3			GPIO_Pin_6		//PB6(M3)
#define BS004_MOTOR_PWM_M4			GPIO_Pin_8		//PB8(M4)
#define BS004_MOTOR_PWM_PORT		GPIOC
//
#define BS004_MOTOR_PWM_SYSCLK_DIV	0
#define BS004_MOTOR_PWM_CLK_72MHZ  	1
//
#define BS004_FLY_MAX_OUT 999
#define BS004_FLY_MIN_OUT 15
//
void BS004_Motor_GPIO_Configuration(void);
void BS004_Motor_PWM_Configuration(void);
void BS004_Motor_NVIC_Configuration(void);
//
void BS004_Motor_Interrupt(void);
//
void BS004_Motor_Reset(void);
signed short BS004_Motor_Speed_Scale(float motor_speed_input);




