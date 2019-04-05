/*
 * servo.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ����豸
 */
#include "servo.h"
#include "propeller.h"
#include "flash.h"
#include <rtthread.h>
#include <elog.h>
#include <stdlib.h>
#include "sys.h"

#define RoboticArm_MedValue  1500
#define YunTai_MedValue  		 1500

extern struct rt_event init_event;/* ALL_init �¼����ƿ�. */

ServoType RoboticArm = { //��е��
												 .OpenValue = 1800,
												 .CloseValue = 1300,
											   .CurrentValue = 1500
};
ServoType YunTai ={  //��̨
										.OpenValue = 1800,
										.CloseValue = 1300,
									  .CurrentValue = 1500
}; 



void servo_thread_entry(void *parameter)
{
		TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
	
		Propeller_Init();
		while(1)
		{
				TIM_SetCompare1(TIM1,1500);  		//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5%
				TIM_SetCompare2(TIM1,1500);  		
				TIM_SetCompare3(TIM1,1500); 		
				TIM_SetCompare4(TIM1,1500);  
			
				TIM_SetCompare1(TIM4,1500);  //����
				TIM_SetCompare2(TIM4,1500);  //����
				TIM_SetCompare3(TIM4,RoboticArm.CurrentValue);  //��е��
				TIM_SetCompare4(TIM4,1500);  //��̨
			
				rt_thread_mdelay(1);

		}
	
}


int servo_thread_init(void)
{
    rt_thread_t servo_tid;
		/*������̬�߳�*/
    servo_tid = rt_thread_create("pwm",//�߳�����
                    servo_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (servo_tid != RT_NULL){
				TIM1_PWM_Init(20000-1,168-1);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  
				TIM4_PWM_Init(20000-1,84-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  
				log_i("Servo_init()");
			
				rt_thread_startup(servo_tid);
				rt_event_send(&init_event, PWM_EVENT); //�����¼�  ��ʾ��ʼ�����
		}

		return 0;
}
INIT_APP_EXPORT(servo_thread_init);













/*����е�ۡ���� �޸� ���������ֵ��MSH���� */
static int robotic_arm_openvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 5000){
				RoboticArm.OpenValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current RoboticArm_OpenValue:  %d",RoboticArm.OpenValue);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(robotic_arm_openvalue_set,ag: robotic_arm_openvalue_set 160);




/*����е�ۡ���� �޸� ���������ֵ�� MSH���� */
static int robotic_arm_closevalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_closevalue_set 1150");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000){
				RoboticArm.CloseValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current RoboticArm_CloseValue:  %d",RoboticArm.CloseValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

		
		
_exit:
    return result;
}
MSH_CMD_EXPORT(robotic_arm_closevalue_set,ag: robotic_arm_closevalue_set 115);





/*����̨����� �޸� ���������ֵ��MSH���� */
static int yuntai_openvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 5000){
				YunTai.OpenValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current YunTai_OpenValue:  %d",RoboticArm.OpenValue);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_openvalue_set,ag: yuntai_openvalue_set 160);




/*����̨����� �޸� ���������ֵ�� MSH���� */
static int yuntai_arm_closevalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_closevalue_set 1150");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000){
				YunTai.CloseValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current YunTai_CloseValue:  %d",RoboticArm.CloseValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_arm_closevalue_set,ag: yuntai_arm_closevalue_set 115);





