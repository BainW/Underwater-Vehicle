#ifndef __PWM_H_
#define __PWM_H_

#include "init.h"



typedef struct  /*  */
{
		u32 open_value;		//��е�� �򿪵�PWMֵ 
		u32 close_value;	 //��е�� �رյ�PWMֵ
}ServoType;

void TIM1_PWM_Init(u32 arr,u32 psc);



#endif

