#ifndef __LED_H
#define __LED_H

#include "init.h"

#define LEDR PEout(15)
#define LEDG PBout(10)
#define LEDB PBout(11)

//RGB�ƹ������ӵ�+3.3V����ƽ 0�� 1��
#define LED_ON(led_pin) 						rt_pin_write(led_pin ,PIN_LOW )
#define LED_OFF(led_pin) 						rt_pin_write(led_pin ,PIN_HIGH)
#define LED_Turn(led_pin,status) 		rt_pin_write(led_pin ,status =! status) //ȡ��



void led_blink_task(void);
void system_led_blink(u8 InputData);


typedef struct
{
  u32 Bling_Contiune_Time;//��˸����ʱ��
  u32 Bling_Period;//��˸����
  float  Bling_Percent;//��˸ռ�ձ�
  u32  Bling_Cnt;//��˸������
  u32 Port; //�˿�
  u32 Pin;//����
  u8 Endless_Flag;//�޾�ģʽ
}Bling_Light;


void Bling_Set(Bling_Light *Light,
               u32 Continue_time,//����ʱ��
               u32 Period,//����100ms~1000ms
               float Percent,//0~100%
               u32  Cnt,
               u32 Port,
               u32 Pin,
							 u8 Flag);
void Bling_Process(Bling_Light *Light);
void Bling_Working(u8 bling_mode);
							 
							 
void Bling_Init(void);
void Quad_Start_Bling(void);
extern Bling_Light Light_1,Light_2,Light_3,Light_4;
extern u8 Bling_Mode;
							 
							 
#endif


