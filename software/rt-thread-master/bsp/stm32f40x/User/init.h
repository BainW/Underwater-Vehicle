#ifndef __INIT_H_
#define __INIT_H_



#include "sys.h" 
#include "led.h"
#include "key.h"
#include "buzzer.h"
#include "drv_oled.h"
#include "oled.h"
#include "gyroscope.h"
#include "adc.h"
#include "pwm.h"



/*---------------------- Constant / Macro Definitions -----------------------*/		

/************��Ҫ����****************/


typedef enum
{
	AUV_Mode,
	ROV_Mode
	
}VehicleMode_E;


/************��Ҫ����***************/


#define LED_EVENT 			(1 << 0)  //LED�¼���־λ
#define KEY_EVENT 			(1 << 1)  //KEY�¼���־λ
#define BUZZ_EVENT 			(1 << 2)  //BUZZER�¼���־λ
#define OLED_EVENT 			(1 << 3)  //OLED�¼���־λ
#define GYRO_EVENT 			(1 << 4)  //Gyroscope�¼���־λ
#define ADC_EVENT 			(1 << 5)  //ADC�¼���־λ
#define PWM_EVENT 			(1 << 6)  //PWM�¼���־λ
#define CAM_EVENT 			(1 << 7)  //Camera�¼���־λ


#define PI 3.1415926f //float��
#define Rad2Deg(Rad) (Rad * 180.0f / PI)
#define Deg2Rad(Deg) (Deg * PI / 180.0f)


enum VehicleStatus{
	System_NORMAL_STATUS = 1,//����ģʽ
  System_DEBUG_STATUS = 2, //����ģʽ
	System_ERROR_STATUS,
};  //ö��ϵͳ״̬




//void delay_us(unsigned int us);
//void delay_ms(unsigned int ms);

//void delay_us(u32 nTimer);
//void delay_ms(u32 nTimer);
void rt_hw_us_delay(u32 us);
void rt_hw_ms_delay(u32 ms);
void delay_us(u32 nTimer);

int sprintf(char *buf, const char *format, ...);

#endif



