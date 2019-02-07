#include "init.h"


/*----------------------- Variable Declarations -----------------------------*/
extern rt_uint8_t VehicleStatus;

/*---------------------- Constant / Macro Definitions -----------------------*/
//RGB�����ź�
#define LED_Red 		68 
#define LED_Green 	69
#define LED_Blue 		70

//RGB�ƹ������ӵ�+3.3V����ƽ 0�� 1��
#define LED_ON(led_pin) 						rt_pin_write(led_pin ,PIN_LOW )
#define LED_OFF(led_pin) 						rt_pin_write(led_pin ,PIN_HIGH)
#define LED_Turn(led_pin,status) 		rt_pin_write(led_pin ,status = ! status) //ȡ��


void led_thread_entry(void *parameter)
{
    rt_pin_mode( LED_Red, 	PIN_MODE_OUTPUT);//�������ģʽ	
    rt_pin_mode( LED_Green, PIN_MODE_OUTPUT);	
    rt_pin_mode( LED_Blue, 	PIN_MODE_OUTPUT);	
	
		rt_pin_write(LED_Red, 	PIN_HIGH);			//��ʼ��Ϊ�ߵ�ƽ ��Ϩ��
		rt_pin_write(LED_Green, PIN_HIGH);			
		rt_pin_write(LED_Blue,  PIN_HIGH);
		
		LED_ON(LED_Red);	//�ȴ�ϵͳ��ʼ�� 1s ��ϣ�ʹϵͳ���ȶ�
		rt_thread_mdelay(1000);
		LED_OFF(LED_Red);

	
    while (1)
    {
				led_blink_task();
    }
}

/* led��˸����ϵͳ��������ָʾ�ơ� */
void led_blink_task(void)
{
		static rt_uint8_t status = 1;
		if(boma_value_get() == 1)
		{
				LED_Turn(LED_Green,status);	//��ʼ��Ϊ�ߵ�ƽ ��Ϩ��
				rt_thread_mdelay(500);
		}
}
	




int led_thread_init(void)
{
    rt_thread_t led_tid;
		/*������̬�߳�*/
    led_tid = rt_thread_create("led",//�߳�����
                    led_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    5,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (led_tid != RT_NULL)
     rt_thread_startup(led_tid);
		return 0;
}
INIT_APP_EXPORT(led_thread_init);





