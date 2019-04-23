/*
 * led.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  LEDָʾ
 */
#define LOG_TAG    "led"

#include "led.h"
#include "key.h"
#include "flash.h"
#include "drv_ano.h"
#include <string.h>
#include <elog.h>
#include <drivers/pin.h>

/*---------------------- Constant / Macro Definitions -----------------------*/
//RGB�����ź�
#define LED_Red 		77 // PD8
#define LED_Green 	78 // PD9
#define LED_Blue 		79 // PD10

//OV Camera �����
#define LED_Camera 	141  // PE0 �ߵ�ƽ����

/*----------------------- Variable Declarations -----------------------------*/
/* ALL_init �¼����ƿ�. */
extern struct rt_event init_event;
extern uint8 VehicleStatus;

Bling_Light Light_1,Light_2,Light_3;

uint8 Bling_Mode = 0;
/*----------------------- Function Implement --------------------------------*/
void led_thread_entry(void *parameter)
{	

	  uint8 i=0;/*��ɫ���ı�> ��   ��   ��   ��   ��   ��   ��   �� */
		uint8 inputdata[8] = {0x00,0x04,0x02,0x01,0x03,0x05,0x06,0x07};
		
		while(i <= 7){
				system_led_blink(inputdata[i++]);}
		
		LED_OFF(LED_Red);			//��ʼ��Ϊ�ߵ�ƽ ��Ϩ��
		LED_OFF(LED_Green);			
		LED_OFF(LED_Blue);
				

    while (1)
    {			
				/* FLASH���� ���� ��λPID���� */
				Save_Or_Reset_PID_Parameter();  
				Bling_Working(Bling_Mode);
				led_blink_task();
				rt_thread_mdelay(10); //10ms
			


    }
}

/* led��˸����ϵͳ��������ָʾ�ơ� */
void led_blink_task(void)
{
		static rt_uint8_t status = 1;
	  static rt_uint8_t cut = 0;
		cut++;
		if(boma_value_get() == System_NORMAL_STATUS && cut >= 40 ){
				cut = 0;
				LED_Turn(LED_Green,status);	//��ʼ��Ϊ�ߵ�ƽ ��Ϩ��
		}
}
	

/* ϵͳ��ʼ��led��˸״̬����ʾ7����ɫ�� -->[��ɫ���ı�> ��  ��  ��  ��  ��  ��  ��  ��] */
void system_led_blink(uint8 InputData)
{
    if(InputData & 0x04){	
						LED_ON(LED_Red); }
		else{ 	LED_OFF(LED_Red);}
		
	  if(InputData & 0x02){	
					LED_ON(LED_Green); }
		else{ LED_OFF(LED_Green);}
		
		if(InputData & 0x01){	
					 LED_ON(LED_Blue); }
		else{  LED_OFF(LED_Blue);}
		rt_thread_mdelay(300);//�ȴ�ϵͳ��ʼ��  ��ϣ�ϵͳ�ȶ����ڶ�ȡ����
}


