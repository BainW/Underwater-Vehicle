#ifndef __ADC_CONVERT_H_
#define __ADC_CONVERT_H_

#include "DataType.h"



//��ʼ��ADC															   
int adc_init(void);

uint16 get_adc(uint8 ch);  //get adcͨ��ֵ

float get_voltage_value(void); //get ��ѹֵ



#endif


