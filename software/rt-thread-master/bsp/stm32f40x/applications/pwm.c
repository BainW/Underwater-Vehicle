#include "pwm.h"

/*---------------------- Constant / Macro Definitions -----------------------*/

#define TIM1_CH1_PIN 		60 //	E9
#define TIM1_CH2_PIN 		64 //	E11
#define TIM1_CH3_PIN 		66 //	E13
#define TIM1_CH4_PIN 		67 //	E14

#define PWM_DEV_NAME        "pwm"  /* PWM�豸���� */
#define PWM_DEV_CHANNEL     1       /* PWMͨ�� */


/*----------------------- Variable Declarations -----------------------------*/

struct rt_device_pwm *pwm_dev;      /* PWM�豸��� */
/*----------------------- Function Implement --------------------------------*/

void pwm_thread_entry(void *parameter)
{
		rt_uint32_t period = 50000, pulse = 0;
	
    rt_pin_mode(TIM1_CH1_PIN, PIN_MODE_OUTPUT);
	  rt_pin_mode(TIM1_CH2_PIN, PIN_MODE_OUTPUT);
	  rt_pin_mode(TIM1_CH3_PIN, PIN_MODE_OUTPUT);
	  rt_pin_mode(TIM1_CH4_PIN, PIN_MODE_OUTPUT);
	
		/* �����豸 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
	  if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
    }

		/* ����PWM���ں�������Ĭ��ֵ */
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, 	 period, pulse);
		rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL+1, period, pulse);
		rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL+2, period, pulse);
		rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL+3, period, pulse);
	
    /* ʹ���豸 */
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL	);
		rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL+1);
		rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL+2);
		rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL+3);
    while (1)
    {	
				rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 20000);
				rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 40000);
				rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 60000);
				rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 80000);
			
				rt_thread_mdelay(500);
    }
}



int pwm_thread_init(void)
{
    rt_thread_t pwm_tid;
		/*������̬�߳�*/
    pwm_tid = rt_thread_create("pwm",//�߳�����
                    pwm_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (pwm_tid != RT_NULL)
     rt_thread_startup(pwm_tid);
		return 0;
}
INIT_APP_EXPORT(pwm_thread_init);





