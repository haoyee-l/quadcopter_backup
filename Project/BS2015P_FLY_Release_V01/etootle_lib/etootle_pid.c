/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "etootle_pid.h" 
//
extern unsigned int pid_setting_P_value[3];		//Բ�㲩ʿ:PID��P��������X��,Y��,Z��
extern unsigned int pid_setting_I_value[3];		//Բ�㲩ʿ:PID��I��������X��,Y��,Z��
extern unsigned int pid_setting_D_value[3];		//Բ�㲩ʿ:PID��D��������X��,Y��,Z��
extern unsigned int pid_setting_M_value[3];		//Բ�㲩ʿ:PID��M��������X��,Y��,Z��
float bs004_pitch_p,bs004_roll_p,bs004_yaw_p;
float bs004_pitch_i,bs004_roll_i,bs004_yaw_i;
float bs004_pitch_d,bs004_roll_d,bs004_yaw_d;
int bs004_pitch_m,bs004_roll_m,bs004_yaw_m;
float bs004_pitch_mf,bs004_roll_mf,bs004_yaw_mf;
//
extern float bs004_mpu6050_gyro_angel_pitch_ave,bs004_mpu6050_gyro_angel_roll_ave,bs004_mpu6050_gyro_angel_yaw_ave;
extern float bs004_mpu6050_acc_angel_pitch_ave,bs004_mpu6050_acc_angel_roll_ave,bs004_mpu6050_acc_angel_yaw_ave;
//
extern signed short bs004_fly_gas,bs004_fly_pitch,bs004_fly_roll,bs004_fly_yaw;
float bs004_fly_gas_scale=0,bs004_fly_pitch_scale=0,bs004_fly_roll_scale=0,bs004_fly_yaw_scale=0;
//
extern float bs004_imu_pitch,bs004_imu_roll,bs004_imu_yaw;
float bs004_angle_cur_pitch=0,bs004_angle_cur_roll=0,bs004_angle_cur_yaw=0;
float bs004_angle_last_pitch=0,bs004_angle_last_roll=0,bs004_angle_last_yaw=0;
float bs004_angle_dif_pitch=0,bs004_angle_dif_roll=0,bs004_angle_dif_yaw=0;
float bs004_angle_int_pitch=0,bs004_angle_int_roll=0,bs004_angle_int_yaw=0;
//
float bs004_fly_m1=0,bs004_fly_m2=0,bs004_fly_m3=0,bs004_fly_m4=0;
float bs004_fly_pitch_dir,bs004_fly_roll_dir,bs004_fly_yaw_dir;
//
void BS004_PID_Control(void)
{
	bs004_roll_p =pid_setting_P_value[0]/10.0f;
	bs004_pitch_p=pid_setting_P_value[1]/10.0f;
	bs004_yaw_p  =pid_setting_P_value[2]/10.0f;
	bs004_roll_i =pid_setting_I_value[0]/10000.0f;
	bs004_pitch_i=pid_setting_I_value[1]/10000.0f;
	bs004_yaw_i  =pid_setting_I_value[2]/10000.0f;
	bs004_roll_d =pid_setting_D_value[0]/1000.0f;
	bs004_pitch_d=pid_setting_D_value[1]/1000.0f;
	bs004_yaw_d  =pid_setting_D_value[2]/1000.0f;
	//
	bs004_roll_m=pid_setting_M_value[0];
	bs004_pitch_m=pid_setting_M_value[1];
	bs004_yaw_m=pid_setting_M_value[2];
	//
	if(bs004_roll_m >1000) bs004_roll_m =-(bs004_roll_m-1000);
	if(bs004_pitch_m>1000) bs004_pitch_m=-(bs004_pitch_m-1000);
	if(bs004_yaw_m  >1000) bs004_yaw_m  =-(bs004_yaw_m-1000);
	//
	bs004_roll_mf=bs004_roll_m/10.0f;
	bs004_pitch_mf=bs004_pitch_m/10.0f;
	bs004_yaw_mf=bs004_yaw_m/10.0f;
	//
	bs004_fly_pitch_dir=(float)bs004_fly_pitch/10.0f;
	bs004_fly_roll_dir=(float)bs004_fly_roll/10.0f;
	bs004_fly_yaw_dir=(float)bs004_fly_yaw/10.0f;
	//
	//Բ�㲩ʿ:�ں�ң���������ź�
	bs004_fly_m1=bs004_fly_gas*bs004_fly_gas_scale - bs004_fly_pitch*bs004_fly_pitch_scale + bs004_fly_yaw*bs004_fly_yaw_scale;
	bs004_fly_m3=bs004_fly_gas*bs004_fly_gas_scale + bs004_fly_pitch*bs004_fly_pitch_scale + bs004_fly_yaw*bs004_fly_yaw_scale;
	bs004_fly_m2=bs004_fly_gas*bs004_fly_gas_scale - bs004_fly_roll*bs004_fly_roll_scale   - bs004_fly_yaw*bs004_fly_yaw_scale;
	bs004_fly_m4=bs004_fly_gas*bs004_fly_gas_scale + bs004_fly_roll*bs004_fly_roll_scale   - bs004_fly_yaw*bs004_fly_yaw_scale;
	
	bs004_angle_cur_pitch=bs004_imu_pitch-bs004_pitch_mf-bs004_fly_pitch_dir;
	bs004_angle_cur_roll =bs004_imu_roll-bs004_roll_mf-bs004_fly_roll_dir;
	bs004_angle_cur_yaw  =bs004_imu_yaw-bs004_yaw_mf-bs004_fly_yaw_dir;
	//
	//Բ�㲩ʿ:�ں�P�Ƕȱ�������
	bs004_fly_m1=bs004_fly_m1 + bs004_pitch_p*bs004_angle_cur_pitch - bs004_roll_p *bs004_angle_cur_roll + bs004_yaw_p*bs004_angle_cur_yaw;
	bs004_fly_m2=bs004_fly_m2 - bs004_pitch_p*bs004_angle_cur_pitch - bs004_roll_p *bs004_angle_cur_roll - bs004_yaw_p*bs004_angle_cur_yaw;
	bs004_fly_m3=bs004_fly_m3 - bs004_pitch_p*bs004_angle_cur_pitch + bs004_roll_p *bs004_angle_cur_roll + bs004_yaw_p*bs004_angle_cur_yaw;
	bs004_fly_m4=bs004_fly_m4 + bs004_pitch_p*bs004_angle_cur_pitch + bs004_roll_p *bs004_angle_cur_roll - bs004_yaw_p*bs004_angle_cur_yaw;
	//
	bs004_angle_int_pitch=bs004_angle_int_pitch+bs004_angle_cur_pitch;
	bs004_angle_int_roll =bs004_angle_int_roll +bs004_angle_cur_roll;	
	bs004_angle_int_roll =bs004_angle_int_yaw +bs004_angle_cur_yaw;	/////////////////////////////////S***********************************/
	//
  //Բ�㲩ʿ:�ں�I
	bs004_fly_m1=bs004_fly_m1 + bs004_pitch_i*bs004_angle_int_pitch - bs004_roll_i *bs004_angle_int_roll + bs004_yaw_i *bs004_angle_int_yaw;
	bs004_fly_m2=bs004_fly_m2 - bs004_pitch_i*bs004_angle_int_pitch - bs004_roll_i *bs004_angle_int_roll - bs004_yaw_i *bs004_angle_int_yaw;
	bs004_fly_m3=bs004_fly_m3 - bs004_pitch_i*bs004_angle_int_pitch + bs004_roll_i *bs004_angle_int_roll + bs004_yaw_i *bs004_angle_int_yaw;
	bs004_fly_m4=bs004_fly_m4 + bs004_pitch_i*bs004_angle_int_pitch + bs004_roll_i *bs004_angle_int_roll - bs004_yaw_i *bs004_angle_int_yaw;
	//
	bs004_angle_dif_pitch=bs004_angle_cur_pitch-bs004_angle_last_pitch;
	bs004_angle_dif_roll =bs004_angle_cur_roll-bs004_angle_last_roll;
	bs004_angle_dif_yaw  =bs004_angle_last_yaw-bs004_angle_cur_yaw;
	//
	//Բ�㲩ʿ:�ں�D
	bs004_fly_m1=bs004_fly_m1 + bs004_pitch_d*bs004_mpu6050_gyro_angel_pitch_ave - bs004_roll_d *bs004_mpu6050_gyro_angel_roll_ave + bs004_yaw_d*bs004_mpu6050_gyro_angel_yaw_ave;
	bs004_fly_m2=bs004_fly_m2 - bs004_pitch_d*bs004_mpu6050_gyro_angel_pitch_ave - bs004_roll_d *bs004_mpu6050_gyro_angel_roll_ave - bs004_yaw_d*bs004_mpu6050_gyro_angel_yaw_ave;
	bs004_fly_m3=bs004_fly_m3 - bs004_pitch_d*bs004_mpu6050_gyro_angel_pitch_ave + bs004_roll_d *bs004_mpu6050_gyro_angel_roll_ave + bs004_yaw_d*bs004_mpu6050_gyro_angel_yaw_ave;
	bs004_fly_m4=bs004_fly_m4 + bs004_pitch_d*bs004_mpu6050_gyro_angel_pitch_ave + bs004_roll_d *bs004_mpu6050_gyro_angel_roll_ave - bs004_yaw_d*bs004_mpu6050_gyro_angel_yaw_ave;
	//
	bs004_angle_last_pitch=bs004_angle_cur_pitch;
	bs004_angle_last_roll =bs004_angle_cur_roll;	
	bs004_angle_last_yaw =bs004_angle_cur_yaw;	
}

