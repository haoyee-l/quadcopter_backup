/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "etootle_parameter.h" 
//
void BS004_Load_Fly_Parameter(void)
{
	BS004_COM1_Send_Str_Head(); 
	BS004_COM1_Send_Str_Body("load default parameter .");   //Բ�㲩ʿ:ʹ�ô���Ĭ�ϲ���
	BS004_COM1_Send_Str_Tail();
	//
	BS004_Load_Filter_Parameter();			//Բ�㲩ʿ:װ�ز���
	BS004_Load_Pid_Parameter();         //Բ�㲩ʿ:װ�ز���
	BS004_Load_Calibration_Parameter(); //Բ�㲩ʿ:װ�ز���
}

void BS004_Load_Filter_Parameter(void)
{
	int bs004_filter_par[12];
	//	
	bs004_filter_par[0]=950;
	bs004_filter_par[1]=50;
	bs004_filter_par[2]=1;
	bs004_filter_par[3]=1000;
	bs004_filter_par[4]=1640;
	bs004_filter_par[5]=5730;
	bs004_filter_par[6]=1000;
	bs004_filter_par[7]=1000;
	bs004_filter_par[8]=36;
	bs004_filter_par[9]=1;
	bs004_filter_par[10]=1600;
	bs004_filter_par[11]=1;		
	//
	bs004_filter_high=(float)bs004_filter_par[0]/1000.0f;						//Բ�㲩ʿ:�˲�����
	bs004_filter_low=(float)bs004_filter_par[1]/1000.0f;						//Բ�㲩ʿ:�˲�����	
	bs004_filter_time=(float)bs004_filter_par[2]/1000.0f;						//Բ�㲩ʿ:�˲�����		
	bs004_sys_timer_period=(unsigned int)bs004_filter_par[3]-1; 		//Բ�㲩ʿ:����������Ƶ��
	bs004_mpu6050_gyro_scale=(float)bs004_filter_par[4]/100.0f;			//Բ�㲩ʿ:������������
	bs004_mpu6050_pi_scale=(float)bs004_filter_par[5]/100.0f;			  //Բ�㲩ʿ:����ϵ��
	bs004_hmc5883l_mag_scale=(float)bs004_filter_par[6]/1000.0f;		//Բ�㲩ʿ:������������
	bs004_motor_pwm_period=(unsigned int)bs004_filter_par[7];	  		//Բ�㲩ʿ:�����������
	BS004_Motor_Scale=(unsigned int)bs004_filter_par[8];          	//Բ�㲩ʿ:�������������
	bs004_quad_halfT=(float)bs004_filter_par[9]/1000.0f;						//Բ�㲩ʿ:��Ԫ��ʱ��ϵ��
	bs004_quad_Kp=(float)bs004_filter_par[10]/1000.0f;							//Բ�㲩ʿ:��Ԫ������ϵ��
	bs004_quad_Ki=(float)bs004_filter_par[11]/1000.0f;							//Բ�㲩ʿ:��Ԫ������ϵ��	
}

void BS004_Load_Pid_Parameter(void)
{
	int bs004_pid_par[12];
	unsigned char i;
	//	
	bs004_pid_par[0]=150;
	bs004_pid_par[1]=150;
	bs004_pid_par[2]=150;
	bs004_pid_par[3]=0;
	bs004_pid_par[4]=0;
	bs004_pid_par[5]=0;
	bs004_pid_par[6]=250;
	bs004_pid_par[7]=250;
	bs004_pid_par[8]=500;
	bs004_pid_par[9]=6;
	bs004_pid_par[10]=15;
	bs004_pid_par[11]=20;		
	//
	for(i=0;i<3;i++) pid_setting_P_value[i]=(unsigned int)bs004_pid_par[i];		//Բ�㲩ʿ:P����
	for(i=0;i<3;i++) pid_setting_I_value[i]=(unsigned int)bs004_pid_par[i+3]; //Բ�㲩ʿ:I����
	for(i=0;i<3;i++) pid_setting_D_value[i]=(unsigned int)bs004_pid_par[i+6]; //Բ�㲩ʿ:D����
	for(i=0;i<3;i++) pid_setting_M_value[i]=0;                                //Բ�㲩ʿ:��е���ĵ��������
	bs004_fly_gas_scale=(float)bs004_pid_par[9]/1.0f;
	bs004_fly_pitch_scale=0.0f;
	bs004_fly_roll_scale=0.0f;
	bs004_fly_yaw_scale=0.0f;
}

