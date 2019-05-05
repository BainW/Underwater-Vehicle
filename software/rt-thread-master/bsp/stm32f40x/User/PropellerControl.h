#ifndef __PROPELLERCONTROL_H
#define __PROPELLERCONTROL_H

#include "DataType.h"


void Propeller_Control(void);
void Propeller_Output(void);
void Propeller_upDown(int depth);

void robotForward(void);  //ǰ��
void robotBackAway(void); //����

void turnRight(void);  //��ת
void turnLeft(void);  //��ת

void moveLeft(void);  //����
void moveRight(void);  //����
#endif



