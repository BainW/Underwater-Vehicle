#ifndef __LED_H
#define __LED_H

#include "DataType.h"

#define LEDR PEout(15)
#define LEDG PBout(10)
#define LEDB PBout(11)

//RGB�ƹ������ӵ�+3.3V����ƽ 0�� 1��
#define LED_ON(led_pin) 						rt_pin_write(led_pin ,PIN_LOW )
#define LED_OFF(led_pin) 						rt_pin_write(led_pin ,PIN_HIGH)
#define LED_Turn(led_pin,status) 		rt_pin_write(led_pin ,status =! status) //ȡ��





typedef struct
{
  uint32 Bling_Contiune_Time;//��˸����ʱ��
  uint32 Bling_Period;//��˸����
  float  Bling_Percent;//��˸ռ�ձ�
  uint32  Bling_Cnt;//��˸������
  uint32 Port; //�˿�
  uint32 Pin;//����
  uint8 Endless_Flag;//�޾�ģʽ
}Bling_Light;

extern Bling_Light Light_1,Light_2,Light_3;



void led_blink_task(void);
void system_led_blink(uint8 InputData);

void Bling_Set(Bling_Light *Light,
               uint32 Continue_time,//����ʱ��
               uint32 Period,//����100ms~1000ms
               float Percent,//0~100%
               uint32  Cnt,
               uint32 Port,
               uint32 Pin,
							 uint8 Flag);
							 
							 
							 
void Bling_Process(Bling_Light *Light);
void Bling_Working(uint8 bling_mode);
							 
							 
void Bling_Init(void);
void Quad_Start_Bling(void);
extern Bling_Light Light_1,Light_2,Light_3,Light_4;
extern uint8 Bling_Mode;
							 
							 
#endif


