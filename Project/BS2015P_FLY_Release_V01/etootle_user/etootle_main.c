/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "etootle_main.h"
//
unsigned int system_idle_counter=0,system_timer_1ms_event=0,system_timer_state=0,system_status=0;
unsigned int system_timer_counter=0,system_led_timer_counter=0;
extern unsigned char bs004_com_command_ops;
extern unsigned char com_status_is_idle;		
extern unsigned char BS004_Ctrl_Gas,BS004_Ctrl_Valid;
extern signed char   BS004_Ctrl_Pitch,BS004_Ctrl_Roll,BS004_Ctrl_Yaw;
extern unsigned int Motor_BS004_M4,Motor_BS004_M2,Motor_BS004_M3,Motor_BS004_M1,BS004_Motor_Scale;
//
int main()
{
	BS004_RCC_Configuration();				//Բ�㲩ʿ:ʱ������
	BS004_NVIC_Configuration();     	//Բ�㲩ʿ:�ж�����
	//
	BS004_SYS_LED_Configuration();    //Բ�㲩ʿ:LED����
	BS004_SYS_Timer_Configuration();  //Բ�㲩ʿ:ϵͳʱ������
	BS004_LED_GPIO_Configuration();   //Բ�㲩ʿ:LED����
	//
	BS004_COM1_GPIO_Configuration();	//Բ�㲩ʿ:��������
	BS004_COM1_Port_Configuration();  //Բ�㲩ʿ:��������
	BS004_ADC_Configuration();        //Բ�㲩ʿ:��ѹ�������
	//
	BS004_MPU6050_Init();             //Բ�㲩ʿ:MPU6050��ʼ��
	BS004_Load_Fly_Parameter();			  //Բ�㲩ʿ:װ�ز���
	BS004_Show_Calibrated_Data();     //Բ�㲩ʿ:���У����MPU6050����
	//
	BS004_Motor_GPIO_Configuration(); //Բ�㲩ʿ:�������
	BS004_Motor_PWM_Configuration(); 	//Բ�㲩ʿ:�������
	//
	while(1)
	{
		if(system_timer_1ms_event)			//Բ�㲩ʿ:1MS�¼�����
		{
			system_timer_1ms_event=0;			//Բ�㲩ʿ:����¼�
			system_timer_counter++;       //Բ�㲩ʿ:ϵͳ������
			system_led_timer_counter++;   //Բ�㲩ʿ:LED����˸������
			//
			switch (system_timer_state)
			{
				case IDLE:									//Բ�㲩ʿ:����״̬
					system_idle_counter++;
					BS004_Get_MPU6050_Data();
					//
					if(system_idle_counter>5)
						system_timer_state=UPDATE_COMMAND;
					break;

				case UPDATE_COMMAND:				//Բ�㲩ʿ:��ȡMPU6050���ݺʹ�������
					BS004_Get_MPU6050_Data();
					system_status=BS004_COM1_Task_Process();
					//
					if(system_status)
						system_timer_state=UPDATE_MOTOR;
					else
						system_timer_state=RESET_MOTOR;
					break;
				
				case UPDATE_MOTOR:					//Բ�㲩ʿ:���µ�����
					BS004_Get_MPU6050_Data();
					BS004_Quad_Calculation();
				  BS004_PID_Control();
					BS004_Motor_Control();
					//
					system_timer_state=UPDATE_COMMAND;
					break;

				case RESET_MOTOR:          //Բ�㲩ʿ:��λ������
					BS004_Get_MPU6050_Data();
				  BS004_Quad_Calculation();
					BS004_Motor_Reset();
					//
					system_timer_state=UPDATE_COMMAND;
					break;
				
				default:
					break;
			}
		}
		else
		{			
			BS004_SYS_LED_TWINKLE();   //Բ�㲩ʿ:LED����˸
		}
	}	
}
//
void BS004_Quad_Calculation(void)
{
	float ax=0,ay=0,az=0,gx=0,gy=0,gz=0;
	//
	bs004_gyro_to_rad_scale=bs004_mpu6050_pi_scale*bs004_mpu6050_gyro_scale;
	//	
	gx=bs004_mpu6050_gyro_angel_pitch_ave/bs004_gyro_to_rad_scale;
	gy=bs004_mpu6050_gyro_angel_roll_ave/bs004_gyro_to_rad_scale;
	gz=bs004_mpu6050_gyro_angel_yaw_ave/bs004_gyro_to_rad_scale;		
	//
	ax=bs004_mpu6050_acc_angel_roll_ave;
	ay=bs004_mpu6050_acc_angel_pitch_ave;	
	az=bs004_mpu6050_acc_angel_yaw_ave;		
	//
	BS004_IMU_Update(ax,ay,az,gx,gy,gz);
}
//
unsigned char BS004_COM1_Task_Process(void)
{
	if(bs004_com_command_ops>0xA0)
	{
		com_status_is_idle=0;					//Բ�㲩ʿ:���ô���æ��־
		BS004_COM1_Communication();   //Բ�㲩ʿ:ִ������
		bs004_com_command_ops=0;			//Բ�㲩ʿ:������������־
		com_status_is_idle=1;					//Բ�㲩ʿ:�������æ��־
	} 
	else if((bs004_com_command_ops==0xA0))//  respond to gas
	{
		if(BS004_Ctrl_Valid==1)					//Բ�㲩ʿ:�����������Ƿ���Ч
		{
			com_status_is_idle=0;					//Բ�㲩ʿ:���ô���æ��־			
			//			
			if(BS004_Ctrl_Gas>BS004_Ctrl_Gas_Noise)
				bs004_fly_gas=BS004_Ctrl_Gas-BS004_Ctrl_Gas_Noise;
			else
				bs004_fly_gas=0;
			if(fabs(BS004_Ctrl_Pitch)>BS004_Ctrl_Dir_Noise)
				bs004_fly_pitch=BS004_Ctrl_Pitch;
			else
				bs004_fly_pitch=0;
			if(fabs(BS004_Ctrl_Roll)>BS004_Ctrl_Dir_Noise)
				bs004_fly_roll=BS004_Ctrl_Roll;
			else
				bs004_fly_roll=0;
			if(fabs(BS004_Ctrl_Yaw)>BS004_Ctrl_Dir_Noise)
				bs004_fly_yaw=BS004_Ctrl_Yaw;
			//else
			bs004_fly_yaw=0;//////////////////////////S*********************************************/
			//
			BS004_Ctrl_Valid=0;					//Բ�㲩ʿ:��������źű�־
			com_status_is_idle=1;				//Բ�㲩ʿ:�������æ��־
		}
	}	
	return bs004_fly_gas;
}
//
void BS004_Motor_Control(void)
{
	bs004_fly_m1_out=BS004_Motor_Speed_Scale(bs004_fly_m1);
	bs004_fly_m2_out=BS004_Motor_Speed_Scale(bs004_fly_m2);	
	bs004_fly_m3_out=BS004_Motor_Speed_Scale(bs004_fly_m3);
	bs004_fly_m4_out=BS004_Motor_Speed_Scale(bs004_fly_m4);	
	//
	TIM_SetCompare4(TIM8,bs004_fly_m1_out);		//Բ�㲩ʿ:����PWMռ�ձ�  
	TIM_SetCompare2(TIM8,bs004_fly_m2_out);	  //Բ�㲩ʿ:����PWMռ�ձ�  
	TIM_SetCompare1(TIM8,bs004_fly_m3_out);   //Բ�㲩ʿ:����PWMռ�ձ�      
	TIM_SetCompare3(TIM8,bs004_fly_m4_out);		//Բ�㲩ʿ:����PWMռ�ձ� 
}
//
void ANBT_SEND_DMP_EULER_DATA(void)
{
	float bs004_display_pitch=0,bs004_display_roll=0,bs004_display_yaw=0;
	unsigned char data_type,checksum=0,euler_data_sign=0,i=0;
	unsigned int bs004_mpu6050_euler_data[3];
	unsigned char bs004_mpu6050_euler_data_buffer[6];
	//
	bs004_display_pitch=bs004_imu_pitch*100;
	bs004_display_roll =bs004_imu_roll*100;
	bs004_display_yaw  =bs004_imu_yaw*100;
	//	
	if(bs004_display_pitch<0) 
	{
		euler_data_sign|=4;
		bs004_display_pitch+=18000;
	}
	if(bs004_display_roll<0) 
	{
		euler_data_sign|=2;	
		bs004_display_roll+=18000;
	}
	if(bs004_display_yaw<0) 
	{
		euler_data_sign|=1;		
		bs004_display_yaw+=18000;
	}
	//
	bs004_mpu6050_euler_data[0]=(unsigned int)bs004_display_pitch;
	bs004_mpu6050_euler_data[1]=(unsigned int)bs004_display_roll;	
	bs004_mpu6050_euler_data[2]=(unsigned int)bs004_display_yaw;	
	//
  for(i=0;i<3;i++) 
	{
		bs004_mpu6050_euler_data_buffer[i*2]=(bs004_mpu6050_euler_data[i]>>8)&0xff;
		bs004_mpu6050_euler_data_buffer[i*2+1]=bs004_mpu6050_euler_data[i]&0xff;
	}
	//
	data_type=0xB0| euler_data_sign;
	checksum=data_type;
	for(i=0;i<6;i++) checksum+=bs004_mpu6050_euler_data_buffer[i];
	checksum&=0xff;
	checksum=~checksum;
	checksum++;
	//
	if(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==SET)
	{
		BS004_COM1_Send_Char(':');
		BS004_COM1_Send_Num(data_type);
		for(i=0;i<6;i++) BS004_COM1_Send_Num(bs004_mpu6050_euler_data_buffer[i]);			
		BS004_COM1_Send_Num(checksum);
		BS004_COM1_Send_Char('/');
		BS004_COM1_Send_Char('\n');
	}
}



