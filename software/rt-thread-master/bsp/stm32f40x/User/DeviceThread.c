/*
 * DeviceThread.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �豸��������
 */

#include "DeviceThread.h"
#include <rtthread.h>


/**
  * @brief  servo_thread_entry(�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void servo_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{



		while(1)
		{

				rt_thread_mdelay(100);
		}
	
}




/**
  * @brief  propeller_thread_entry(�ƽ�������������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void propeller_thread_entry(void *parameter)
{


		while(1)
		{

				rt_thread_mdelay(100);
		}
	
}