void BS004_Load_Calibration_Parameter(void)
{
	int bs004_cal_par[12];
	//	
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("start to do self calibration.");      //Բ�㲩ʿ:���ҽ������������е���λ�ã�������Ӧ�ñ���ˮƽ��
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
	//
	BS004_Cal_MPU6050_Data(bs004_cal_par);
	//
	bs004_mpu6050_gyro_pitch_cal=(signed short int)bs004_cal_par[0];	//Բ�㲩ʿ:������У�����
	bs004_mpu6050_gyro_roll_cal=(signed short int)bs004_cal_par[1];   //Բ�㲩ʿ:������У�����
	bs004_mpu6050_gyro_yaw_cal=(signed short int)bs004_cal_par[2];    //Բ�㲩ʿ:������У�����
	bs004_mpu6050_acc_roll_cal=(signed short int)bs004_cal_par[3];		//Բ�㲩ʿ:���ٶ�У�����
	bs004_mpu6050_acc_pitch_cal=(signed short int)bs004_cal_par[4];   //Բ�㲩ʿ:���ٶ�У�����
	bs004_mpu6050_acc_yaw_cal=(signed short int)bs004_cal_par[5];     //Բ�㲩ʿ:���ٶ�У�����
	bs004_hmc5883l_mag_pitch_cal=(signed short int)bs004_cal_par[6];  //Բ�㲩ʿ:������У�����
	bs004_hmc5883l_mag_roll_cal=(signed short int)bs004_cal_par[7];   //Բ�㲩ʿ:������У�����
	bs004_hmc5883l_mag_yaw_cal=(signed short int)bs004_cal_par[8];    //Բ�㲩ʿ:������У�����
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("gyro pitch cal= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_gyro_pitch_cal);
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("gyro roll cal= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_gyro_roll_cal);
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("gyro yaw cal= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_gyro_yaw_cal);
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("acc roll cal= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_acc_roll_cal);
	BS004_COM1_Send_Str_Tail();	
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("acc pitch cal= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_acc_pitch_cal);
	BS004_COM1_Send_Str_Tail();	
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("acc yaw cal= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_acc_yaw_cal);
	BS004_COM1_Send_Str_Tail();	
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("finish the self calibration.");      //Բ�㲩ʿ:�������ҽ���
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
}

void BS004_Cal_MPU6050_Data(int *bs004_cal_data)   
{
	unsigned char i,j;
	unsigned char bs004_mpu6050_cal_data_buffer[14];
	signed short int bs004_mpu6050_cal_data[100][7];
	int bs004_mpu6050_cal_sum[7];
	//
	for(i=0;i<7;i++) bs004_mpu6050_cal_sum[i]=0;
	//
	for(j=0;j<100;j++)
	{
		BS004_I2C_START();
		BS004_I2C_SendByte(BS004_MPU6050_GYRO_ADDR);							//Բ�㲩ʿ:����������д��ַ
		BS004_I2C_SendByte(BS004_MPU6050_ACCEL_DATA_ADDR);    		//Բ�㲩ʿ:���������ǼĴ�����ַ
		BS004_I2C_START();
		BS004_I2C_SendByte(BS004_MPU6050_GYRO_ADDR+1);      			//Բ�㲩ʿ:���������Ƕ���ַ
		BS004_I2C_Receive14Bytes(bs004_mpu6050_cal_data_buffer);	//Բ�㲩ʿ:���������ǼĴ���ֵ
		BS004_I2C_STOP();
		//				
		for(i=0;i<7;i++) bs004_mpu6050_cal_data[j][i]=(((signed short int)bs004_mpu6050_cal_data_buffer[i*2]) << 8) | bs004_mpu6050_cal_data_buffer[i*2+1];
	}
	//
	for(i=0;i<7;i++)
	{
		for(j=0;j<100;j++) bs004_mpu6050_cal_sum[i]=bs004_mpu6050_cal_sum[i]+(int)bs004_mpu6050_cal_data[j][i];
	}
	//
	bs004_cal_data[0]=bs004_mpu6050_cal_sum[4]/100;
	bs004_cal_data[1]=bs004_mpu6050_cal_sum[5]/100;
	bs004_cal_data[2]=bs004_mpu6050_cal_sum[6]/100;
	bs004_cal_data[3]=bs004_mpu6050_cal_sum[0]/100;
	bs004_cal_data[4]=bs004_mpu6050_cal_sum[1]/100;
	bs004_cal_data[5]=bs004_mpu6050_cal_sum[2]/100;;
	bs004_cal_data[6]=0;
	bs004_cal_data[7]=0;
	bs004_cal_data[8]=0;
}

void BS004_Show_Calibrated_Data(void)
{
	unsigned char i;
	unsigned char bs004_mpu6050_caled_data_buffer[14];
	signed short int bs004_mpu6050_caled_data[7];
	int bs004_mpu6050_acc_roll_caled,bs004_mpu6050_acc_pitch_caled,bs004_mpu6050_acc_yaw_caled;
	int bs004_mpu6050_gyro_pitch_caled,bs004_mpu6050_gyro_roll_caled,bs004_mpu6050_gyro_yaw_caled;
	//
	BS004_I2C_START();
	BS004_I2C_SendByte(BS004_MPU6050_GYRO_ADDR);								//Բ�㲩ʿ:����������д��ַ
	BS004_I2C_SendByte(BS004_MPU6050_ACCEL_DATA_ADDR);    			//Բ�㲩ʿ:���������ǼĴ�����ַ
	BS004_I2C_START();
	BS004_I2C_SendByte(BS004_MPU6050_GYRO_ADDR+1);      				//Բ�㲩ʿ:���������Ƕ���ַ
	BS004_I2C_Receive14Bytes(bs004_mpu6050_caled_data_buffer);	//Բ�㲩ʿ:���������ǼĴ���ֵ
	BS004_I2C_STOP();
	//				
	for(i=0;i<7;i++) bs004_mpu6050_caled_data[i]=(((signed short int)bs004_mpu6050_caled_data_buffer[i*2]) << 8) | bs004_mpu6050_caled_data_buffer[i*2+1];
	bs004_mpu6050_acc_roll_caled=bs004_mpu6050_caled_data[0]-bs004_mpu6050_acc_roll_cal;
	bs004_mpu6050_acc_pitch_caled=bs004_mpu6050_caled_data[1]-bs004_mpu6050_acc_pitch_cal;
	bs004_mpu6050_acc_yaw_caled=bs004_mpu6050_caled_data[2];
	bs004_mpu6050_gyro_pitch_caled=bs004_mpu6050_caled_data[4]-bs004_mpu6050_gyro_pitch_cal;
	bs004_mpu6050_gyro_roll_caled=bs004_mpu6050_caled_data[5]-bs004_mpu6050_gyro_roll_cal;;
	bs004_mpu6050_gyro_yaw_caled=bs004_mpu6050_caled_data[6]-bs004_mpu6050_gyro_yaw_cal;
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("begin to show mpu6050 data.");      
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
	//
		BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("gyro pitch= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_gyro_pitch_caled);
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("gyro roll= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_gyro_roll_caled);
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("gyro yaw= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_gyro_yaw_caled);
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("acc roll= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_acc_roll_caled);
	BS004_COM1_Send_Str_Tail();	
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("acc pitch= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_acc_pitch_caled);
	BS004_COM1_Send_Str_Tail();	
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("acc yaw= .");
	BS004_COM1_Send_4bits_BCD_Num(bs004_mpu6050_acc_yaw_caled);
	BS004_COM1_Send_Str_Tail();	
	BS004_Long_Delay(300000);	
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("finish to show mpu6050 data.");     
	BS004_COM1_Send_Str_Tail();
	BS004_Long_Delay(300000);	
}

