#include "stm32f10x.h"
#include "system_init.h"

#include <stdlib.h>
#include "quad_uart.h"
#include "delay.h"

extern char UART1RxData[50];
extern char pidFresh;

float P,I,D;

/****************************************
��ʼ��������
���룺ϵͳʱ�ӡ�NVIC��Systick��ʱ����LED��MPU6050�����PWM��nRF24L01
��ѡ�����ڡ�
****************************************/
int main()
{
	char tmp[5],i;
	SysClockInit();
	Initial_UART1(57600);
	PrintChar("READY\n");
	while(1)
	{
		if(pidFresh)
		{
			for(i=0;i<5;i++)
				tmp[i] = UART1RxData[i];
			P = atof(tmp);
			
			for(i=0;i<5;i++)
				tmp[i] = UART1RxData[i+6];
			I = atof(tmp);
			
			for(i=0;i<5;i++)
				tmp[i] = UART1RxData[i+12];
			D = atof(tmp);
			pidFresh = 0;
		}
	}
}
