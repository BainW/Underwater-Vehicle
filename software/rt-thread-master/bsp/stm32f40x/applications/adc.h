#ifndef __ADC_CONVERT_H_
#define __ADC_CONVERT_H_

#include "init.h"

extern float  volatge;


//��ʼ��ADC															   
void  adc_init(void);

u16 get_adc(u8 ch);  //get adcͨ��ֵ

float get_vol(void); //get ��ѹֵ

void get_voltage(void);




#endif


