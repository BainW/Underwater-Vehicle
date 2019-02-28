#include "init.h"


#include <board.h>
#include <elog_flash.h>

#include <spi_flash.h>
#include <spi_flash_sfud.h>

#include <easyflash.h>
/*---------------------- Constant / Macro Definitions -----------------------*/		

#define VoltgeParameter 23.4

/*----------------------- Variable Declarations -----------------------------*/

extern struct rt_event init_event;/* ALL_init �¼����ƿ� */
u32 volatge = 0;
u32 adc_value[10] = {0};

rt_spi_flash_device_t nor_flash;

struct rt_thread thread_sys_monitor;

/*----------------------- Function Implement --------------------------------*/



/**
 * ϵͳ����߳�
 * @param parameter parameter
 */







/**
 * ϵͳ��ʼ���߳�
 * @param parameter parameter
 */
void sys_init_thread(void* parameter){

    /* ��ʼ�� nor_flash Flash �豸 */
    if ((nor_flash = rt_sfud_flash_probe("nor_flash", "spi20")) == NULL) {
				rt_kprintf("Error! No find nor_flash!");
        return;
    }

    /* ��ʼ�� EasyFlash ģ�� */
    easyflash_init();

    /* ��ʼ����־ϵͳ */
    elog_init();
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));

#ifdef DEBUG
    elog_set_filter_lvl(ELOG_LVL_VERBOSE);
#elif RELEASE
    elog_set_filter_lvl(ELOG_LVL_INFO);
#endif

#ifdef ELOG_COLOR_ENABLE
    elog_set_text_color_enabled(true);
#endif
    /* ��ʼ��EasyLogger��Flash��� */
    elog_flash_init();
    /* ����EasyLogger */
    elog_start();
		
}


int rt_my_application_init(void)
{
    rt_thread_t sys_thread;


    sys_thread = rt_thread_create("sys init", sys_init_thread, NULL, 4096, 10, 10);
    if (sys_thread != NULL) {
        rt_thread_startup(sys_thread);
    }
		else {
				rt_kprintf("sys init error!");
		}

    return 0;
}
INIT_APP_EXPORT(rt_my_application_init);









void adc_thread_entry(void *parameter)
{
		while(1)
		{
			
				rt_thread_mdelay(1000);
		}
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
                    12,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (adc_tid != RT_NULL){
				adc_init();
				log_w("System Self-Checking... ");
				log_d("adc_init()");

			
				rt_event_send(&init_event, ADC_EVENT); //�����¼�  ��ʾ��ʼ�����
				rt_thread_startup(adc_tid);

		}
		return 0;
}
INIT_APP_EXPORT(adc_thread_init);



/* ð�� get��ѹ */
double get_vol(void)
{
		u8 i,j;
		u32 res = 0;   //reserve


		for(i = 0;i < 20;i+=2){
				adc_value[i/2] = get_adc(ADC_Channel_10);
				rt_thread_mdelay(10);
		}
		for(j = 0;j < 10;j++){
				for(i = 0;i < 9-j;i++){
						if( adc_value[i] > adc_value[i+1] ){ //��С����
								res = adc_value[i];
								adc_value[i] = adc_value[i+1];
								adc_value[i+1] = res;
						}
					
				}
		}
		volatge = (*(adc_value+4)+ *(adc_value+5)+ *(adc_value+6))/3;		
		return (double)volatge*3.3*VoltgeParameter/4096;
} 

/* MSH  get ��ѹ���� */
void get_voltage(void)
{
		char str[128];
		sprintf(str,"voltage:%.2f\r\n",(double)volatge*3.3*VoltgeParameter/4096 ); // 23.4Ϊ��ѹ��·ϵ��
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




