#include "adc_convert.h"


/*----------------------- Variable Declarations -----------------------------*/




/*----------------------- Function Implement --------------------------------*/

void adc_thread_entry(void *parameter)
{

		adc_init();
		LOG_I("adc_init()");
	
		while(1)
		{
				//get_vol(ADC_Channel_10,1);
				//LOG_E("volage:%d",get_voltage(ADC_Channel_10,10));
				rt_thread_mdelay(1000);

		}
	
}


int adc_thread_init(void)
{
    rt_thread_t adc_tid;
		/*������̬�߳�*/
    adc_tid = rt_thread_create("adc",				 //�߳�����
                    adc_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (adc_tid != RT_NULL)
     rt_thread_startup(adc_tid);
		return 0;
}
INIT_APP_EXPORT(adc_thread_init);




u16 get_vol(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t=0;
	for(t=0;t<times;t++)
	{
		temp_val+=get_adc(ch);
	//	delay_ms(1);
	}
	return temp_val/times;
} 





void get_voltage(void)
{
	u32 temp_val=0;
	u8 t=0;
	char str[128];
	for(t=0;t<5;t++)
	{
		temp_val+=get_adc(ADC_Channel_10);

	}
	
	sprintf(str,"voltage:%.3f\r\n",(float)temp_val);
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

	 //��ʼ��ADC1ͨ��10 IO��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PC0 ͨ��10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;//����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��  
 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	  //ADC1��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);	//��λ����	 
 
	
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; //DMAʧ��
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











