#include "stm32f10x.h"
#include "system_init.h"
#include "led.h"
#include "MPU6050.h"
#include "motor.h"
#include "nRF24L01.h"
#include "quadcopter.h"
#include "pid.h"

#include "quad_uart.h"
#include <stdlib.h>

extern char UART1RxData[50];
extern char pidFresh;
extern struct PID{
	float pryPitch,pryRoll,pryYaw;								//ŷ����ʵ��ֵ���⻷��
	float pryPitchCom,pryRollCom,pryYawCom;				//ŷ����Ҫ��ֵ���⻷��
	float pitchErr,rollErr,yawErr;								//ŷ���ǵ�ǰƫ��
	float pitchErrLast,rollErrLast,yawErrLast;		//ŷ�����ϴ�ƫ��
	float outP,outI,outD;													//�⻷PID����
	float integralPitch,integralRoll,integralYaw;	//�⻷ŷ���ǻ����ۼ�
	
	float gyroX,gyroY,gyroZ;															//���ٶȵ�ǰֵ���ڻ���
	float gyroXCom,gyroYCom,gyroZCom;											//���ٶ�Ҫ��ֵ���ڻ���//�⻷�����
	float gyroXErr,gyroYErr,gyroZErr;											//���ٶȵ�ǰƫ��
	float gyroXErrLast,gyroYErrLast,gyroZErrLast;					//���ٶ��ϴ�ƫ��
	float inP,inI,inD;																		//�ڻ�PID����
	float integralgyroX,integralgyroY,integralgyroZ;			//�ڻ����ٶȻ����ۼ�
	
	float pitchOutput,rollOutput,yawOutput;				//�������
}pid;

/****************************************
��ʼ��������
���룺ϵͳʱ�ӡ�NVIC��Systick��ʱ����LED��MPU6050�����PWM��nRF24L01
��ѡ�����ڡ�
****************************************/
int main()
{
	/*SysClockInit();
	Initial_UART1(57600);
	PrintChar("temp:21.0    fan-state:stop\n");
	PrintChar("temp:22.3    fan-state:stop\n");
	PrintChar("temp:25.2    fan-state:slow\n");
	PrintChar("temp:28.7    fan-state:fast\n");*/
	char tmp[5],i;
	SysClockInit();
	Initial_UART1(57600);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);		//��λ��ռ--һλ��Ӧ
	SystickInit();		//δ����
	LEDInit();
	while(MPU6050_Init())
		PrintChar("MPU6050 failed!\n");
	MoterPWMTimerInit();
	nRF24L01_SPI2_Init();
	while(nRF24L01_Check() == 1)
		PrintChar("nRF24L01 check failed!\n");;
	nRF24L01_RX_Mode();
	
	//TIM_Cmd(TIM3, DISABLE);
	
	LoadParameter();
	//����ն˱�־��FIFO
	nRF24L01_Write_Reg(nRF24L01_WRITE_REG+STATUS, 0xFF);
	nRF24L01_Write_Reg(FLUSH_RX,0xFF);
	SystickEnable();
	PrintChar("System running...\n");
	while(1)
	{
		if(pidFresh)
		{
			for(i=0;i<5;i++)
				tmp[i] = UART1RxData[i];
			pid.outP = atof(tmp);
			
			for(i=0;i<5;i++)
				tmp[i] = UART1RxData[i+6];
			pid.outI = atof(tmp);
			
			for(i=0;i<5;i++)
				tmp[i] = UART1RxData[i+12];
			pid.outD = atof(tmp);
			pidFresh = 0;
		}
	}
}
