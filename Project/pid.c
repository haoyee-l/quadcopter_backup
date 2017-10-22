#include "CONTROL.h" 
#include "IMU1.h"        
#include "moto.h"
#include "RFdate.h"
#include <math.h>
extern T_RC_Data	Rc_D;                //ң��ͨ������;

extern u8 txbuf[4];         //���ͻ���
extern u8 rxbuf[4];         //���ջ���
extern u16 test1[3]; //���յ�NRf24L01����
extern S_INT16_XYZ ACC_F,GYRO_F;

PID PID_ROL,PID_PIT,PID_YAW;

extern S_INT16_XYZ	MPU6050_ACC_LAST,MPU6050_GYRO_LAST;        

int Motor_Ele=0;                                           //��������
int Motor_Ail=0;                                           //�������

//u8 ARMED = 0;

//float rol_i=0,pit_i=0,yaw_p=0;
float thr=0;

S_FLOAT_XYZ EXP_ANGLE ,DIF_ANGLE;
PID1 PID_Motor;
/*********************************/
float Pitch_i,Roll_i,Yaw_i;                                   //������
float Pitch_old,Roll_old,Yaw_old;                 //�Ƕȱ���
float Pitch_d,Roll_d,Yaw_d;          //΢����
float RC_Pitch,RC_Roll,RC_Yaw;                       //��̬��
float Pitch_shell_out,Roll_shell_out,Yaw_shell_out;//�⻷�����
        //�⻷PID����
float Pitch_shell_kp=280;//30 140
float Pitch_shell_kd=0;//
float Pitch_shell_ki=0;//
/*********************************/
float Roll_shell_kp=250;//30
float Roll_shell_kd=0;//10                 
float Roll_shell_ki=0;//0.08
/*********************************/
float Yaw_shell_kp=1.5;//10;//30
float Yaw_shell_kd=0;//10                 
float Yaw_shell_ki=0;//0.08;//0.08
float Gyro_radian_old_x,Gyro_radian_old_y,Gyro_radian_old_z;//�����Ǳ���
float pitch_core_kp_out,pitch_core_kd_out,Roll_core_kp_out,Roll_core_kd_out,Yaw_core_kp_out,Yaw_core_kd_out;//�ڻ��������
float Pitch_core_out,Roll_core_out,Yaw_core_out;//�ڻ������        
        
//�ڻ�PID����
//float Pitch_core_kp=0.040;
//float Pitch_core_kd=0.008;////0.007;//0.07;
float Pitch_core_kp=0.040;
float Pitch_core_kd=0.002;////0.007;//0.07;

float Roll_core_kp=0.040;//;
float Roll_core_kd=0.002;////0.007;//06;//0.07;

float Yaw_core_kp=0.046;//;
float Yaw_core_kd=0.012;////0.007;//06;//0.07;


int16_t moto1=0,moto2=0,moto3=0,moto4=0;

