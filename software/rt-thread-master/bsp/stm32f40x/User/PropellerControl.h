#ifndef __PROPELLERCONTROL_H
#define __PROPELLERCONTROL_H

#include "DataType.h"
#include "propeller.h"

void Propeller_Control(void);
void Propeller_Output(void);
void robot_upDown(float depth); 

void robotForward(void);  //ǰ��
void robotBackAway(void); //����

void turnRight(void);  //��ת
void turnLeft(void);  //��ת

void moveLeft(void);  //����
void moveRight(void);  //����

extern PropellerPower_Type  PropellerPower; //�ƽ������������
extern int16 Expect_Depth;
#endif



