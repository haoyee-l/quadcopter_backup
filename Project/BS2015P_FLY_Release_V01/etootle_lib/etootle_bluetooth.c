/*************************************************************************************************************
Բ�㲩ʿС���������2015+������Դ��������:
��Դ��������ο�,Բ�㲩ʿ����Դ�����ṩ�κ���ʽ�ĵ���,Ҳ������ʹ�ø�Դ��������ֵ���ʧ����.
�û�������ѧϰ��Ŀ���޸ĺ�ʹ�ø�Դ����.
���û����޸ĸ�Դ����ʱ,�����Ƴ��ò��ְ�Ȩ��Ϣ�����뱣��ԭ������.

������Ϣ������ʹٷ���վwww.etootle.com, �ٷ�����:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "etootle_bluetooth.h" 
#include "etootle_parameter.h" 
#include "etootle_motor.h" 
#include "etootle_adc.h" 
#include "etootle_led.h" 
#include "etootle_sys.h" 
//
#define AnBT_Command_Head     58		//Բ�㲩ʿ����ͷ���� :
#define AnBT_Command_Tail	    47    //Բ�㲩ʿ����ͷ���� /
#define AnBT_COM_Buf_Length		64		//Բ�㲩ʿ���ջ��峤��
//
unsigned char  pid_data_buffer_valid=0;			//Բ�㲩ʿ:PID���ݺϷ���־
unsigned int   pid_data_buffer=0;         	//Բ�㲩ʿ:PID���ݻ���
unsigned int   pid_setting_P_value[3]={0};	//Բ�㲩ʿ:PID��P��������X��,Y��,Z��
unsigned int   pid_setting_I_value[3]={0};	//Բ�㲩ʿ:PID��I��������X��,Y��,Z��	
unsigned int   pid_setting_D_value[3]={0};	//Բ�㲩ʿ:PID��D��������X��,Y��,Z��	
unsigned int   pid_setting_M_value[3]={0};  //Բ�㲩ʿ:PID��M��������X��,Y��,Z��	
//
unsigned char  ctrl_data_buffer_valid=0;          //Բ�㲩ʿ:�������ݺϷ���־
unsigned char  remote_ctrl_data_buffer[4]={0};		//Բ�㲩ʿ:ң�����ݻ���
//
unsigned int   bs004_bat_value=0;									//Բ�㲩ʿ:��ص�ѹֵ
//
unsigned char com_status_is_idle=1;												//Բ�㲩ʿ:����ͨѶ���б�־
unsigned char valid_command_was_received=0;								//Բ�㲩ʿ:�Ϸ�������ձ�־
unsigned char bs004_com_command_ops=0;											//Բ�㲩ʿ:���յ�������
unsigned char com_receive_str_index=0;										//Բ�㲩ʿ:���ջ����ַ����
unsigned char com_receive_str_buf[AnBT_COM_Buf_Length]; 	//Բ�㲩ʿ:�����ַ�����
unsigned char com_receive_data_buf[AnBT_COM_Buf_Length]; 	//Բ�㲩ʿ:�������ݻ���
//
unsigned char BS004_Ctrl_Gas=0,BS004_Ctrl_Valid=0,BS004_Ctrl_Gas_Noise=50;
signed char   BS004_Ctrl_Pitch=0,BS004_Ctrl_Roll=0,BS004_Ctrl_Yaw=0,BS004_Ctrl_Dir_Noise=20;
signed short  bs004_fly_gas=0,bs004_fly_pitch=0,bs004_fly_roll=0,bs004_fly_yaw=0;
unsigned char BS004_IMU_Output=1,BS004_Motor_Lock=1;
//
void BS004_COM1_Interrupt(void)	//Բ�㲩ʿ:���������ж�
{
	unsigned char com_receive_data=0;
	unsigned char com_receive_data_checksum=0;
	unsigned char com_receive_data_checksum_low,com_receive_data_checksum_high;
	unsigned char com_data_checksum=0;
	unsigned char anbt_com_command_HH=0,anbt_com_command_HL=0,anbt_com_command_LH=0,anbt_com_command_LL=0;
	unsigned char i;
	//
	com_receive_data=USART_ReceiveData(USART1);   
	//	
	if(com_receive_data==AnBT_Command_Head)   
	{
			com_receive_str_index=0;     
			valid_command_was_received=0;			
			com_data_checksum=0;
	}
	else if(com_receive_data==AnBT_Command_Tail)  
	{
			if(com_receive_str_index>1) 
			{			
				for(i=0;i<com_receive_str_index-2;i++) com_data_checksum += com_receive_str_buf[i];    //Բ�㲩ʿ:��������У��ͱ�־
				com_receive_data_checksum_low=com_receive_str_buf[com_receive_str_index-1];
				com_receive_data_checksum_high=com_receive_str_buf[com_receive_str_index-2];				
				if(com_receive_data_checksum_low>58) com_receive_data_checksum_low-=55;
				else com_receive_data_checksum_low-=48;
				if(com_receive_data_checksum_high>58) com_receive_data_checksum_high-=55;
				else com_receive_data_checksum_high-=48;	
				com_receive_data_checksum=((com_receive_data_checksum_high<<4)&0xf0)|(com_receive_data_checksum_low&0x0f);
				com_data_checksum=com_data_checksum+com_receive_data_checksum;
				if(com_data_checksum==0) valid_command_was_received=1;
			}
			if(com_status_is_idle && valid_command_was_received) //data is correct and allowed to receive
			{				
				anbt_com_command_HH=com_receive_str_buf[0];
				anbt_com_command_HL=com_receive_str_buf[1];
				anbt_com_command_LH=com_receive_str_buf[2];
				anbt_com_command_LL=com_receive_str_buf[3];
				pid_data_buffer_valid=0;
				ctrl_data_buffer_valid=0;
				//
				if((anbt_com_command_HH=='R')&&(anbt_com_command_HL=='C'))	bs004_com_command_ops=0xA0;					//write remote ctrol
				else if((anbt_com_command_HH=='B')&&(anbt_com_command_HL=='A')&&(anbt_com_command_LH=='T'))  	bs004_com_command_ops=0xA1;		//read voltage
				else if((anbt_com_command_HH=='V')&&(anbt_com_command_HL=='E')&&(anbt_com_command_LH=='R'))		bs004_com_command_ops=0xA2;		//return version
				else if((anbt_com_command_HH=='C')&&(anbt_com_command_HL=='T')&&(anbt_com_command_LH=='R')&&(anbt_com_command_LL=='L'))	bs004_com_command_ops=0xA3;	  //read remote ctrol
				else if((anbt_com_command_HH=='P')&&(anbt_com_command_HL=='W')&&(anbt_com_command_LH=='O')&&(anbt_com_command_LL=='N'))	bs004_com_command_ops=0xA4;		//motor pwm on
				else if((anbt_com_command_HH=='P')&&(anbt_com_command_HL=='W')&&(anbt_com_command_LH=='O')&&(anbt_com_command_LL=='F'))	bs004_com_command_ops=0xA5;		//motor pwm off
				else if((anbt_com_command_HH=='P')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='S')&&(anbt_com_command_LL=='T'))	bs004_com_command_ops=0xA6;		//PID paramater mode	
				else if((anbt_com_command_HH=='Z')&&(anbt_com_command_HL=='T')&&(anbt_com_command_LH=='O')&&(anbt_com_command_LL=='N'))	bs004_com_command_ops=0xA7;		//PID paramater mode	
				else if((anbt_com_command_HH=='Z')&&(anbt_com_command_HL=='T')&&(anbt_com_command_LH=='O')&&(anbt_com_command_LL=='F'))	bs004_com_command_ops=0xA8;		//PID paramater mode	
				else if((anbt_com_command_HH=='R')&&(anbt_com_command_HL=='S')&&(anbt_com_command_LH=='T')) bs004_com_command_ops=0xA9;
				//	
				else if((anbt_com_command_HH=='P')&&(anbt_com_command_HL=='A')&&(anbt_com_command_LH=='L')&&(anbt_com_command_LL=='L'))	bs004_com_command_ops=0xB0;		//read P data
				else if((anbt_com_command_HH=='I')&&(anbt_com_command_HL=='A')&&(anbt_com_command_LH=='L')&&(anbt_com_command_LL=='L'))	bs004_com_command_ops=0xB1;		//read I data
				else if((anbt_com_command_HH=='D')&&(anbt_com_command_HL=='A')&&(anbt_com_command_LH=='L')&&(anbt_com_command_LL=='L'))	bs004_com_command_ops=0xB2;		//read D data
				else if((anbt_com_command_HH=='M')&&(anbt_com_command_HL=='A')&&(anbt_com_command_LH=='L')&&(anbt_com_command_LL=='L'))	bs004_com_command_ops=0xB3;		//read M data
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='P')&&(anbt_com_command_LL=='X'))	bs004_com_command_ops=0xB4;		//write PX
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='P')&&(anbt_com_command_LL=='Y'))	bs004_com_command_ops=0xB5;		//write PY
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='P')&&(anbt_com_command_LL=='Z'))	bs004_com_command_ops=0xB6;		//write PZ
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='I')&&(anbt_com_command_LL=='X'))	bs004_com_command_ops=0xB7;		//write IX
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='I')&&(anbt_com_command_LL=='Y'))	bs004_com_command_ops=0xB8;		//write IY
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='I')&&(anbt_com_command_LL=='Z'))	bs004_com_command_ops=0xB9;		//write IZ				
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='D')&&(anbt_com_command_LL=='X'))	bs004_com_command_ops=0xBA;		//write DX
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='D')&&(anbt_com_command_LL=='Y'))	bs004_com_command_ops=0xBB;		//write DY
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='D')&&(anbt_com_command_LL=='Z'))	bs004_com_command_ops=0xBC;		//write DZ
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='M')&&(anbt_com_command_LL=='X'))	bs004_com_command_ops=0xBD;		//write MX
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='M')&&(anbt_com_command_LL=='Y'))	bs004_com_command_ops=0xBE;		//write MY
				else if((anbt_com_command_HH=='W')&&(anbt_com_command_HL=='R')&&(anbt_com_command_LH=='M')&&(anbt_com_command_LL=='Z'))	bs004_com_command_ops=0xBF;		//write MZ
				else bs004_com_command_ops=0;
				//
				if(com_receive_str_index>4)	
				{
					if(com_receive_str_index==10)
					{
						for(i=0;i<4;i++) 
						{
							com_receive_data_buf[i]=com_receive_str_buf[i+4]-48;
							if(com_receive_data_buf[i]>9) com_receive_data_buf[i]=0;
						}
						//
						pid_data_buffer=com_receive_data_buf[0]*1000+com_receive_data_buf[1]*100+com_receive_data_buf[2]*10+com_receive_data_buf[3];	
						pid_data_buffer_valid=1;
					}
					else if(com_receive_str_index==12)
					{
						for(i=0;i<8;i++) 
						{			
							com_receive_data_buf[i]=com_receive_str_buf[i+2];
							if(com_receive_data_buf[i]>58) com_receive_data_buf[i]-=55;
							else com_receive_data_buf[i]-=48;
						}
						//
						for(i=0;i<4;i++) remote_ctrl_data_buffer[i]=com_receive_data_buf[i*2]*16+com_receive_data_buf[i*2+1];
						ctrl_data_buffer_valid=1;
					}
				}
				//		
				if(bs004_com_command_ops==0xA0)
				{
					BS004_Ctrl_Gas=remote_ctrl_data_buffer[0];
					BS004_Ctrl_Pitch=remote_ctrl_data_buffer[1]&0x7f;
					BS004_Ctrl_Roll=remote_ctrl_data_buffer[2]&0x7f;
					BS004_Ctrl_Yaw=remote_ctrl_data_buffer[3]&0x7f;
					//
					if(remote_ctrl_data_buffer[1]&0x80) BS004_Ctrl_Pitch=-BS004_Ctrl_Pitch;
					if(remote_ctrl_data_buffer[2]&0x80) BS004_Ctrl_Roll=-BS004_Ctrl_Roll;
					if((remote_ctrl_data_buffer[3]&0x80)==0x80) BS004_Ctrl_Yaw=-BS004_Ctrl_Yaw;					
					//
					BS004_Ctrl_Valid=1;
				}
		 }
	}
	else
	{
			com_receive_str_buf[com_receive_str_index] = com_receive_data;
			if(com_receive_str_index<AnBT_COM_Buf_Length-1) com_receive_str_index++;			//Բ�㲩ʿ:���ջ����ַ��1
			else com_receive_str_index=0;					//Բ�㲩ʿ:��0���ܻ����ַ,��ֹ�������
	}
}

//---------------------------------------------------------------------------------------------------

void BS004_COM1_Send_Char(unsigned char ascii_code) 						//Բ�㲩ʿ:����һ���ַ�
{
	BS004_Long_Delay(10000);
	USART_SendData(USART1,ascii_code);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}	//Բ�㲩ʿ:�ȴ�ֱ���������
}

void BS004_COM1_Send_Num(unsigned char number) 	//Բ�㲩ʿ:����һ������
{
	unsigned char num_low,num_high;
	num_low=number&0x0f;													//Բ�㲩ʿ:ȡ���ݵ�λ
	num_high=(number&0xf0)>>4;										//Բ�㲩ʿ:ȡ���ݸ�λ
	if(num_high<10)USART_SendData(USART1,num_high+48);
	else USART_SendData(USART1,num_high+55);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}								//Բ�㲩ʿ:�ȴ�ֱ���������
	if(num_low<10)USART_SendData(USART1,num_low+48);
	else USART_SendData(USART1,num_low+55);
  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}								//Բ�㲩ʿ:�ȴ�ֱ���������
}

void BS004_COM1_Send_Str_Head(void)
{
	BS004_COM1_Send_Char(':');  		//Բ�㲩ʿ:�����ַ�:
	BS004_COM1_Send_Char('D');  		//Բ�㲩ʿ:�����ַ�D
	BS004_COM1_Send_Char('-');  		//Բ�㲩ʿ:�����ַ�-
}

void BS004_COM1_Send_Str_Body(unsigned char* str_buf)		//Բ�㲩ʿ:����һ��ָ�����ȵ��ַ���
{
	unsigned char i,str_len;
	str_len=0;
	for(i=0;i<64;i++)
	{	
		str_len++;
		if(str_buf[i]=='.') break;
	}
  for(i=0;i<str_len;i++) BS004_COM1_Send_Char(str_buf[i]); 	//Բ�㲩ʿ:�����ַ�:
}

void BS004_COM1_Send_Str_Tail(void)
{
	BS004_COM1_Send_Char('/');																//Բ�㲩ʿ:�����ַ�/
	BS004_COM1_Send_Char(13);																	//Բ�㲩ʿ:���ͻس��ַ�
}

void BS004_COM1_Send_4bits_BCD_Num(int number) 	//Բ�㲩ʿ:����һ���ַ�
{
	unsigned int num;
	unsigned char num_th[4]={0};
	unsigned char i,bcd_code_len;
	//
	num=fabs(number);
	if(number<0) BS004_COM1_Send_Char('-');  		//Բ�㲩ʿ:�����ַ�-
	if(num>9999) num=9999;
	if(num>999) bcd_code_len=4;
	else if(num>99) bcd_code_len=3;
	else if(num>9) bcd_code_len=2;		
	else bcd_code_len=1;
	//
	if(num>999) 	{num_th[3]=num/1000;	num-=num_th[3]*1000;}
	if(num>99) 	 	{num_th[2]=num/100;		num-=num_th[2]*100; }
	if(num>9) 		{num_th[1]=num/10;		num-=num_th[1]*10;	}	
	num_th[0]=num;
	//
	for(i=0;i<bcd_code_len;i++) BS004_COM1_Send_Char(num_th[bcd_code_len-1-i]+48);
}

//---------------------------------------------------------------------------------------------------

void BS004_COM1_Communication(void)
{
	unsigned char i;
	//
	if(bs004_com_command_ops==0xA1) 			//Բ�㲩ʿ:���ص�ص�ѹ,ע��,��������Դû��,������0
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("read bat voltage .");
		BS004_ADC_Get_ADC_Value();
		BS004_COM1_Send_4bits_BCD_Num(bs004_bat_value);	
		BS004_COM1_Send_Str_Tail();
	}
	//
	else if(bs004_com_command_ops==0xA2) //Բ�㲩ʿ:���ذ汾��
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("version 03.");
		BS004_COM1_Send_Str_Tail();
	}
	//
	else if(bs004_com_command_ops==0xA3) //Բ�㲩ʿ:����ң��������
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("read remote control data .");
		//	
		BS004_COM1_Send_4bits_BCD_Num(BS004_Ctrl_Gas);			
		BS004_COM1_Send_Char(',');				
		BS004_COM1_Send_4bits_BCD_Num(BS004_Ctrl_Pitch);			
		BS004_COM1_Send_Char(',');			
		BS004_COM1_Send_4bits_BCD_Num(BS004_Ctrl_Roll);			
		BS004_COM1_Send_Char(',');	
		BS004_COM1_Send_4bits_BCD_Num(BS004_Ctrl_Yaw);		
		//		
		BS004_COM1_Send_Str_Tail();
	}
	else if(bs004_com_command_ops==0xA4) //Բ�㲩ʿ:�򿪵����Դ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("motor power on.");	
		BS004_COM1_Send_Str_Tail();		
		//
		BS004_Motor_Lock=0;
		BS004_Ctrl_Gas=0;					//Բ�㲩ʿ:��λң������
		BS004_Ctrl_Pitch=0;				//Բ�㲩ʿ:��λң������
		BS004_Ctrl_Roll=0;				//Բ�㲩ʿ:��λң������
		BS004_Ctrl_Yaw=0;					//Բ�㲩ʿ:��λң������
		BS004_MOTOR_LED_ON();			//Բ�㲩ʿ:����LED
	}
	//
	else if(bs004_com_command_ops==0xA5) //Բ�㲩ʿ:�رյ����Դ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("motor power off.");	
		BS004_COM1_Send_Str_Tail();		
		//
		BS004_Motor_Lock=1;
		BS004_Ctrl_Gas=0;					//Բ�㲩ʿ:��λң������
		BS004_Ctrl_Pitch=0;				//Բ�㲩ʿ:��λң������
		BS004_Ctrl_Roll=0;				//Բ�㲩ʿ:��λң������
		BS004_Ctrl_Yaw=0;					//Բ�㲩ʿ:��λң������
		BS004_MOTOR_LED_OFF();		//Բ�㲩ʿ:�ر�LED
	}
	//
	else if(bs004_com_command_ops==0xA6) //Բ�㲩ʿ:��λPID���ݺ�У������
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("reset PID cal data .");	
		BS004_COM1_Send_Str_Tail();		
		//
		bs004_mpu6050_acc_roll_cal=0;
		bs004_mpu6050_acc_pitch_cal=0;		
		bs004_mpu6050_acc_yaw_cal=0;
		bs004_mpu6050_gyro_pitch_cal=0;
		bs004_mpu6050_gyro_roll_cal=0;
		bs004_mpu6050_gyro_yaw_cal=0;
		//
		for(i=0;i<3;i++) pid_setting_P_value[i]=0;
		for(i=0;i<3;i++) pid_setting_I_value[i]=0;
		for(i=0;i<3;i++) pid_setting_D_value[i]=0;
		for(i=0;i<3;i++) pid_setting_M_value[i]=0;
	}	
	else if(bs004_com_command_ops==0xA7) //Բ�㲩ʿ:���������̬����λ��
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("begin to output angle .");	
		BS004_COM1_Send_Str_Tail();	
		//
		BS004_IMU_Output=1;
	}
	else if(bs004_com_command_ops==0xA8) //Բ�㲩ʿ:ֹͣ�����̬����λ��
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("stop to output angle .");	
		BS004_COM1_Send_Str_Tail();	
		//
		BS004_IMU_Output=0;
	}
	//
	else if(bs004_com_command_ops==0xA9) //Բ�㲩ʿ:��λ�ɻ�
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("reset the system .");	
		BS004_COM1_Send_Str_Tail();		
		//
		NVIC_SETFAULTMASK();
		NVIC_GenerateSystemReset();
	}	
	//
	else if(bs004_com_command_ops==0xB0) 	//Բ�㲩ʿ:����Pֵ
	{		
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("read All P data .");
		//
		for(i=0;i<3;i++)
		{
			BS004_COM1_Send_4bits_BCD_Num(pid_setting_P_value[i]);			
			BS004_COM1_Send_Char(',');		
		}
		//
		BS004_COM1_Send_Str_Tail();
	}
	//
	else if(bs004_com_command_ops==0xB1) 	//Բ�㲩ʿ:����Iֵ
	{		
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("read All I data .");
		//
		for(i=0;i<3;i++)
		{
			BS004_COM1_Send_4bits_BCD_Num(pid_setting_I_value[i]);			
			BS004_COM1_Send_Char(',');		
		}
		//
		BS004_COM1_Send_Str_Tail();
	}
	//
	else if(bs004_com_command_ops==0xB2) 	//Բ�㲩ʿ:����Dֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("read All D data .");
		//
		for(i=0;i<3;i++)
		{
			BS004_COM1_Send_4bits_BCD_Num(pid_setting_D_value[i]);			
			BS004_COM1_Send_Char(',');		
		}
		//
		BS004_COM1_Send_Str_Tail();
	}
	//
	else if(bs004_com_command_ops==0xB3) 	//Բ�㲩ʿ:����Mֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("read All M data .");
		//
		for(i=0;i<3;i++)
		{
			BS004_COM1_Send_4bits_BCD_Num(pid_setting_M_value[i]);			
			BS004_COM1_Send_Char(',');		
		}
		//
		BS004_COM1_Send_Str_Tail();
	}
	//
	else if(bs004_com_command_ops==0xB4)  //Բ�㲩ʿ:����X��Pֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write PX data .");
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_P_value[0]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//
	else if(bs004_com_command_ops==0xB5) //Բ�㲩ʿ:����Y��Pֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write PY data .");		
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_P_value[1]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//
	else if(bs004_com_command_ops==0xB6) //Բ�㲩ʿ:����Z��Pֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write PZ data .");				
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_P_value[2]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//
	else if(bs004_com_command_ops==0xB7)  //Բ�㲩ʿ:����X��Iֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write IX data .");
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_I_value[0]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//
	else if(bs004_com_command_ops==0xB8) //Բ�㲩ʿ:����Y��Iֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write IY data .");		
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_I_value[1]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//
	else if(bs004_com_command_ops==0xB9) //Բ�㲩ʿ:����Z��Iֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write IZ data .");				
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_I_value[2]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//	
	else if(bs004_com_command_ops==0xBA) //Բ�㲩ʿ:����X��Dֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write DX data .");	
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_D_value[0]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();			
	}
	//
	else if(bs004_com_command_ops==0xBB) //Բ�㲩ʿ:����Y��Dֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write DY data .");	
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_D_value[1]=pid_data_buffer;;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//
	else if(bs004_com_command_ops==0xBC) //Բ�㲩ʿ:����Z��Dֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write DZ data .");	
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_D_value[2]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//
	else if(bs004_com_command_ops==0xBD) //Բ�㲩ʿ:����X��Mֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write MX data .");	
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_M_value[0]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();			
	}
	//
	else if(bs004_com_command_ops==0xBE) //Բ�㲩ʿ:����Y��Mֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write MY data .");	
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_M_value[1]=pid_data_buffer;;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	//
	else if(bs004_com_command_ops==0xBF) //Բ�㲩ʿ:����Z��Mֵ
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("write MZ data .");	
		//
		if(pid_data_buffer_valid==1)
		{
			pid_setting_M_value[2]=pid_data_buffer;
			pid_data_buffer_valid=0;
		}
		//
		BS004_COM1_Send_4bits_BCD_Num(pid_data_buffer);	
		BS004_COM1_Send_Str_Tail();		
	}
	else
	{
		BS004_COM1_Send_Str_Head();
		BS004_COM1_Send_Str_Body("unknown command.");	//Բ�㲩ʿ:δ֪����
		BS004_COM1_Send_Str_Tail();		
	}
}

//---------------------------------------------------------------------------------------------------

void BS004_COM1_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
	//
  GPIO_InitStructure.GPIO_Pin = BS004_COM1_TX;					//Բ�㲩ʿ:����PA9�ܽ�Ϊ����TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 		//Բ�㲩ʿ:���ô���TX�����������ٶ�
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   		//Բ�㲩ʿ:���ô���TXΪ���
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	//
  GPIO_InitStructure.GPIO_Pin = BS004_COM1_RX;					//Բ�㲩ʿ:����PA9�ܽ�Ϊ����RX
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //Բ�㲩ʿ:���ô���RXΪ����
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
}

void BS004_COM1_Port_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
	//
  USART_InitStructure.USART_BaudRate = 115200;									//Բ�㲩ʿ:���ô��ڲ�����Ϊ115200
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //Բ�㲩ʿ:���ô������ݳ���Ϊ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //Բ�㲩ʿ:���ô���ֹͣλ����Ϊ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No ;					//Բ�㲩ʿ:���ô�����żУ��Ϊ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //Բ�㲩ʿ:���ô�������������Ϊ��
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;										//Բ�㲩ʿ:���ô���Ϊ���ͺͽ���ģʽ
  USART_Init(USART1, &USART_InitStructure);			//Բ�㲩ʿ:���ô��ڲ���
	//
	BS004_COM1_NVIC_Configuration();							//Բ�㲩ʿ:�����ж����ȼ�
  USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);	//Բ�㲩ʿ:��������ж�
  USART_Cmd(USART1, ENABLE);  									//Բ�㲩ʿ:ʹ�ܴ���
	//
	BS004_COM1_Send_Str_Head();
	BS004_COM1_Send_Str_Body("finish to init bluetooth device.");	//Բ�㲩ʿ:δ֪����
	BS004_COM1_Send_Str_Tail();		
}

void BS004_COM1_NVIC_Configuration(void)				//Բ�㲩ʿ:���ô����ж����ȼ�
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}





