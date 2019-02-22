#include "init.h"
/*---------------------- Constant / Macro Definitions -----------------------*/		

u32 	FLASH_SIZE=16*1024*1024;	//FLASH ��СΪ16�ֽ�

/*----------------------- Variable Declarations -----------------------------*/

extern struct rt_event init_event;/* ALL_init �¼����ƿ�. */

struct  /* ����u8 Ϊ���� FLASH��д��ʹ��ʱ��Ҫ���� 10 �õ� ��ʵPWMֵ */
{
		u8 open_value;		//��е�� �򿪵�PWMֵ 
		u8 close_value;	 //��е�� �رյ�PWMֵ
}servo_motor;

/*----------------------- Function Implement --------------------------------*/

void pwm_thread_entry(void *parameter)
{
	
		while(1)
		{
				TIM_SetCompare1(TIM1,2000);  		//���ת���ź�   	ˮƽ�ƽ���1��
				TIM_SetCompare2(TIM1,4000);  		//���ת���ź�    ˮƽ�ƽ���2��
				TIM_SetCompare3(TIM1,6000); 		//���ת���ź�    ˮƽ�ƽ���3��
				TIM_SetCompare4(TIM1,10000);  	//���ת���ź�    ˮƽ�ƽ���4��
				rt_thread_mdelay(100);

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

    if (pwm_tid != RT_NULL){
				TIM1_PWM_Init(20000-1,168-1);	//168M/168=1Mhz�ļ���Ƶ��,��װ��ֵ20000������PWMƵ��Ϊ 1M/20000=50Hz.  

				LOG_I("pwm_init()");

				rt_thread_startup(pwm_tid);
				rt_event_send(&init_event, PWM_EVENT); //�����¼�  ��ʾ��ʼ�����
		}

		return 0;
}
INIT_APP_EXPORT(pwm_thread_init);



/*����е�ۡ���� �޸� ������ֵ��MSH���� */
static int servo_motor_openvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Usage: servo_motor_OpenValue_set 160\n");
				result = -RT_ERROR;
        goto _exit;
    }
		servo_motor.open_value = atoi(argv[1]);
		
		W25QXX_Write(&servo_motor.open_value,0,1);		//��0����ַ��д������
		rt_kprintf("Write_Successed! Current ser_OpenValue:  %d\n",servo_motor.open_value);
_exit:
    return result;
}
MSH_CMD_EXPORT(servo_motor_openvalue_set,ag: servo_motor_OpenValue_set 160);




/*����е�ۡ���� �޸� ���ر�ֵ�� MSH���� */
static int servo_motor_closevalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Usage: servo_motor_CloseValue_set 115\n");
				result = -RT_ERROR;
        goto _exit;
    }
		servo_motor.close_value = atoi(argv[1]);

		W25QXX_Write(&servo_motor.close_value,1,1);	//��1����ַ��д������
		
		rt_kprintf("Write_Successed! Current ser_CloseValue:  %d\n",servo_motor.close_value);
_exit:
    return result;
}
MSH_CMD_EXPORT(servo_motor_closevalue_set,ag: servo_motor_CloseValue_set 115);




/* list �����Ҫ���� */
void list_value(void)
{
		W25QXX_Read(&servo_motor.open_value,0,1);		// ��ַ0
		W25QXX_Read(&servo_motor.close_value,1,1);	// ��ַ1
	
		rt_kprintf("variable name    value\n");
    rt_kprintf("--------------  ---------\n");
	  rt_kprintf("ser_OpenValue  	 %d\n",servo_motor.open_value);
	  rt_kprintf("ser_CloseValue   %d\n",servo_motor.close_value);
    rt_kprintf("                         \n");
}
MSH_CMD_EXPORT(list_value,list some important values);



//TIM1 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��

void TIM1_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	//TIM1ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//ʹ��PORTEʱ��	
	
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9, GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_TIM1);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;//GPIOE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOE,&GPIO_InitStructure);              //��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ե�
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM14
}  


///*---------------------- Constant / Macro Definitions -----------------------*/

//#define TIM1_CH1_PIN 		60 //	E9
//#define TIM1_CH2_PIN 		64 //	E11
//#define TIM1_CH3_PIN 		66 //	E13
//#define TIM1_CH4_PIN 		67 //	E14

//#define PWM_DEV_NAME        "pwm"  /* PWM�豸���� */
//#define PWM_DEV_CHANNEL     1       /* PWMͨ�� */


///*----------------------- Variable Declarations -----------------------------*/

//struct rt_device_pwm *pwm_dev;      /* PWM�豸��� */
///*----------------------- Function Implement --------------------------------*/

//void pwm_thread_entry(void *parameter)
//{
//		rt_uint32_t period = 50000, pulse = 0;
//	
//    rt_pin_mode(TIM1_CH1_PIN, PIN_MODE_OUTPUT);
//	  rt_pin_mode(TIM1_CH2_PIN, PIN_MODE_OUTPUT);
//	  rt_pin_mode(TIM1_CH3_PIN, PIN_MODE_OUTPUT);
//	  rt_pin_mode(TIM1_CH4_PIN, PIN_MODE_OUTPUT);
//	
//		/* �����豸 */
//    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
//	  if (pwm_dev == RT_NULL)
//    {
//        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
//    }

//		/* ����PWM���ں�������Ĭ��ֵ */
//    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, 	 period, pulse);
//		rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL+1, period, pulse);
//		rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL+2, period, pulse);
//		rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL+3, period, pulse);
//	
//    /* ʹ���豸 */
//    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL	);
//		rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL+1);
//		rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL+2);
//		rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL+3);
//    while (1)
//    {	
//				rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 20000);
//				rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 40000);
//				rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 60000);
//				rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, 80000);
//			
//				rt_thread_mdelay(500);
//    }
//}





