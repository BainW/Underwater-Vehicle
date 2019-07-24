#include "light.h"
#include <rtthread.h>
#include <elog.h>
#include "sys.h"
#include <stdlib.h>
#include "drv_pwm.h"

#include <drivers/pin.h>
int light_value = 0;

/*******************************************
* �� �� ����Servo_Output_Limit
* ��    �ܣ�����������
* �������������ֵ������ṹ���ַ 
* �� �� ֵ��None
* ע    �⣺
********************************************/
void Light_Output_Limit(int vlaue)
{
		vlaue = vlaue  > 100 ? 100 : vlaue ;//�޷�
		vlaue = vlaue  < 0   ? 0   : vlaue ;//�޷�
	
}


/*����̨����� �޸� ����ǰ�� MSH���� */
static int light(int argc, char **argv)
{
    int result = 0;
    if (argc > 1){
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
  * @brief  light_thread_entry(�����ʼ��������)
  * @param  void* parameter
  * @retval None
  * @notice 
  */
void explore_light_thread_entry(void *parameter)//�ߵ�ƽ1.5ms ������20ms  ռ�ձ�7.5% volatil
{

//		TIM10_PWM_Init(99,7);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  ������Ϊ500Hz��
//		TIM11_PWM_Init(99,7);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ(��PWM����)20000������PWMƵ��Ϊ 1M/20000=50Hz.  
//		TIM_Cmd(TIM10, ENABLE);  //ʹ��TIM1
//		TIM_Cmd(TIM11, ENABLE);  //ʹ��TIM4
		rt_pin_mode (18, PIN_MODE_OUTPUT);  //���ģʽ
		rt_pin_mode (19, PIN_MODE_OUTPUT);  //���ģʽ
		rt_pin_write(18, PIN_LOW);
		rt_pin_write(19, PIN_LOW);
		rt_thread_mdelay(1000);
		while(1)
		{
//				TIM_SetCompare1(TIM10,light_value);
//				TIM_SetCompare1(TIM11,light_value);
			
//				rt_pin_write(18, PIN_LOW);
//				rt_pin_write(19, PIN_LOW);
				rt_thread_mdelay(10);
		}
	
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

				log_i("light_init()");
			
				rt_thread_startup(servo_tid);
		}

		return 0;
}
INIT_APP_EXPORT(light_thread_init);