float tempjd=0;
void CONTROL(float rol, float pit, float yaw)
{
  RC_Pitch=(Rc_D.PITCH-1500)/20;
////////////////////////�⻷�ǶȻ�(PID)///////////////////////////////
  Pitch_i+=(Q_ANGLE.Pitch-RC_Pitch);
//-------------Pitch�����޷�----------------//
  if(Pitch_i>300) Pitch_i=300;
  else if(Pitch_i<-300) Pitch_i=-300;
//-------------Pitch΢��--------------------//
  Pitch_d=Q_ANGLE.Pitch-Pitch_old;
//-------------Pitch  PID-------------------//
  Pitch_shell_out = Pitch_shell_kp*(Q_ANGLE.Pitch-RC_Pitch) + Pitch_shell_ki*Pitch_i + Pitch_shell_kd*Pitch_d;
//�Ƕȱ���
  Pitch_old=Q_ANGLE.Pitch;
/*********************************************************/        
        
	RC_Roll=(Rc_D.ROLL-1500)/20;
	Roll_i+=(Q_ANGLE.Rool-RC_Roll);
//-------------Roll�����޷�----------------//
  if(Roll_i>300)
		Roll_i=300;
  else if
		(Roll_i<-300) Roll_i=-300;
//-------------Roll΢��--------------------//
  Roll_d=Q_ANGLE.Rool-Roll_old;
//-------------Roll  PID-------------------//
  Roll_shell_out  = Roll_shell_kp*(Q_ANGLE.Rool-RC_Roll) + Roll_shell_ki*Roll_i + Roll_shell_kd*Roll_d;
//------------Roll�Ƕȱ���------------------//
  Roll_old=Q_ANGLE.Rool;
        
        
  RC_Yaw=(Rc_D.YAW-1500)*10;
//-------------Yaw΢��--------------------//
  Yaw_d=MPU6050_GYRO_LAST.Z-Yaw_old;
//-------------Roll  PID-------------------//
  Yaw_shell_out  = Yaw_shell_kp*(MPU6050_GYRO_LAST.Z-RC_Yaw) + Yaw_shell_ki*Yaw_i + Yaw_shell_kd*Yaw_d;
//------------Roll�Ƕȱ���------------------//
  Yaw_old=MPU6050_GYRO_LAST.Z;
        
        
////////////////////////�ڻ����ٶȻ�(PD)///////////////////////////////        
  pitch_core_kp_out = Pitch_core_kp * (Pitch_shell_out + MPU6050_GYRO_LAST.Y * 3.5);
  pitch_core_kd_out = Pitch_core_kd * (MPU6050_GYRO_LAST.Y   - Gyro_radian_old_y);

  Roll_core_kp_out  = Roll_core_kp  * (Roll_shell_out  + MPU6050_GYRO_LAST.X *3.5);
  Roll_core_kd_out  = Roll_core_kd  * (MPU6050_GYRO_LAST.X   - Gyro_radian_old_x);

  Yaw_core_kp_out  = Yaw_core_kp  * (Yaw_shell_out  + MPU6050_GYRO_LAST.Z * 1);
  Yaw_core_kd_out  = Yaw_core_kd  * (MPU6050_GYRO_LAST.Z   - Gyro_radian_old_z);
        
        
  Pitch_core_out = pitch_core_kp_out + pitch_core_kd_out;
  Roll_core_out  = Roll_core_kp_out  + Roll_core_kd_out;
  Yaw_core_out   = Yaw_core_kp_out   + Yaw_core_kd_out;

  Gyro_radian_old_y = MPU6050_GYRO_LAST.X;
  Gyro_radian_old_x = MPU6050_GYRO_LAST.Y; 
  Gyro_radian_old_z = MPU6050_GYRO_LAST.Z;   //������ʷֵ
        
//--------------------�����ֵ�ںϵ��ĸ����--------------------------------//

        
	if(Rc_D.THROTTLE>1020)
  {
		thr=Rc_D.THROTTLE- 1000;
		moto1=(int16_t)(thr - Roll_core_out - Pitch_core_out- Yaw_core_out);
		moto2=(int16_t)(thr + Roll_core_out - Pitch_core_out+ Yaw_core_out);        
		moto3=(int16_t)(thr + Roll_core_out + Pitch_core_out- Yaw_core_out);
		moto4=(int16_t)(thr - Roll_core_out + Pitch_core_out+ Yaw_core_out);
//                if(Rc_D.THROTTLE<=2000)
//                {
//  moto1=(int16_t)(thr  - Pitch_core_out);//- yaw);
//        moto2=(int16_t)(thr  - Pitch_core_out);//+ yaw);        
//        moto3=(int16_t)(thr  + Pitch_core_out);// - yaw);
//        moto4=(int16_t)(thr  + Pitch_core_out);//+ yaw);        
   
//  moto1=(int16_t)(thr  - Roll_core_out);//- yaw);
//        moto2=(int16_t)(thr  + Roll_core_out);//+ yaw);        
//        moto3=(int16_t)(thr  + Roll_core_out);// - yaw);
//        moto4=(int16_t)(thr  - Roll_core_out);//+ yaw);

//  moto1=(int16_t)(thr  - Yaw_core_out);//- yaw);
//        moto2=(int16_t)(thr  + Yaw_core_out);//+ yaw);        
//        moto3=(int16_t)(thr  - Yaw_core_out);// - yaw);
//        moto4=(int16_t)(thr  + Yaw_core_out);//+ yaw);                        
                        
//moto1=(int16_t)(thr - Roll_core_out - Pitch_core_out);
//moto2=(int16_t)(thr + Roll_core_out - Pitch_core_out);        
//moto3=(int16_t)(thr + Roll_core_out + Pitch_core_out);
//moto4=(int16_t)(thr - Roll_core_out + Pitch_core_out);        
//                        
//                }
  }
	else
	{
		moto1 = 0;
		moto2 = 0;
		moto3 = 0;
		moto4 = 0;
	}
	MOTO_PWMRFLASH(moto1,moto2,moto3,moto4);//        Moto_PwmRflash(moto1,moto2,moto3,moto4);
}
