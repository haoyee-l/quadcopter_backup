#include "pida.h"

extern struct MOTOR{
	int16_t motor_1;
	int16_t motor_2;
	int16_t motor_3;
	int16_t motor_4;
}motor;

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

void QuadPIDA(void)	//PID
{
	//pitch
	/*pid.pitchErr = pid.pryPitchCom - pid.pryPitch;
	pid.integralPitch += pid.pitchErr;
	if(pid.integralPitch>300)		pid.integralPitch = 300;
	if(pid.integralPitch<-300)	pid.integralPitch = -300;
	pid.pitchOutput = pid.outP*pid.pitchErr + pid.outI*pid.integralPitch + pid.outD*(pid.pitchErr-pid.pitchErrLast);
	pid.pitchErrLast = pid.pitchErr;
	
		motor.motor_2 += pid.pitchOutput;
		motor.motor_3 += pid.pitchOutput;
		motor.motor_1 -= pid.pitchOutput;
		motor.motor_4 -= pid.pitchOutput;*/
	
	//Roll
	pid.rollErr = pid.pryRollCom - pid.pryRoll;
	pid.integralRoll += pid.rollErr;
	if(pid.integralRoll>500)	pid.integralRoll = 500;
	if(pid.integralRoll<-500)	pid.integralRoll = -500;
	pid.rollOutput = pid.outP*pid.rollErr + pid.outI*pid.integralRoll + pid.outD*(pid.rollErr-pid.rollErrLast);
	pid.rollErrLast = pid.rollErr;
	

		motor.motor_1 += pid.rollOutput;
		motor.motor_2 += pid.rollOutput;
		motor.motor_3 -= pid.rollOutput;
		motor.motor_4 -= pid.rollOutput;
}

void PIDClear(void)
{
	pid.pitchErr = 0;
	pid.rollErr = 0;
	pid.yawErr = 0;
	pid.pitchErrLast = 0;
	pid.rollErrLast = 0;
	pid.yawErrLast = 0;
	pid.integralPitch = 0;
	pid.integralRoll = 0;
	pid.integralYaw = 0;
	pid.pitchOutput = 0;
	pid.rollOutput = 0;
}
