/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "etootle_led.h" 
//
void BS004_LED_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);
	//
  GPIO_InitStructure.GPIO_Pin = AnBT_MOTOR_LED_A;				//Բ�㲩ʿ:����ʹ�õ�LED��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;   		//Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(AnBT_MOTOR_LED_A_PORT, &GPIO_InitStructure); 
	//
  GPIO_InitStructure.GPIO_Pin = AnBT_MOTOR_LED_B;				//Բ�㲩ʿ:����ʹ�õ�LED��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;   		//Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(AnBT_MOTOR_LED_B_PORT, &GPIO_InitStructure); 
	//
	GPIO_InitStructure.GPIO_Pin = AnBT_MOTOR_LED_C;				//Բ�㲩ʿ:����ʹ�õ�LED��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;   		//Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(AnBT_MOTOR_LED_C_PORT, &GPIO_InitStructure); 
	//
	GPIO_InitStructure.GPIO_Pin = AnBT_MOTOR_LED_D;				//Բ�㲩ʿ:����ʹ�õ�LED��
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;   		//Բ�㲩ʿ:����LED�������������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	  	//Բ�㲩ʿ:����LED��Ϊ���
  GPIO_Init(AnBT_MOTOR_LED_D_PORT, &GPIO_InitStructure); 	
	//
	BS004_MOTOR_LED_OFF();
}

void BS004_MOTOR_LED_ON(void)		
{
	GPIO_SetBits(AnBT_MOTOR_LED_A_PORT, AnBT_MOTOR_LED_A);		//Բ�㲩ʿ:����LED
	GPIO_SetBits(AnBT_MOTOR_LED_B_PORT, AnBT_MOTOR_LED_B);		//Բ�㲩ʿ:����LED
	GPIO_SetBits(AnBT_MOTOR_LED_C_PORT, AnBT_MOTOR_LED_C);		//Բ�㲩ʿ:����LED
	GPIO_SetBits(AnBT_MOTOR_LED_D_PORT, AnBT_MOTOR_LED_D);		//Բ�㲩ʿ:����LED
}

void BS004_MOTOR_LED_OFF(void)		
{
	GPIO_ResetBits(AnBT_MOTOR_LED_A_PORT, AnBT_MOTOR_LED_A);	//Բ�㲩ʿ:Ϩ��LED
	GPIO_ResetBits(AnBT_MOTOR_LED_B_PORT, AnBT_MOTOR_LED_B);	//Բ�㲩ʿ:Ϩ��LED
	GPIO_ResetBits(AnBT_MOTOR_LED_C_PORT, AnBT_MOTOR_LED_C);	//Բ�㲩ʿ:Ϩ��LED
	GPIO_ResetBits(AnBT_MOTOR_LED_D_PORT, AnBT_MOTOR_LED_D);	//Բ�㲩ʿ:Ϩ��LED
}

