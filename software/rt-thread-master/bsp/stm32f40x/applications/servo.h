#ifndef __SERVO_H
#define __SERVO_H

#include "pwm.h"

typedef struct  /*  */
{
		uint16 MaxValue;		//��е�� �������ֵ
		uint16 MinValue;	  //��е�� ����
		uint16 MedValue;
		uint16 CurrentValue; //��е�۵�ǰֵ
		uint8  Speed; //��е�۵�ǰֵ
}ServoType;

extern ServoType RoboticArm ;//��е��

extern ServoType YunTai; //��̨

void RoboticArm_Control(uint8 *action);

void YunTai_Control(uint8 *action);





#endif
