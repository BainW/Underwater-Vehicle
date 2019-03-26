#ifndef __PWM_H_
#define __PWM_H_

#include "init.h"



typedef struct  /*  */
{
		u32 OpenValue;		//��е�� �򿪵�PWMֵ 
		u32 CloseValue;	 //��е�� �رյ�PWMֵ
		u32 CurrentValue; //��е�۵�ǰֵ
}ServoType;

void TIM1_PWM_Init(u32 arr,u32 psc);
void TIM4_PWM_Init(u32 arr,u32 psc);


#endif

