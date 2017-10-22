#include "quadcopter.h"

uint32_t IntNum = 0;		//5ms�жϴ�������
uint8_t RxData[11];			//24L01�յ���ԭʼ����

int16_t Power = 0, PowerLast = 0, PowerErr = 0;
uint8_t LockState = LOCKED;

/*********************************
							M1(P16.T3-1)
					M2+			M4
							M3+
*********************************/
struct MOTOR{
	int16_t motor_1;
	int16_t motor_2;
	int16_t motor_3;
	int16_t motor_4;
}motor={0,0,0,0};

struct PID{
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
}pid={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/*****����******************************************************************************************************************************/
/**
  * @brief  ���յ���ԭʼ�������ݽ���ת��.
	* @detail	��RxData[]������ת�����浽pid.pryPitchCom,pryRollCom,pryYawCom��
	* @resut	
  * @param  None
  * @retval None
  */
void ControlPrepare(void)
{
	if(RxData[0] == 0xA5)	//�յ��Ƕȿ�������
	{
		Power = (int16_t)( (RxData[1]<<8)|RxData[2] );
		pid.pryPitchCom = ((float)(int16_t)( (RxData[3]<<8)|RxData[4] ))/10;
		pid.pryRollCom = ((float)(int16_t)( (RxData[5]<<8)|RxData[6] ))/10;
		pid.pryYawCom = ((float)(int16_t)( (RxData[7]<<8)|RxData[8] ))/10;
		
		PowerErr = Power - PowerLast;	//����λ����ת��Ϊ���Ų�ֵ
		PowerLast = Power;
		
		motor.motor_1 += PowerErr;		//�����Ų�ֵ����
		motor.motor_2 += PowerErr;
		motor.motor_3 += PowerErr;
		motor.motor_4 += PowerErr;
		if(motor.motor_1 < 0)
			motor.motor_1 = 0;
		if(motor.motor_2 < 0)
			motor.motor_2 = 0;
		if(motor.motor_3 < 0)
			motor.motor_3 = 0;
		if(motor.motor_4 < 0)
			motor.motor_4 = 0;
	}
}

/**
  * @brief  װ�ز���.
	* @detail	װ��pid����������������ֵ��
	* @resut	
  * @param  None
  * @retval None
  */
void LoadParameter(void)
{
	pid.outP = 1;
	pid.outI = 0;
	pid.outD = 0;
	pid.inP = 0;
	pid.inI = 0;
	pid.inD = 0;
}
