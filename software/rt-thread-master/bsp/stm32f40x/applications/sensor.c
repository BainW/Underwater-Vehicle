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


void sensor_highSpeed_thread_entry(void* parameter)
{

		rt_thread_mdelay(3000);//�ȴ�3sϵͳ�ȶ��ٻ�ȡ����

		while(1)
		{
				JY901_Convert(&Sensor.JY901); //JY901����ת��
				
				Depth_Sensor_Data_Convert();  //�������ת��

				Sensor.Depth = (int) ((int)(Sensor.MS5837.PessureValue - Sensor.MS5837.Init_PessureValue));		
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
			
				if(AUV_Mode == VehicleMode){
						spl1301_init();        //AUVʹ�õĴ�����
						log_i("SPL1301_init()");
				}
				else{
						if(MS5837_Init()){
								log_i("MS5837_Init()");
						}
						else {
								log_e("MS5837_Init_Failed!");
						}
				}
				
				if(adc_init()){ //ADC��ѹ�ɼ���ʼ��
						log_i("Adc_Init()");
				}
				
				rt_thread_startup(sensor_lowSpeed_tid);
				rt_thread_startup(sensor_highSpeed_tid);
		}
		return 0;
}
INIT_APP_EXPORT(sensor_thread_init);


void Depth_Sensor_Data_Convert(void)//��ȴ���������ת��
{
		static uint32 value[10] = {0};
		static uint8 ON_OFF = 0; //��������
		static uint8 i = 0;
		
		if(AUV_Mode == VehicleMode){  //AUVʹ�ø�� SPL1301
				spl1301_get_raw_temp();
				spl1301_get_raw_pressure();//����������ת��
				
				if(ON_OFF == 0){
						ON_OFF = 1;
						Sensor.MS5837.Init_PessureValue = get_spl1301_pressure();//��ȡ��ʼ������
				}
				for(i = 0;i < 10;i++){
						value[i++] = get_spl1301_pressure();//��ȡ1������
				}
				Sensor.MS5837.Temperature = get_spl1301_temperature();
				Sensor.MS5837.PessureValue = Bubble_Filter(value);
			
		}
		else{ //����ΪROVʹ��MS5837
				MS583703BA_getTemperature();//�Ȼ�ȡ�ⲿ�¶�,��Ϊ�˸���ȴ����������¶�����У׼
				MS583703BA_getPressure();   //��ȡˮѹ
				
				if(ON_OFF == 0){
						ON_OFF = 1;
						Sensor.MS5837.Init_PessureValue = get_ms5837_pressure();//��ȡ��ʼ������
				}
				for(i = 0;i < 10;i++){
						value[i++] = get_ms5837_pressure();//��ȡ10��
				}
				Sensor.MS5837.Temperature  = get_ms5837_temperature();
				Sensor.MS5837.PessureValue = Bubble_Filter(value);
				
				
		}

		

		
		

}


/* ��ӡ��������Ϣ */
void print_sensor_info(void)
{
		log_i("    variable        |  value");
		log_i("--------------------|-----------");
	
		log_i("      Roll          |  %+0.3f",Sensor.JY901.Euler.Roll);
		log_i("      Pitch         |  %+0.3f",Sensor.JY901.Euler.Pitch);
		log_i("      Yaw           |  %+0.3f",Sensor.JY901.Euler.Yaw);
		log_i("--------------------|-----------");
		log_i("      Acc.x         |  %+0.3f",Sensor.JY901.Acc.x);
		log_i("      Acc.y         |  %+0.3f",Sensor.JY901.Acc.y);//
		log_i("      Acc.z         |  %+0.3f",Sensor.JY901.Acc.z);//
		log_i("--------------------|-----------");
		log_i("      Gyro.x        |  %+0.3f",Sensor.JY901.Gyro.x);
		log_i("      Gyro.y        |  %+0.3f",Sensor.JY901.Gyro.y);//	
		log_i("      Gyro.z        |  %+0.3f",Sensor.JY901.Gyro.z);//	
		log_i("  JY901_Temperature |  %+0.3f",Sensor.JY901.Temperature);//					

	
		log_i("--------------------|-----------");
		log_i("     Voltage        |  %0.3f",Sensor.PowerSource.Voltage); //��ѹ
		log_i("     Current        |  %d",Sensor.PowerSource.Current);    //����
		log_i("--------------------|-----------");
		log_i(" Water Temperature  |  %0.3f",Sensor.MS5837.Temperature);    //ˮ��
		log_i("sensor_Init_Pressure|  %d",Sensor.MS5837.Init_PessureValue); //��ȴ�������ʼѹ��ֵ	
		log_i("   sensor_Pressure  |  %d",Sensor.MS5837.PessureValue); 		 //��ȴ�������ǰѹ��ֵ	
		log_i("     Depth          |  %d",Sensor.Depth); 									 //���ֵ
		log_i("--------------------|-----------");	
		log_i("    CPU.Usages      |  %0.3f",	Sensor.CPU.Temperature); //CPU�¶�
		log_i("   CPU.Temperature  |  %0.3f",	Sensor.CPU.Usage); 			 //CPUʹ����

		

}
MSH_CMD_EXPORT(print_sensor_info, printf gysocope & PowerSource & pressure);












