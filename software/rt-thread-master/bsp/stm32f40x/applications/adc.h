#ifndef __ADC_CONVERT_H
#define __ADC_CONVERT_H

#include "init.h"


//��ʼ��ADC															   
void  adc_init(void);

u16 get_adc(u8 ch);  //get adcͨ��ֵ

double get_vol(void);//���ص�ѹֵ

void get_voltage(void);




#endif


