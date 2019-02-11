#include "key.h"  

/*---------------------- Constant / Macro Definitions -----------------------*/

#define KEY_PIN  		79 	 //PD10

#define BOMA1_PIN	  14	 //PF4
#define BOMA2_PIN 	15	 //PF5

#define boma1_read 					rt_pin_read(BOMA1_PIN)
#define boma2_read 					rt_pin_read(BOMA2_PIN)

/*----------------------- Variable Declarations -----------------------------*/

int page_num = 0;
u8 boma_value = 0;	//�ݴ沦��״̬ �жϲ���״̬�Ƿ�ı�

/*----------------------- Function Implement --------------------------------*/

void key_thread_entry(void* parameter)// --- Buzzer   KEY   BOMA ---
{
		rt_pin_mode(KEY_PIN, PIN_MODE_INPUT_PULLUP);    //���ܰ���  ��������
	
		rt_pin_mode(BOMA1_PIN, PIN_MODE_INPUT_PULLUP);  //���뿪��  ��������
		rt_pin_mode(BOMA2_PIN, PIN_MODE_INPUT_PULLUP);  
		LOG_I("KEY_Init()");
		boma_value = boma_value_get();	//��ʼ���õ���ǰ����״̬ --> VehicleStatus
		LOG_I("Current: BOMA_Value = %d", boma_value);
	
    while (1)
    {
				key_down();
			
				if(boma_value != boma_value_get()){
						buzzer_once();	
						boma_value = boma_value_get();	
						rt_kprintf("\nCurrent Change: BOMA_Value = %d", boma_value);
				}
				rt_thread_mdelay(300);
    }
}


/* get 2λ����ֵ */
rt_uint8_t boma_value_get(void)
{
    rt_uint8_t val; //reserve(�洢)
    
		val = boma1_read *2 + boma2_read *1 + 1; //�õ�����״̬
    return val;
}

/* �������²��������� */
void key_down(void)  
{
		if (rt_pin_read(KEY_PIN) == PIN_LOW){
				buzzer_once();	
				page_num ++;
					
    }while(rt_pin_read(KEY_PIN) == PIN_LOW);


}


int key_thread_init(void)
{
   rt_thread_t key_tid;

    key_tid = rt_thread_create("key",
                    key_thread_entry,
                    RT_NULL,
                    512,
                    10,
                    10);

    if (key_tid != RT_NULL)
     rt_thread_startup(key_tid);
		return 0;
}
INIT_APP_EXPORT(key_thread_init);




