/*************************************************************************************************************
圆点博士小四轴飞行器2015+版配套源代码声明:
该源代码仅供参考,圆点博士不对源代码提供任何形式的担保,也不对因使用该源代码而出现的损失负责.
用户可以以学习的目的修改和使用该源代码.
但用户在修改该源代码时,不得移除该部分版权信息，必须保留原版声明.

更多信息，请访问官方网站www.etootle.com, 官方博客:http://weibo.com/xiaosizhou
**************************************************************************************************************/
#include "stm32f10x_lib.h"
#include "etootle_motor.h"
#include "etootle_mpu6050.h"
#include "etootle_imu.h"
#include "etootle_pid.h"
//
#define BS004_MCU_LED				GPIO_Pin_8		//PA8
#define BS004_MCU_LED_PORT	GPIOA
//
#define BS004_Bootloader_USB_CON_PORT		GPIOB					
#define BS004_Bootloader_USB_CON   			GPIO_Pin_8			
//
#define BS004_SYS_TIMER_SYSCLK_DIV	0
#define BS004_SYS_TIMER_CLK_1MHZ  	71
//
void BS004_RCC_Configuration(void);
void BS004_NVIC_Configuration(void);
void BS004_Long_Delay(unsigned int nCount);
//
void BS004_SYS_LED_Configuration(void);
void BS004_MCU_LED_OFF(void);
void BS004_MCU_LED_ON(void);
void BS004_SYS_LED_TWINKLE(void);
void BS004_SYS_EULER_Update(void);
//
void BS004_SYS_Timer_Configuration(void);
void BS004_SYS_NVIC_Configuration(void);
void BS004_SYS_Timer_Interrupt(void);


