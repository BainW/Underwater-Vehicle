#define LOG_TAG    "self_check"

#include "init.h"
#include "self_check.h"

/*----------------------- Variable Declarations -----------------------------*/

struct rt_event init_event;/* ALL_init �¼����ƿ�. */

rt_thread_t self_check_tid;

/*----------------------- Function Implement --------------------------------*/


void self_check_entry(void* parameter)
{
	  rt_uint32_t e;
	
	  RCC_ClocksTypeDef Get_RCC_Clocks;
		RCC_GetClocksFreq(&Get_RCC_Clocks); //��ȡϵͳʱ��

		 /* �����¼����ж��Ƿ����������ʼ����� �������������¼���־ */
    if (rt_event_recv(&init_event, (LED_EVENT | KEY_EVENT | BUZZ_EVENT | OLED_EVENT | GYRO_EVENT | ADC_EVENT | PWM_EVENT | CAM_EVENT ),
                      RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
                      RT_WAITING_FOREVER, &e) == RT_EOK)
    {
				log_w("System Self-Check:0x%x --> Success!", e);
				log_v("Clock: %d Hz.",Get_RCC_Clocks.SYSCLK_Frequency); //��ӡϵͳʱ��
		}
		else {
				log_w("some devices initialization failed.");
		}
}


int Self_Check_thread_init(void)
{
	
		/*������̬�߳�*/
    self_check_tid = rt_thread_create("self_check",			 //�߳�����
                    self_check_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    8,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (self_check_tid != RT_NULL){
				log_i("SelfCheck_Init()");
				rt_thread_startup(self_check_tid);
		}
		return 0;
}
INIT_APP_EXPORT(Self_Check_thread_init);









