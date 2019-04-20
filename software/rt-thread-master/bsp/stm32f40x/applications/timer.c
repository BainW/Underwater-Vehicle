/*
 * timer.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ��ʱ��
 */
#define LOG_TAG    "timer"

#include <string.h>
#include "timer.h"
#include "flash.h"
#include <rtthread.h>
#include <elog.h>
#include "drv_MS5837.h"
#include  "Control.h"
#include "gyroscope.h"
#include "debug.h"
#include "Return_Data.h"
/*---------------------- Constant / Macro Definitions -----------------------*/



/*----------------------- Variable Declarations -----------------------------*/




/*----------------------- Function Implement --------------------------------*/


/**
  * @brief  timer1_out(��ʱ���жϺ���)
  * @param  void* parameter
  * @retval None
  * @notice ��ʱ���в��ܴ�����ʱ�����ͷ��̵߳Ĳ���
  */
static void timer1_out(void* parameter)// ��ʱ��1��ʱ����  ����JY901ģ������ת��
{
		static int count = 0;
		uint8 begin_buff[3] = {0xAA,0x55,0x00};
		count ++;

		/* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
		rt_enter_critical();
				
		JY901_Convert(&Sensor.JY901); //JY901����ת��

		/* ���������� */
		rt_exit_critical();
		Convert_Return_Computer_Data();
	
		Send_Buffer_Agreement(begin_buff,Return_Data,18);
		Angle_Control();
	
}



int timer1_init(void)
{
		/* ��ʱ���Ŀ��ƿ� */
		static rt_timer_t timer1;
    /* ������ʱ��1 */
    timer1 = rt_timer_create("timer1",  /* ��ʱ�������� timer1 */
                        timer1_out, 		  /* ��ʱʱ�ص��Ĵ����� */
                        RT_NULL, 			  /* ��ʱ��������ڲ��� */
                        1,      			  /* ��ʱ���ȣ���OS TickΪ��λ����5��OS Tick   --> 50MS*/  
                        RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_HARD_TIMER); /* �����Զ�ʱ�� */
    /* ������ʱ�� */
    if (timer1 != RT_NULL){ 
				
				rt_timer_start(timer1);
				
		}

    return 0;
}
INIT_APP_EXPORT(timer1_init);