int led_thread_init(void)
{
    rt_thread_t led_tid;
		/*������̬�߳�*/
    led_tid = rt_thread_create("led",//�߳�����
                    led_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    512,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    25,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (led_tid != RT_NULL){
				rt_pin_mode(LED_Red, 	PIN_MODE_OUTPUT);//�������ģʽ	
				rt_pin_mode(LED_Green, PIN_MODE_OUTPUT);	
				rt_pin_mode(LED_Blue, 	PIN_MODE_OUTPUT);	
			

				log_i("LED_Init()");
				//rt_event_send(&init_event, LED_EVENT);
				rt_thread_startup(led_tid);
		}
		return 0;
}
INIT_APP_EXPORT(led_thread_init);



/***************************************************
������: void Bling_Set(Bling_Light *Light,
uint32_t Continue_time,//����ʱ��
uint16_t Period,//����100ms~1000ms
float Percent,//0~100%
uint16_t  Cnt,
GPIO_TypeDef* Port,
uint16_t Pin
,uint8_t Flag)
˵��:	״ָ̬ʾ�����ú���
���:	ʱ�䡢���ڡ�ռ�ձȡ��˿ڵ�
����:	��
��ע:	�����ʼ����ʼ������
****************************************************/
void Bling_Set(Bling_Light *Light,
               uint32 Continue_time,//����ʱ��
               uint32 Period,//����100ms~1000ms
               float Percent,//0~100%
               uint32  Cnt,
               uint32 Port,
               uint32 Pin,
               uint8 Flag)
{
		Light->Bling_Contiune_Time=(Continue_time/5);//����ʱ��
		Light->Bling_Period=Period;//����
		Light->Bling_Percent=Percent;//ռ�ձ�
		Light->Port=Port;//�˿�
		Light->Pin=Pin;//����
		Light->Endless_Flag=Flag;//�޾�ģʽ
}

/***************************************************
������: void Bling_Process(Bling_Light *Light)//��˸�����߳�
˵��:	״ָ̬ʾ��ʵ��
���:	״̬�ƽṹ��     
����:	��
��ע:	�����ʼ����ʼ������
****************************************************/
void Bling_Process(Bling_Light *Light)//��˸�����߳�
{
  if(Light->Bling_Contiune_Time>=1) {
			Light->Bling_Contiune_Time--;
	}
  else {rt_pin_write(Light->Pin ,0);}//��
  if(Light->Bling_Contiune_Time != 0//��ʱ��δ��0
			||Light->Endless_Flag==1)//�ж��޾�ģʽ�Ƿ���
  {
			Light->Bling_Cnt++;
			if(5*Light->Bling_Cnt>=Light->Bling_Period){
					Light->Bling_Cnt=0;//��������
			}
			if(5*Light->Bling_Cnt <= Light->Bling_Period * Light->Bling_Percent){	
					rt_pin_write(Light->Pin ,0);   //��
			}
			else {rt_pin_write(Light->Pin ,1);}//��
  }
	else {	
			rt_pin_write(Light->Pin ,1);		//�ߵ�ƽ ��Ϩ��		
	}
}



/***************************************************
������: Bling_Working(uint16 bling_mode)
˵��:	״ָ̬ʾ��״̬��
���:	��ǰģʽ
����:	��
��ע:	�����ʼ����ʼ������
****************************************************/
void Bling_Working(uint8 bling_mode)
{
		if(0 == bling_mode)
		{
				Bling_Process(&Light_1);
				Bling_Process(&Light_2);
				Bling_Process(&Light_3);
		}
		else if(1 == bling_mode)//���ٶȼ�6��У׼ģʽ
		{
				Bling_Process(&Light_1);

		}
		else if(2 == bling_mode)//������У׼ģʽ
		{
				Bling_Process(&Light_2);
		}
		else if(3 == bling_mode)//ȫ��
		{
				Bling_Process(&Light_3);
		}
		 
}



/* led on MSH���� */
static int led_on(int argc, char **argv)
{
    int result = 0;

    if (argc != 2){
        log_e("Error! Proper Usage: led_on r\n Species:r / g / b / c");
				result = -RT_ERROR;
				return result;
    }
		
		switch(*argv[1]){
				case 'r':LED_ON(LED_Red);break;
				case 'g':LED_ON(LED_Green);break;
				case 'b':LED_ON(LED_Blue);break;
				case 'c':LED_OFF(LED_Camera);break;
				default:log_e("Error! Proper Usage: led_on R\n Species:r / g / b / c");break;
		}

		return result;
}
MSH_CMD_EXPORT(led_on,ag: led_on r  );


/* led off MSH���� */
static int led_off(int argc, char **argv)
{
    int result = 0;

    if (argc != 2){
        log_e("Error! Proper Usage: led_off r\n Species:r /g /b /c");
				result = -RT_ERROR;
        goto _exit;
    }
		
		switch(*argv[1]){
			
				case 'r':LED_OFF(LED_Red);break;
				case 'g':LED_OFF(LED_Green);break;
				case 'b':LED_OFF(LED_Blue);break;
				case 'c':LED_ON(LED_Camera);break;
				default:log_e("Error! Proper Usage: led_off r\n Species:r /g /b /c");break;
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(led_off,ag:led_off r);




