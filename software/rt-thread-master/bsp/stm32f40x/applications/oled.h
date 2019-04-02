#ifndef __OLED_H
#define __OLED_H

#include <drivers/pin.h>
#include "init.h"




void Boot_Animation(void);//��������
void menu_define(void); //�˵�����
void OLED_StatusPage(void);
void OLED_PicturePage(void);
void OLED_GyroscopePage(void);
void draw_circle(u8 x,u8 y,u8 r); //Բ��(x,y),�뾶r
void draw_line(u8 x0,u8 y0,float k,u8 dot); //���̶���(x0,y0),б��k  dot:0,���;1,���	  

void draw_fill_circle(u8 x0,u8 y0,u8 r,u8 dot);//д��ʵ��Բ��(x0,y0),�뾶r



typedef struct {
		u8 pagenum;  				//ҳ��
		u8 pagechange;
	  u8 pagechange_flag;
		char *pagename[20]; //ҳ��
}OledType;


typedef enum
{
		StatusPage = 1, //ҳ��ӵ�һҳ��ʼ
		GyroscopePage,
		FlashPage,   
		PicturePage,
	
		OLED_Page_MAX   //ҳ�����ֵ
	
}
MENU_LIST_e; //�˵�ö��

extern char *VehicleModeName[2];


#endif


