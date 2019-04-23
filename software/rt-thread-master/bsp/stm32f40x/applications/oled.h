#ifndef __OLED_H_
#define __OLED_H_

#include "DataType.h"

void Boot_Animation(void);//��������
void menu_define(void); //�˵�����
void OLED_StatusPage(void);
void OLED_PicturePage(void);
void OLED_FlashPage(void);
void OLED_GyroscopePage(void);
void draw_circle(uint8 x,uint8 y,uint8 r); //Բ��(x,y),�뾶r
void draw_line(uint8 x0,uint8 y0,float k,uint8 dot); //���̶���(x0,y0),б��k  dot:0,���;   1,���	  

void draw_fill_circle(uint8 x0,uint8 y0,uint8 r,uint8 dot);//д��ʵ��Բ��(x0,y0),�뾶r



typedef struct {
		uint8 pagenum;  				//ҳ��
		uint8 pagechange;
	  uint8 pagechange_flag;
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


