#include "init.h"
#include <board.h>
#include <stdio.h>


/*----------------------- Variable Declarations -----------------------------*/

struct rt_event init_event;/* ALL_init �¼����ƿ� */

rt_thread_t 	init_tid;

void init_thread_entry(void* parameter)
{
	  rt_uint32_t e;
		rt_err_t result;

		 /* �����¼����ж��Ƿ����������ʼ����� �������������¼���־ */
    if (rt_event_recv(&init_event, (LED_EVENT | KEY_EVENT | BUZZ_EVENT | OLED_EVENT | GYRO_EVENT | ADC_EVENT | PWM_EVENT | W25Q128_EVENT),
                      RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                      RT_WAITING_FOREVER, &e) == RT_EOK)
    {
				LOG_W("System Self-Check Completed :0x%x ---> Success!!! ", e);
				result = rt_thread_suspend (init_tid);  //�̹߳���
			  if (result != RT_EOK){
						LOG_E("init_tid thread suspend failed.\n");
				}
		}
		rt_thread_mdelay(1000);

}


int All_thread_init(void)
{

		/*������̬�߳�*/
    init_tid = rt_thread_create("init",			 //�߳�����
                    init_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (init_tid != RT_NULL){
				LOG_I("Init_Init()");
				rt_thread_startup(init_tid);
		}
		return 0;
}
INIT_APP_EXPORT(All_thread_init);




/* ð�� get��ѹ */
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







int test(void)
{
    rt_uint8_t i;
    for(i = 1 ; i <= 3 ;i++){       
        rt_kprintf("Test, count : %d\r\n", i);
        rt_thread_mdelay(500);
    }
		rt_kprintf("Test-->successed!\n");
    return 0;
}
MSH_CMD_EXPORT(test, This is a test example.);


void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t delta;
    /* �����ʱ������ tick �� */
    us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));
    /* ��õ�ǰʱ�� */
    delta = SysTick->VAL;
    /* ѭ����õ�ǰʱ�䣬ֱ���ﵽָ����ʱ����˳�ѭ�� */
    while (delta - SysTick->VAL< us);
}


void delay_us(u32 nTimer)
{
	u32 i=0;
	for(i=0;i<nTimer;i++){
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}

void delay_ms(u32 nTimer)
{
		u32 i=1000*nTimer;
		delay_us(i);
}





