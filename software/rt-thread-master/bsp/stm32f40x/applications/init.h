#ifndef __INIT_H_
#define __INIT_H_


#include <rtdevice.h>

#include "sys.h" 
#include "led.h"
#include "key.h"
#include "buzzer.h"
#include "drv_oled.h"
#include "oled.h"
#include "gyroscope.h"
#include "adc.h"
#include "pwm.h"
#include "debug.h"


#include "w25qxx.h"



#include "ulog.h"

/*---------------------- Constant / Macro Definitions -----------------------*/		

/************��Ҫ����****************/

#define MODE  ROV_Mode

/************��Ҫ����***************/


#define LED_EVENT 			(1 << 0)  //LED�¼���־λ
#define KEY_EVENT 			(1 << 1)  //KEY�¼���־λ
#define BUZZ_EVENT 			(1 << 2)  //BUZZER�¼���־λ
#define OLED_EVENT 			(1 << 3)  //OLED�¼���־λ
#define GYRO_EVENT 			(1 << 4)  //Gyroscope�¼���־λ
#define ADC_EVENT 			(1 << 5)  //ADC�¼���־λ
#define PWM_EVENT 			(1 << 6)  //PWM�¼���־λ
#define W25Q128_EVENT   (1 << 7)  //W25Q128�¼���־λ








enum VehicleStatus{
	System_NULL = 1,
	System_Er,
	System_OK

};  //ö��ϵͳ״̬


enum VehicleMode{
	AUV_Mode,
	ROV_Mode
};  //ö��ϵͳģʽ


//void delay_us(unsigned int us);
//void delay_ms(unsigned int ms);

void delay_us(u32 nTimer);
void delay_ms(u32 nTimer);
void rt_hw_us_delay(rt_uint32_t us);

short bubble(short *adc_value);

int sprintf(char *buf, const char *format, ...);

#endif



