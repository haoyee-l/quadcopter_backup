#include "pid.h"

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

void QuadPID(void)	//PID
{
	//pitch
	pid.pitchErr = pid.pryPitchCom - pid.pryPitch;
	pid.integralPitch += pid.pitchErr;
	if(pid.integralPitch>100)		pid.integralPitch = 100;
	if(pid.integralPitch<-100)	pid.integralPitch = -100;
	pid.gyroXCom = pid.outP*pid.pitchErr + pid.outI*pid.integralPitch + pid.outD*(pid.pitchErr-pid.pitchErrLast);
	pid.pitchErrLast = pid.pitchErr;
	
	pid.gyroXErr = pid.gyroXCom - pid.gyroX;
	pid.integralgyroX += pid.gyroXErr;
	if(pid.integralgyroX>100)	pid.integralgyroX = 100;
	if(pid.integralgyroX<-100)	pid.integralgyroX = -100;
	pid.pitchOutput = pid.inP*pid.gyroXErr + pid.inI*pid.integralgyroX + pid.inD*(pid.gyroXErr-pid.gyroXErrLast);
	if(pid.pitchOutput>250)	pid.pitchOutput = 250;
	if(pid.pitchOutput<-250)	pid.pitchOutput = -250;
	pid.gyroXErrLast = pid.gyroXErr;
	if(pid.pitchOutput>0)
	{
		motor.motor_2 += pid.pitchOutput;
		motor.motor_3 += pid.pitchOutput;
	}
	else
	{
		motor.motor_1 -= pid.pitchOutput;
		motor.motor_4 -= pid.pitchOutput;
	}
	
	//Roll
	pid.rollErr = pid.pryRollCom - pid.pryRoll;
	pid.integralRoll += pid.rollErr;
	if(pid.integralRoll>100)	pid.integralRoll = 100;
	if(pid.integralRoll<-100)	pid.integralRoll = -100;
	pid.gyroYCom = pid.outP*pid.rollErr + pid.outI*pid.integralRoll + pid.outD*(pid.rollErr-pid.rollErrLast);
	pid.rollErrLast = pid.rollErr;
	
	pid.gyroYErr = pid.gyroYCom - pid.gyroY;
	pid.integralgyroY += pid.gyroYErr;
	if(pid.integralgyroY>100)	pid.integralgyroY = 100;
	if(pid.integralgyroY<-100)	pid.integralgyroY = -100;
	pid.rollOutput = pid.inP*pid.gyroYErr + pid.inI*pid.integralgyroY + pid.inD*(pid.gyroYErr-pid.gyroYErrLast);
	if(pid.rollOutput>250)		pid.rollOutput = 250;
	if(pid.rollOutput<-250)	pid.rollOutput = -250;
	pid.gyroYErrLast = pid.gyroYErr;
	if(pid.rollOutput>0)
	{
		motor.motor_3 += pid.rollOutput;
		motor.motor_4 += pid.rollOutput;
	}
	else
	{
		motor.motor_1 -= pid.rollOutput;
		motor.motor_2 -= pid.rollOutput;
	}
	
	//Yaw
	/*pid.yawErr = pid.pryYawCom - pid.pryYaw;
	pid.integralYaw += pid.yawErr;
	if(pid.integralYaw>100)		pid.integralYaw = 100;
	if(pid.integralYaw<-100)	pid.integralYaw = -100;
	pid.gyroZCom = pid.outP*pid.yawErr + pid.outI*pid.integralYaw + pid.outD*(pid.yawErr-pid.yawErrLast);
	pid.yawErrLast = pid.yawErr;
	
	pid.gyroZErr = pid.gyroZCom - pid.gyroZ;
	pid.integralgyroZ += pid.gyroZErr;
	if(pid.integralgyroZ>100)	pid.integralgyroZ = 100;
	if(pid.integralgyroZ<-100)	pid.integralgyroZ = -100;
	pid.yawOutput = pid.inP*pid.gyroZErr + pid.inI*pid.integralgyroZ + pid.inD*(pid.gyroZErr-pid.gyroZErrLast);
	if(pid.yawOutput>250)	pid.yawOutput = 250;
	if(pid.yawOutput<-250)	pid.yawOutput = -250;
	pid.gyroZErrLast = pid.gyroZErr;*/
}
