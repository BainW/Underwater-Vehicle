#ifndef __PROPELLERCONTROL_H
#define __PROPELLERCONTROL_H

#include "DataType.h"
#include "propeller.h"
#include "rc_data.h"

void Propeller_Control(void);
void Propeller_Output(void);
void robot_upDown(float depth); 
void Horizontal_Propeller_Power_Clear(void);//ˮƽ������������
void Propller_stop(void);  //�ƽ���ͣת

void robotForward(void);  //ǰ��
void robotBackAway(void); //����

void turnRight(uint16 power); //��ת
void turnLeft(uint16 power);  //��ת

extern float Expect_Depth;
#endif



