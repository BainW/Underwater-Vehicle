#ifndef __OLED_H
#define __OLED_H

#include <drivers/pin.h>
#include "init.h"

void Boot_Animation(void);//��������
void menu_define(void); //�˵�����
void OLED_StatusPage(void);

void draw_circle(u8 x,u8 y,u8 r); //Բ��(x,y),�뾶r
void draw_line(u8 x0,u8 y0,float k); //���̶���(x0,y0),б��k








#endif


