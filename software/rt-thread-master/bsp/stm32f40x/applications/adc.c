/*
 * adc.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ADC��ѹ�ɼ�   ��ѹ��ʱ�ɼ�ת����Ϊ5v����ʱΪ70v��bug��
 */
#define LOG_TAG    "adc"

#include "adc.h"
#include <rtthread.h>
#include <elog.h>
#include "sys.h"
#include <stdio.h>

/*---------------------- Constant / Macro Definitions -----------------------*/		

#define REFER_VOLTAGE       3.3         // �ο���ѹ 3.3V,���ݾ��ȳ���100����2λС��
#define CONVERT_BITS        (1 << 12)   // ת��λ��Ϊ12λ 
#define Voltge_Parameter    23.4						// 23.4 ��ѹ��ѹϵ��

/*----------------------- Variable Declarations -----------------------------*/

extern struct rt_event init_event;/* ALL_init �¼����ƿ� */
float  volatge = 0.0f;
uint32 adc_value[10] = {0};


/*----------------------- Function Implement --------------------------------*/




void adc_thread_entry(void *parameter)
{
		while(1)
		{
			
				rt_thread_mdelay(1000);
		}
}



/* ð�� get��ѹ */
float get_vol(void)
{
		uint8 i,j;
		uint32 res = 0;   //reserve
		double vol_bubble = 0.0f;

		for(i = 0;i < 20;i+=2){
				adc_value[i/2] = get_adc(ADC_Channel_10);//ȡż��

		}
		
		for(j = 0;j < 10-1;j++){
				for(i = 0;i < 9-j;i++){
						if( adc_value[i] > adc_value[i+1] ){ //����
								res = adc_value[i];
								adc_value[i] = adc_value[i+1];
								adc_value[i+1] = res;
						}
				}
		}
		vol_bubble = (float)(adc_value[4]+adc_value[5]+adc_value[6])/3;	
		
		volatge = vol_bubble * REFER_VOLTAGE / CONVERT_BITS * Voltge_Parameter;		
		return volatge;
} 

/* MSH  get ��ѹ���� */
void get_voltage(void)
{
		char str[128];
		sprintf(str,"voltage:%.2f\r\n",volatge ); // 23.4Ϊ��ѹ��·ϵ��
		rt_kprintf(str);
}
MSH_CMD_EXPORT(get_voltage,get voltage[u]);


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
void  adc_init(void)
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

}


int adc_thread_init(void)
{
		rt_err_t result;
    rt_thread_t adc_tid;
    /* ��ʼ���¼����� */
    result = rt_event_init(&init_event, "event", RT_IPC_FLAG_FIFO);
	
		
    if (result != RT_EOK){
        log_e("init event failed.\n");
        return -1;
		}
		/*������̬�߳�*/
    adc_tid = rt_thread_create("adc",				 //�߳�����
                    adc_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    15,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (adc_tid != RT_NULL){
				adc_init();
				log_i("adc_init()");

				rt_event_send(&init_event, ADC_EVENT); //�����¼�  ��ʾ��ʼ�����
				rt_thread_startup(adc_tid);
		}
		return 0;
}
INIT_APP_EXPORT(adc_thread_init);


