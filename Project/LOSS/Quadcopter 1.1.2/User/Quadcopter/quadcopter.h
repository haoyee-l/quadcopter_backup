#include "stm32f10x.h"
#include "stdio.h"

uint32_t IntNum = 0;		//5ms�жϴ�������
uint8_t RxData[11];			//24L01�յ���ԭʼ����

/*********************************
							M1(P16.T3-1)
					M2+			M4
							M3+
*********************************/
typedef struct {
	uint16_t motor_1;
	uint16_t motor_2;
	uint16_t motor_3;
	uint16_t motor_4;
}MOTOR;

typedef struct {
	float pryPitch,pryRoll,pryYaw;								//ŷ����ʵ��ֵ���⻷��
	float pryPitchCom,pryRollCom,pryYawCom;				//ŷ����Ҫ��ֵ���⻷��
	float pitchErr,rollErr,yawErr;								//ŷ���ǵ�ǰƫ��
	float pitchErrLast,rollErrLast,yawErrLast;		//ŷ�����ϴ�ƫ��
	float outP,outI,outD;													//�⻷PID����
	float integralPitch,integralRoll,integralYaw;	//�⻷ŷ���ǻ����ۼ�
	
	float accelX,accelY,accelZ;															//���ٶȵ�ǰֵ���ڻ���
	float accelXCom,accelYCom,accelZCom;										//���ٶ�Ҫ��ֵ���ڻ���//�⻷�����
	float accelXErr,accelYErr,accelZErr;										//���ٶȵ�ǰƫ��
	float accelXErrLast,accelYErrLast,accelZErrLast;				//���ٶ��ϴ�ƫ��
	float inP,inI,inD;																			//�ڻ�PID����
	float integralAccelX,integralAccelY,integralAccelZ;			//�ڻ����ٶȻ����ۼ�
	
	float pitchOutput,rollOutput,yawOutput;			//�������
}PID;

void ControlPrepare(void);
void LoadParameter(void);
