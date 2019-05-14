#include "stm8s_gpio.h"
#include "key.h"
#include "bsp_timer.h"


//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //HSI = 16M (1��Ƶ)  //ʱ������
  GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)GPIO_PIN_3,GPIO_MODE_IN_PU_NO_IT);  //KEY0�������ж�����
  GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_4,GPIO_MODE_IN_PU_NO_IT);//KEY1��KEY2
  GPIO_Init(GPIOB, (GPIO_Pin_TypeDef)GPIO_PIN_5,GPIO_MODE_IN_PU_NO_IT); 
}


char KEY_Scan(char mode)
{	 
	static char key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY0==0||KEY1==0||KEY2==0))
	{
		TIMDelay_Nms(10);//ȥ���� 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
	}else if(KEY0==1&&KEY1==1&&KEY2_PRES==0)key_up=1; 	    
 	return 0;// �ް�������
}
