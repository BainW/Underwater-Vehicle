#define LOG_TAG    "light"

#include "light.h"
#include <rtthread.h>
#include <elog.h>
#include "sys.h"
#include <stdlib.h>
#include "drv_pwm.h"
#include <drivers/pin.h>

short light_value = 0;

/*******************************************
* �� �� ����Light_Output_Limit
* ��    �ܣ��ƹ������������
* ����������ƹ�����ֵ 0~100%
* �� �� ֵ��None
* ע    �⣺
********************************************/
int Light_Output_Limit(short *value)
{
		*value = *value >= 90 ? 90 : *value ;//�޷�
		*value = *value <= 0  ? 0   : *value ;//�޷�
		return *value;
}


/*��̽�յơ� �޸� ����ǰ�� MSH���� */
static int light(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: YunTai_medvalue_set 2000");
				result = -RT_ERROR;
        goto _exit;
    }

		if(atoi(argv[1]) <= 100 ){		
				light_value = atoi(argv[1]) ;
				log_i("Light Value:  %d",light_value);
		}
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(light,ag: light <0~100>);


/**
  * @brief  Light_Control(̽�յƿ���)
  * @param  ����ָ�� 0x00��������  0x01������  0x02������
  * @retval None
  * @notice 
  */

void Explore_Light_Control(uint8 *action)
{
		switch(*action)
		{
				case 0x01:light_value += 10;  //����
									break;  		
				case 0x02:light_value -=10; //����
									break;  
				default: break;
		}
		Light_Output_Limit(&light_value);
		TIM10_PWM_CH1_F6(light_value);
		TIM11_PWM_CH1_F7(light_value);
		*action = 0x00; //���������
}

/**
  * @brief  explore_light_thread_entry(������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void explore_light_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{
	
	

}


int light_thread_init(void)
{
    rt_thread_t servo_tid;
		/*������̬�߳�*/
    servo_tid = rt_thread_create("light",		 //�߳�����
                    explore_light_thread_entry,			 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    15,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (servo_tid != RT_NULL){
			
				TIM10_PWM_Init(100-1,1680-1);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)100������PWMƵ��Ϊ 1M/100=10KHz. 
				TIM11_PWM_Init(100-1,1680-1);	
				TIM_Cmd(TIM10, ENABLE); //ʹ��TIM10
				TIM_Cmd(TIM11, ENABLE); //ʹ��TIM11
				TIM10_PWM_CH1_F6(0);
				TIM11_PWM_CH1_F7(0);
				log_i("light_init()");
			
				rt_thread_startup(servo_tid);
		}

		return 0;
}
INIT_APP_EXPORT(light_thread_init);
