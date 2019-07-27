/*
 * DeviceThread.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �豸��������
 */

#include "DeviceThread.h"
#include <rtthread.h>
#include <elog.h>
#include "propeller.h"
#include "servo.h"
#include "light.h"
#include "rc_data.h"
#include "Control.h"
#include "PropellerControl.h"
#include "sys.h"
#include "focus.h"
#include "debug.h"

int button_value_test = 0;
/**
  * @brief  propeller_thread_entry(�ƽ�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void propeller_thread_entry(void *parameter)
{
		Propeller_Init();       //�ƽ�����ʼ��
		while(1)
		{
			
				Control_Cmd_Get(&ControlCmd); //���������ȡ ������λ������������ڴˡ�Important��

				if(UNLOCK == ControlCmd.All_Lock){ //�������
						Convert_RockerValue(&Rocker); //ң������ ת�� Ϊ�ƽ�������
				}

				if(FOUR_AXIS == VehicleMode){
						FourtAxis_Control(&Rocker);
						ROV_Depth_Control(&Rocker);
				}
				else if(SIX_AXIS == VehicleMode){
						SixAxis_Control(&Rocker);
				}
				else{
						log_e("not yet set vehicle mode !");
				}
				
				Propeller_Output(); //�ƽ�����ʵPWM���		
				rt_thread_mdelay(10);
		}
	
}












/**
  * @brief  servo_thread_entry(�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void devices_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{



		while(1)
		{
			
				if(WORK == WorkMode){
						Explore_Light_Control(&ControlCmd.Light);  //̽�յƿ���
						YunTai_Control(&ControlCmd.Yuntai); //��̨����
						RoboticArm_Control(&ControlCmd.Arm);//��е�ۿ���
						Focus_Zoom_Camera_Control(&ControlCmd.Focus);//�佹����ͷ����
				}
				else if(DEBUG == WorkMode)
				{	
						button_value_test = get_button_value(&ControlCmd);
						Debug_Mode(button_value_test);
				}
				rt_thread_mdelay(20);
		}
	
}


int propeller_thread_init(void)
{
    rt_thread_t propeller_tid;
		/*������̬�߳�*/
    propeller_tid = rt_thread_create("propoller",//�߳�����
                    propeller_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (propeller_tid != RT_NULL){

				TIM1_PWM_Init(20000-1,168-1);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  ������Ϊ500Hz��
				TIM3_PWM_Init(20000-1,84-1);  //��ȡ��
			
				TIM4_PWM_Init(20000-1,84-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  
				TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
				TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM1
				TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
	
				rt_thread_startup(propeller_tid);
		}

		return 0;
}
INIT_APP_EXPORT(propeller_thread_init);

int devices_thread_init(void)
{
    rt_thread_t devices_tid;
		/*������̬�߳�*/
    devices_tid = rt_thread_create("devices",//�߳�����
                    devices_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    12,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (devices_tid != RT_NULL){

				log_i("Devices_init()");
				rt_thread_startup(devices_tid);
				//rt_event_send(&init_event, PWM_EVENT); //�����¼�  ��ʾ��ʼ�����
		}

		return 0;
}
INIT_APP_EXPORT(devices_thread_init);






