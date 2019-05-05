/*
 * servo.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ��ȴ������豸
 */
#include "sensor.h"
#include <rtthread.h>
#include <elog.h>
#include "drv_MS5837.h"
#include "gyroscope.h"
#include "stdio.h"
#include "drv_i2c.h"

extern struct rt_event init_event; /* ALL_init �¼����ƿ� */


/**
  * @brief  sensor_thread_entry(������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void sensor_thread_entry(void* parameter)
{
		rt_thread_mdelay(1000);
		Sensor.MS5837.Init_Depth = get_ms5837_pressure();
		rt_thread_mdelay(1000);
		Sensor.MS5837.Init_Depth = get_ms5837_pressure();
		while(1)
		{

				MS583703BA_getTemperature();//��ȡ�¶�
				MS583703BA_getPressure();   //��ȡ����ѹ

				Sensor.MS5837.Temperature = get_ms5837_temperature();
				Sensor.MS5837.Depth = get_ms5837_pressure();
		
			
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
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    20,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (sensor_tid != RT_NULL){

				if(MS5837_Init()){
						//rt_event_send(&init_event, MS5837_EVENT);
						log_i("MS5837_Init()");
						rt_thread_startup(sensor_tid);

				}
				else {
						log_e("MS5837_Init_Failed!");
				}

		}
		return 0;
}
INIT_APP_EXPORT(sensor_thread_init);




/* Get Pressure */
void get_pressure(void)
{
		static char str[50] = {0};


		MS583703BA_getTemperature();//��ȡ�¶�
		MS583703BA_getPressure();   //��ȡ����ѹ
	
		sprintf(str,"MS_Temp2:%f\n",MS5837_Temperature);
		rt_kprintf(str);
		rt_kprintf("MS_Pressure:%d\n",MS5837_Pressure);//MS5837_Pressure
}
MSH_CMD_EXPORT(get_pressure, get pressure[pa]);












