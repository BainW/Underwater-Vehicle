#include "sys.h"
#include <math.h>
#include "filter.h"

float  KalmanGain;//  ����������
float  EstimateCovariance = 0.1f;//����Э����
float  MeasureCovariance = 0.2f;//����Э����
float  EstimateValue = 0.0f;//����ֵ






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

float  KalmanGain;//  ����������
float  EstimateCovariance = 0.1f;//����Э����
float  MeasureCovariance = 0.2f;//����Э����
float  EstimateValue = 0.0f;//����ֵ


*/

 
float KalmanFilter(float *Measure)
{
		//���㿨��������
		KalmanGain=EstimateCovariance*sqrt(1/(EstimateCovariance*EstimateCovariance+MeasureCovariance*MeasureCovariance));
		//���㱾���˲�����ֵ
		EstimateValue=EstimateValue+KalmanGain*(*Measure-EstimateValue);
		//���¹���Э����
		EstimateCovariance=sqrt(1-KalmanGain)*EstimateCovariance;
		//���²�������
		MeasureCovariance=sqrt(1-KalmanGain)*MeasureCovariance;
		//���ع���ֵ
		return EstimateValue;
}


float kalmanFilter(float *Original_Data) 
{
  static float prevData=0; 
  static float p=10, q=0.0001, r=0.005, kGain=0;
	p = p+q; 
	kGain = p/(p+r);

	*Original_Data = prevData+(kGain*(*Original_Data-prevData)); 
	p = (1-kGain)*p;

	prevData = *Original_Data;

	return *Original_Data; 
}
