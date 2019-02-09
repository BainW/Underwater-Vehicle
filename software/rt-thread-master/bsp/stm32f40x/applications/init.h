#ifndef __INIT_H
#define __INIT_H

#include <rtdevice.h>

#include "sys.h" 
#include "led.h"
#include "key.h"
#include "buzzer.h"
#include "drv_oled.h"
#include "oled.h"

#include "gyroscope.h"
 	 
enum VehicleStatus{
	System_NULL = 1,
	System_Er,
	System_OK

};  //ö��ϵͳ״̬



enum VehicleMode{
	ROV_Mode = 1,
	AUV_Mode,
};  //ö��ϵͳģʽ

//void delay_us(unsigned int us);
//void delay_ms(unsigned int ms);

void delay_us(u32 nTimer);
void delay_ms(u32 nTimer);

#endif



