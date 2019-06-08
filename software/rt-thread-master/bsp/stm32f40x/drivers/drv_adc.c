/*
 * adc.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ADC��ѹ�ɼ�   
 */
#define LOG_TAG    "adc"

#include "drv_adc.h"
#include <rtthread.h>
#include <elog.h>
#include <stdio.h>
#include "sys.h"
#include "filter.h"
/*---------------------- Constant / Macro Definitions -----------------------*/		

#define REFER_VOLTAGE       3.3         // �ο���ѹ 3.3V,���ݾ��ȳ���100����2λС��
#define CONVERT_BITS        (1 << 12)   // ת��λ��Ϊ12λ 
#define Voltge_Parameter    11				  // ��ѹϵ�� 1/(200K/(200K+2M))   23.4 ��ѹ��ѹϵ�� 1.67

/*----------------------- Variable Declarations -----------------------------*/

extern struct rt_event init_event;/* ALL_init �¼����ƿ� */

/*----------------------- Function Implement --------------------------------*/



/* ð�� get��ѹ */
float get_voltage_value(void)
{
		uint8 i;
		float voltage = 0.0f;
		short adc_value[10] = {0};
		
		for(i = 0;i < 20;i+=2){
				adc_value[i/2] = get_adc(ADC_Channel_10);//ȡż�β���ֵ
		}
	
		voltage = Bubble_Filter(adc_value) * REFER_VOLTAGE / CONVERT_BITS * Voltge_Parameter;	//��ѹ���㹫ʽ��voltage = adc����ֵ / ��������(3.3v/4096) *��ѹϵ��
		return voltage;
} 

/* MSH  get ��ѹ���� */
void print_voltage(void)
{
		char str[128];
		sprintf(str,"voltage:%.2f\r\n",get_voltage_value()); // 23.4Ϊ��ѹ��·ϵ��
		rt_kprintf(str);
}
MSH_CMD_EXPORT(print_voltage,print voltage[u]);


//���ADCֵ
//ch: @ref ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_Channel_0~ADC_Channel_16
//����ֵ:ת�����
u16 get_adc(u8 ch)   
{
		//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
		ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��			    

		ADC_SoftwareStartConv(ADC2);		//ʹ��ָ����ADC1�����ת����������	
		 
		while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//�ȴ�ת������

		return ADC_GetConversionValue(ADC2);	//�������һ��ADC1�������ת�����
}


//��ʼ��ADC															   
int  adc_init(void)
{    
		GPIO_InitTypeDef  GPIO_InitStructure;
		ADC_CommonInitTypeDef ADC_CommonInitStructure;
		ADC_InitTypeDef       ADC_InitStructure;
		
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOAʱ��
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); //ʹ��ADC1ʱ��

		 //��ʼ��ADC2ͨ��10 IO��
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PC0 ͨ��10
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
		GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��  
	 
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	  //ADC1��λ
		RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);	//��λ����	 
	 
		
		ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
		ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
		ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
		ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz 
		ADC_CommonInit(&ADC_CommonInitStructure);//��ʼ��
		
		ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ	
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
		ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
		ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1 
		ADC_Init(ADC2, &ADC_InitStructure);//ADC��ʼ��
		
		ADC_Cmd(ADC2, ENABLE);//����ADת����	
		return 1;
}

