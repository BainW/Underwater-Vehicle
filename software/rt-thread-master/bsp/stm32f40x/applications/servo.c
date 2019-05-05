/*
 * servo.c
 *
 *  Created on: 2019��3��30��
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
#include "Return_Data.h"

#define RoboticArm_MedValue  1500
#define YunTai_MedValue  		 2000

uint8 RoboticArm_Speed = 5;
uint8 Yuntai_Speed = 5;

ServoType RoboticArm;  //��е��
ServoType YunTai;      //��̨

//YunTai.OpenValue = 1500;   //����
//YunTai.CloseValue = 2500;  //����
//YunTai.CurrentValue = 2000;

uint16 propeller_power = 1500;

extern struct rt_event init_event;/* ALL_init �¼����ƿ�. */
/*******************************************
* �� �� ����Servo_Output_Limit
* ��    �ܣ�����������
* �������������ֵ��Value   ����޷���max  ��������޷�:min
* �� �� ֵ���޷���� Value
* ע    �⣺
********************************************/
uint16 Servo_Output_Limit(uint16* Value,uint16 max,uint16 min)
{
		*Value = *Value > max ? max: *Value;//�����޷�
		*Value = *Value < min ? min: *Value;//�����޷�
	
		return *Value;
}



void RoboticArm_Control(uint8 *action)
{
		switch(*action)
		{
				case 0x01:RoboticArm.CurrentValue += RoboticArm.Speed;
									if(RoboticArm.CurrentValue >= RoboticArm.MaxValue){device_hint |= 0x01;}//��е�۵�ͷ��־
									else {device_hint &= 0xFE;}; //�����е�۵�ͷ��־
									RoboticArm.CurrentValue = Servo_Output_Limit(&RoboticArm.CurrentValue,RoboticArm.MaxValue,RoboticArm.MinValue);
									break;
				case 0x02:RoboticArm.CurrentValue -= RoboticArm.Speed;
									if(RoboticArm.CurrentValue <= RoboticArm.MinValue){device_hint |= 0x01;}//��е�۵�ͷ��־
									else {device_hint &= 0xFE;}; //�����е�۵�ͷ��־
									RoboticArm.CurrentValue = Servo_Output_Limit(&RoboticArm.CurrentValue,RoboticArm.MaxValue,RoboticArm.MinValue);
									break;
				default:break;
		}
		TIM_SetCompare3(TIM4,RoboticArm.CurrentValue);
		*action = 0x00; //���������
}

void YunTai_Control(uint8 *action)
{
		switch(*action)
		{
				case 0x01:YunTai.CurrentValue -= YunTai.Speed;  //����
						if(YunTai.CurrentValue <= YunTai.MinValue){device_hint |= 0x02;}//��̨��ͷ��־
						else {device_hint &= 0xFD;}; //�����̨��ͷ��־
						YunTai.CurrentValue = Servo_Output_Limit(&YunTai.CurrentValue,YunTai.MaxValue,YunTai.MinValue);
						break;  
						
				case 0x02:YunTai.CurrentValue += YunTai.Speed; //����
						if(YunTai.CurrentValue >= YunTai.MaxValue){device_hint |= 0x02;}//��̨��ͷ��־
						else {device_hint &= 0xFD;}; //�����̨��ͷ��־
						YunTai.CurrentValue = Servo_Output_Limit(&YunTai.CurrentValue,YunTai.MaxValue,YunTai.MinValue);
						break;  

				case 0x03:YunTai.CurrentValue = YunTai_MedValue;break;   //����
				default: break;
		}
		TIM_SetCompare4(TIM4,YunTai.CurrentValue); 
		*action = 0x00; //���������
}
/**
  * @brief  servo_thread_entry(���������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void servo_thread_entry(void *parameter)
{
		TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
	
		Propeller_Init();

		while(1)
		{
				
				TIM_SetCompare1(TIM1,propeller_power);  		//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5%
				TIM_SetCompare2(TIM1,propeller_power);  		
				TIM_SetCompare3(TIM1,propeller_power); 		
				TIM_SetCompare4(TIM1,propeller_power);  
			
				TIM_SetCompare1(TIM4,propeller_power);  //����
				TIM_SetCompare2(TIM4,propeller_power);  //����

			
				rt_thread_mdelay(10);

		}
	
}


int servo_thread_init(void)
{
    rt_thread_t servo_tid;
		/*������̬�߳�*/
    servo_tid = rt_thread_create("pwm",//�߳�����
                    servo_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (servo_tid != RT_NULL){
				TIM1_PWM_Init(20000-1,168-1);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  
				TIM4_PWM_Init(20000-1,84-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  
				log_i("Servo_init()");
			
				rt_thread_startup(servo_tid);
				//rt_event_send(&init_event, PWM_EVENT); //�����¼�  ��ʾ��ʼ�����
		}

		return 0;
}
INIT_APP_EXPORT(servo_thread_init);








/*����е�ۡ���� �޸� ���������ֵ��MSH���� */
static int propeller_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 2000){
				propeller_power = atoi(argv[1]);
				log_d("Current propeller_set:  %d",propeller_power);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_set,ag: propeller set 2000);




/*����е�ۡ���� �޸� �ٶ�ֵ */
static int robotic_arm_speed_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_Speed 10");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 255){
				RoboticArm.Speed = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current RoboticArm_Speed:  %d",RoboticArm.Speed);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(robotic_arm_speed_set,ag: robotic_arm_speed_set 10);


/*����е�ۡ���� �޸� ���������ֵ��MSH���� */
static int robotic_arm_maxvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_Maxvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 5000){
				RoboticArm.MaxValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current RoboticArm_MaxValue:  %d",RoboticArm.MaxValue);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(robotic_arm_maxvalue_set,ag: robotic_arm_openvalue_set 2000);




/*����е�ۡ���� �޸� ���������ֵ�� MSH���� */
static int robotic_arm_minvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: RoboticArm_minvalue_set 1150");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000){
				RoboticArm.MinValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current RoboticArm_minValue:  %d",RoboticArm.MinValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

		
		
_exit:
    return result;
}
MSH_CMD_EXPORT(robotic_arm_minvalue_set,ag: robotic_arm_closevalue_set 1500);


/*����е�ۡ���� �޸� �ٶ�ֵ */
static int yuntai_speed_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: yuntai_speed_set 5");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 255){
				YunTai.Speed = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current Yuntai_Speed:  %d",YunTai.Speed);
		}
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_speed_set,ag: yuntai_speed_set 5);

/*����̨����� �޸� ���������ֵ��MSH���� */
static int yuntai_maxvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_maxvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 5000){
				YunTai.MaxValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current YunTai_MaxValue:  %d",YunTai.MaxValue);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_maxvalue_set,ag: yuntai_maxvalue_set 2500);




/*����̨����� �޸� ���������ֵ�� MSH���� */
static int yuntai_minvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_minvalue_set 1150");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000){
				YunTai.MinValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current YunTai_minValue:  %d",YunTai.MinValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_minvalue_set,ag: yuntai_arm_closevalue_set 1500);

/*����̨����� �޸� ���������ֵ�� MSH���� */
static int yuntai_medvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_medvalue_set 2000");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 3000){
				YunTai.MedValue = atoi(argv[1]);
				Flash_Update();
				log_d("Write_Successed! Current YunTai_MedValue:  %d",YunTai.MedValue);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(yuntai_medvalue_set,ag: yuntai_arm_medvalue_set 2000);





