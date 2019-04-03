
#include "sensor.h"
#include <rtthread.h>
#include <elog.h>
#include "drv_MS5837.h"
#include "gyroscope.h"

extern struct rt_event init_event;/* ALL_init �¼����ƿ� */


void sensor_thread_entry(void* parameter)
{
	
		while(1)
		{
					/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
				rt_enter_critical();
				
				JY901_Convert(&JY901); //JY901����ת��
				/* ���������� */
				rt_exit_critical();
			
				MS583703BA_getTemperature();//��ȡ�¶�
				rt_thread_mdelay(20);
				MS583703BA_getPressure();   //��ȡ����ѹ


				rt_thread_mdelay(10);
		}
}





int sensor_thread_init(void)
{
    rt_thread_t sensor_tid;
		/*������̬�߳�*/
    sensor_tid = rt_thread_create("sensor",//�߳�����
                    sensor_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    8,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (sensor_tid != RT_NULL){

				if(MS5837_Init()){
						rt_event_send(&init_event, MS5837_EVENT);
						log_i("MS5837_Init()");
				}
				else {
						log_e("MS5837_Init_Failed!");
				}

				rt_thread_startup(sensor_tid);
		}
		return 0;
}
INIT_APP_EXPORT(sensor_thread_init);



















