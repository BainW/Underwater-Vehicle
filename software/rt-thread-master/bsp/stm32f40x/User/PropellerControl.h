#ifndef __PROPELLERCONTROL_H
#define __PROPELLERCONTROL_H

#include "DataType.h"
#include "propeller.h"

void Propeller_Control(void);
void Propeller_Output(void);
void robot_upDown(float depth); 
void Horizontal_Propeller_Power_Clear(void);//ˮƽ������������


void robotForward(void);  //ǰ��
void robotBackAway(void); //����

void turnRight(uint16 power); //��ת
void turnLeft(uint16 power);  //��ת



extern int32 Expect_Depth;
#endif



