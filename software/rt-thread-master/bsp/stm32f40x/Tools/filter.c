#include "sys.h"
#include <math.h>
#include "filter.h"





/* ð��  */
short bubble(short *adc_value)
{
		u8 i,j;
		short res = 0;   //reserve
		short value = 0;

		for(j = 0;j < 10-1;j++){
				for(i = 0;i < 9-j;i++){
						if( adc_value[i] > adc_value[i+1] ){ //>����   <����
								res = adc_value[i];
								adc_value[i] = adc_value[i+1];
								adc_value[i+1] = res;
						}
					
				}
		}
		value = (*(adc_value+4)+ *(adc_value+5)+ *(adc_value+6))/3;		
		return (short)value;
} 

/*
����p�ĳ�ֵ�������ȡ�����ǲ���Ϊ0��Ϊ0�Ļ��������˲�������Ϊ�Ѿ��������˲����ˣ� 

q,r��ֵ��Ҫ�����Գ����������˾���(������¶ȼ��ж��ƣ��Լ���ĳ������ж�ǿ)

q���������˲����������ʵ�����ߵ�����̶ȣ�qԽ��Խ�ӽ���

r�������˲��������ƽ���̶ȣ�rԽ��Խƽ���� 

*/
float KalmanFilter(float *Original_Data) 
{
  static float prevData=0; 
  static float p=10, q=0.0001, r=0.05, kGain=0;
	
	p = p+q; 
	kGain = p/(p+r);

	*Original_Data = prevData+(kGain*(*Original_Data-prevData)); 
	p = (1-kGain)*p;

	prevData = *Original_Data;

	return *Original_Data; 
}
