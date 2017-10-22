#include "pid.h"

struct PID{
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
}pid;

void QuadPID(void)	//PID
{
	//pitch
	pid.pitchErr = pid.pryPitchCom - pid.pryPitch;
	pid.integralPitch += pid.pitchErr;
	if(pid.integralPitch>500)		pid.integralPitch = 500;
	if(pid.integralPitch<-500)	pid.integralPitch = -500;
	pid.accelXCom = pid.outP*pid.pitchErr + pid.outI*pid.integralPitch + pid.outD*(pid.pitchErr-pid.pitchErrLast);
	pid.pitchErrLast = pid.pitchErr;
	
	pid.accelXErr = pid.accelXCom - pid.accelX;
	pid.integralAccelX += pid.accelXErr;
	if(pid.integralAccelX>500)	pid.integralAccelX = 500;
	if(pid.integralAccelX<-500)	pid.integralAccelX = -500;
	pid.pitchOutput = pid.inP*pid.accelXErr + pid.inI*pid.integralAccelX + pid.inD*(pid.accelXErr-pid.accelXErrLast);
	if(pid.pitchOutput>4000)	pid.pitchOutput = 4000;
	if(pid.pitchOutput<-4000)	pid.pitchOutput = -4000;
	pid.accelXErrLast = pid.accelXErr;
	if(pid.pitchOutput>0)
		motor.motor_3 += pid.pitchOutput;
	else
		motor.motor_1 -= pid.pitchOutput;
	
	//Roll
	pid.rollErr = pid.pryRollCom - pid.pryRoll;
	pid.integralRoll += pid.rollErr;
	if(pid.integralRoll>500)	pid.integralRoll = 500;
	if(pid.integralRoll<-500)	pid.integralRoll = -500;
	pid.accelYCom = pid.outP*pid.rollErr + pid.outI*pid.integralRoll + pid.outD*(pid.rollErr-pid.rollErrLast);
	pid.rollErrLast = pid.rollErr;
	
	pid.accelYErr = pid.accelYCom - pid.accelY;
	pid.integralAccelY += pid.accelYErr;
	if(pid.integralAccelY>500)	pid.integralAccelY = 500;
	if(pid.integralAccelY<-500)	pid.integralAccelY = -500;
	pid.rollOutput = pid.inP*pid.accelYErr + pid.inI*pid.integralAccelY + pid.inD*(pid.accelYErr-pid.accelYErrLast);
	if(pid.rollOutput>4000)		pid.rollOutput = 4000;
	if(pid.rollOutput<-4000)	pid.rollOutput = -4000;
	pid.accelYErrLast = pid.accelYErr;
	if(pid.rollOutput>0)
		motor.motor_4 += pid.rollOutput;
	else
		motor.motor_2 -= pid.rollOutput;
	
	//Yaw
	/*pid.yawErr = pid.pryYawCom - pid.pryYaw;
	pid.integralYaw += pid.yawErr;
	if(pid.integralYaw>500)		pid.integralYaw = 500;
	if(pid.integralYaw<-500)	pid.integralYaw = -500;
	pid.accelZCom = pid.outP*pid.yawErr + pid.outI*pid.integralYaw + pid.outD*(pid.yawErr-pid.yawErrLast);
	pid.yawErrLast = pid.yawErr;
	
	pid.accelZErr = pid.accelZCom - pid.accelZ;
	pid.integralAccelZ += pid.accelZErr;
	if(pid.integralAccelZ>500)	pid.integralAccelZ = 500;
	if(pid.integralAccelZ<-500)	pid.integralAccelZ = -500;
	pid.yawOutput = pid.inP*pid.accelZErr + pid.inI*pid.integralAccelZ + pid.inD*(pid.accelZErr-pid.accelZErrLast);
	if(pid.yawOutput>4000)	pid.yawOutput = 4000;
	if(pid.yawOutput<-4000)	pid.yawOutput = -4000;
	pid.accelZErrLast = pid.accelZErr;*/
}
