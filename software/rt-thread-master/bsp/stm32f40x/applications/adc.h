#ifndef __ADC_CONVERT_H_
#define __ADC_CONVERT_H_

#include "DataType.h"



//��ʼ��ADC															   
void  adc_init(void);

uint16 get_adc(uint8 ch);  //get adcͨ��ֵ

float get_vol(void); //get ��ѹֵ

void get_voltage(void);


extern float  volatge;

#endif


