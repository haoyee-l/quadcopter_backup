/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "stm32f10x_it.h"
//
#define AnBT_MOTOR_LED_A				GPIO_Pin_15		//PA15
#define AnBT_MOTOR_LED_A_PORT		GPIOA
#define AnBT_MOTOR_LED_B				GPIO_Pin_9		//PB9
#define AnBT_MOTOR_LED_B_PORT		GPIOB
#define AnBT_MOTOR_LED_C				GPIO_Pin_5		//PB5
#define AnBT_MOTOR_LED_C_PORT		GPIOB
#define AnBT_MOTOR_LED_D				GPIO_Pin_2		//PB2
#define AnBT_MOTOR_LED_D_PORT		GPIOD

void BS004_LED_GPIO_Configuration(void);
void BS004_MOTOR_LED_ON(void);
void BS004_MOTOR_LED_OFF(void);


