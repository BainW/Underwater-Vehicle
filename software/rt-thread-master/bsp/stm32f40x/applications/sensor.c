/*
 * servo.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �������豸����ȴ�������CPU������ADC��ѹ��
 */
 
 #define LOG_TAG    "sensor"
#include "sensor.h"
#include <rtthread.h>
#include <elog.h>
#include "drv_MS5837.h"
#include "gyroscope.h"
#include "stdio.h"
#include "drv_i2c.h"
#include "drv_adc.h"
#include "drv_cpu_temp.h"
#include "drv_cpuusage.h"
#include "filter.h"
#include "drv_spl1301.h"

//#define Using_SPL1301 

extern struct rt_event init_event; /* ALL_init �¼����ƿ� */

Sensor_Type Sensor;//����������
/**
  * @brief  sensor_lowSpeed_thread_entry(���ٻ�ȡ������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void sensor_lowSpeed_thread_entry(void* parameter)
{
		uint8 cpu_usage_major, cpu_usage_minor; //����λ��С��λ
		rt_thread_mdelay(1500);//�ȴ�1.5sϵͳ�ȶ��ٻ�ȡ����

		while(1)
		{
			
				Sensor.CPU.Temperature = get_cpu_temp(); //��ȡCPU�¶�
				Sensor.PowerSource.Voltage = get_voltage_value();//��ȡ��Դ��ѹֵ
			
				cpu_usage_get(&cpu_usage_major, &cpu_usage_minor); //��ȡCPUʹ����
				Sensor.CPU.Usage = cpu_usage_major + (float)cpu_usage_minor/100;
			

				rt_thread_mdelay(1000);
		}
}

/**
  * @brief  sensor_highSpeed_thread_entry(���ٻ�ȡ������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
float spl_pressure = 0.0f;
float spl_init_pressure = 0.0f;
float spl_res_pressure = 0.0f;
float spl_init_temp = 0.0f;
float spl_temp = 0.0f;

void sensor_highSpeed_thread_entry(void* parameter)
{
		static uint8 ON_OFF = 0; //��������
		rt_thread_mdelay(1500);//�ȴ�1.5sϵͳ�ȶ��ٻ�ȡ����

		while(1)
		{
				JY901_Convert(&Sensor.JY901); //JY901����ת��
#ifdef Using_SPL1301		
			
				spl1301_get_raw_temp();
				spl1301_get_raw_pressure();
				if(0 == ON_OFF){
						Sensor.MS5837.Init_PessureValue = spl1301_get_pressure(); //��ȡѹ����ֵ 
						ON_OFF = 1; //����
				}
				Sensor.MS5837.PessureValue = spl1301_get_pressure();
				Sensor.MS5837.Temperature = spl1301_get_temperature();
#else
				MS5837_Convert();   //MS5837�豸����ת��
				if(0 == ON_OFF){
						Sensor.MS5837.Init_PessureValue = get_ms5837_init_pressure(); //��ȡѹ����ֵ 
						ON_OFF = 1; //����
				}

#endif
			
				Sensor.Depth = (int)((int)(Sensor.MS5837.PessureValue - Sensor.MS5837.Init_PessureValue)/10);		
				rt_thread_mdelay(20);
		}
}



/*******************************�̳߳�ʼ��*******************************************/
int sensor_thread_init(void)
{
    rt_thread_t sensor_lowSpeed_tid; //���ٻ�ȡ�Ĵ���������ӿ�
	  rt_thread_t sensor_highSpeed_tid;//���ٻ�ȡ�Ĵ���������ӿ�
		/*������̬�߳�*/
    sensor_lowSpeed_tid = rt_thread_create("sensor",  //�߳�����
                    sensor_lowSpeed_thread_entry,		 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    30,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

	  sensor_highSpeed_tid = rt_thread_create("sensor",  //�߳�����
                    sensor_highSpeed_thread_entry,		 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    15,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (sensor_lowSpeed_tid != RT_NULL && sensor_highSpeed_tid != RT_NULL){
			
#ifdef Using_SPL1301
				spl1301_init();
#else
			  if(MS5837_Init()){
						log_i("MS5837_Init()");

				}
				else {
						log_e("MS5837_Init_Failed!");
				}
				
#endif
				if(adc_init()){
						log_i("Adc_Init()");
				}
				
				rt_thread_startup(sensor_lowSpeed_tid);
				rt_thread_startup(sensor_highSpeed_tid);
		}
		return 0;
}
INIT_APP_EXPORT(sensor_thread_init);


void MS5837_Convert(void)//MS5837����ת��
{
		static uint32 res_value[10] = {0};
		static uint8 i = 0;
		MS583703BA_getTemperature();//��ȡ�ⲿ�¶�
		MS583703BA_getPressure();   //��ȡˮѹ

		if(i >= 9){i = 0;}
		res_value[i++] = get_ms5837_pressure();
		
		Sensor.MS5837.Temperature  = get_ms5837_temperature();
		Sensor.MS5837.PessureValue = Bubble_Filter(res_value);
}



/* ��ӡ��������Ϣ */
void print_sensor_info(void)
{
		log_i("    variable        |  value");
		log_i("--------------------|-----------");
	
		print_JY901_info();  //��ӡ�Ƕ�
				


		log_i("--------------------|-----------");
		log_i("     Voltage        |   %0.3f",Sensor.PowerSource.Voltage); //��ѹ
		log_i("     Current        |   %d",Sensor.PowerSource.Current); //����
		log_i("--------------------|-----------");
		log_i(" Water Temperature  |   %0.3f",Sensor.MS5837.Temperature);  //ˮ��
		log_i("sensor_Init_Pressure|   %d",Sensor.MS5837.Init_PessureValue); //��ȴ�������ʼѹ��ֵ	
		log_i("   sensor_Pressure  |   %d",Sensor.MS5837.PessureValue); //��ȴ�������ǰѹ��ֵ	
		log_i("     Depth          |   %d",Sensor.Depth); //���ֵ
	

		

}
MSH_CMD_EXPORT(print_sensor_info, printf gysocope & PowerSource & pressure);












