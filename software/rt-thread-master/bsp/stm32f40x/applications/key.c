/*
 * key.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ��������������
 */
#define LOG_TAG    "key"

#include "key.h"
#include "buzzer.h"
#include "oled.h"
#include <drivers/pin.h>
#include <elog.h>
/*---------------------- Constant / Macro Definitions -----------------------*/
#define KEY_PIN  						88 	 //PDG3   

#define WIFI_CONNECT_PIN    80 	 //PD11   WIFI����IO���
#define WIFI_RELOAD_PIN     68 	 //PE15   WIFI��λ

#define BOMA3_PIN 					13	 //PF3
#define BOMA2_PIN	  				14	 //PF4
#define BOMA1_PIN 					15	 //PF5

#define boma1_read 					rt_pin_read(BOMA1_PIN)
#define boma2_read 					rt_pin_read(BOMA2_PIN)
#define boma3_read 					rt_pin_read(BOMA3_PIN)

#define wifi_read 					rt_pin_read(WIFI_CONNECT_PIN)
/*----------------------- Variable Declarations -----------------------------*/
/* ALL_init �¼����ƿ�. */
extern struct rt_event init_event;
extern OledType oled;

uint8 boma_value = 0;	//�ݴ沦��״̬ �жϲ���״̬�Ƿ�ı�

/*----------------------- Function Implement --------------------------------*/

void key_thread_entry(void* parameter)// --- KEY   BOMA ---
{
		if(System_NORMAL_STATUS == boma_value_get())
				//buzzer_bibi(3,1);
    while (1)
    {
				if(boma_value != boma_value_get()){
						buzzer_bibi(1,1);	
						boma_value = boma_value_get();	
						log_i("\nCurrent Change: BOMA_Value = %d", boma_value);
				}
				rt_thread_mdelay(10);
    }
}


/* get 2λ����ֵ */
uint8 boma_value_get(void)
{
    uint8 val; //reserve(�洢)
    
		val = boma1_read *2 + boma2_read *1 + 1; //�õ�����״̬
    return val;
}

/**
	* @brief  wifi_connect_get(wifi����)
  * @param  None
  * @retval 1 ������   0������
  * @notice 
  */
uint8 wifi_connect_get(void)
{
		return !wifi_read; 
}

/* �������²��������� */
void key_down(void *args)  
{
				oled.pagenum ++;				
}


int key_thread_init(void)
{
   rt_thread_t key_tid;

    key_tid = rt_thread_create("key",
                    key_thread_entry,
                    RT_NULL,
                    512,
                    25,
                    10);

    if (key_tid != RT_NULL){			
				rt_pin_mode(KEY_PIN, PIN_MODE_INPUT_PULLUP);    //���ܰ���  ��������
				rt_pin_mode(BOMA1_PIN, PIN_MODE_INPUT_PULLUP);  //���뿪��  ��������
				rt_pin_mode(BOMA2_PIN, PIN_MODE_INPUT_PULLUP);  
				rt_pin_mode(BOMA3_PIN, PIN_MODE_INPUT_PULLUP);  
	
				rt_pin_mode(WIFI_RELOAD_PIN, PIN_MODE_OUTPUT);  //WIFI ����IO���
				rt_pin_write(WIFI_RELOAD_PIN ,PIN_HIGH);
			
				rt_pin_attach_irq(KEY_PIN, PIN_IRQ_MODE_FALLING, key_down, RT_NULL);/* ���жϣ�������ģʽ���ص�������Ϊkey_down */
				rt_pin_irq_enable(KEY_PIN, PIN_IRQ_ENABLE);/* ʹ���ж� */
			
				log_i("KEY_Init()");
				boma_value = boma_value_get();	//��ʼ���õ���ǰ����״̬ --> VehicleStatus
				//log_i("Current: BOMA_Value = %d", boma_value);
	
				rt_thread_startup(key_tid);
				//rt_event_send(&init_event, KEY_EVENT);
		}
		return 0;
}
INIT_APP_EXPORT(key_thread_init);


/*��WIFI������ */
int wifi_reload(void)
{
		rt_pin_write(WIFI_RELOAD_PIN ,PIN_LOW);
		rt_thread_mdelay(4000);
		rt_pin_write(WIFI_RELOAD_PIN ,PIN_HIGH);
		rt_thread_mdelay(1000);
		log_d("WIFI Reload!\r\n");
		return 0;

}
MSH_CMD_EXPORT(wifi_reload,wifi reload);

