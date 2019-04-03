#ifndef __PWM_H_
#define __PWM_H_

#include "DataType.h"



typedef struct  /*  */
{
		uint32 OpenValue;		//��е�� �򿪵�PWMֵ 
		uint32 CloseValue;	 //��е�� �رյ�PWMֵ
		uint32 CurrentValue; //��е�۵�ǰֵ
}ServoType;

void TIM1_PWM_Init(uint32 arr,uint32 psc);
void TIM4_PWM_Init(uint32 arr,uint32 psc);


#endif

