#include "focus.h"
#include <rtthread.h>
#include <elog.h>

extern rt_device_t focus_uart_device;	

uint8 addFocus_Data[5] = {0xAA,0x55,0x01,0x00,0x00}; //�Ŵ�
uint8 minFocus_Data[5] = {0xAA,0x55,0x02,0x00,0x01}; //��С

uint8 addZoom_Data[5]  = {0xAA,0x55,0x00,0x01,0x00}; //�۽�
uint8 minZoom_Data[5]  = {0xAA,0x55,0x00,0x02,0x01}; //�Ž�

void Focus_Camera(uint8 focus)
{
		if(0x01 == focus){ //�۽�
				rt_device_write(focus_uart_device, 0,addFocus_Data, 5);    //���ͺ�����
				
		}
		else if(0x02 == focus ){ //�Ž�
				rt_device_write(focus_uart_device, 0,minFocus_Data, 5);    //���ͺ�����
		}
}


void Zoom_Camera(uint8 zoom)
{
	
		if(0x01 == zoom ){ //�۽�
				rt_device_write(focus_uart_device, 0,addZoom_Data, 5);    //���ͺ�����
				
		}
		else if(0x02 == zoom){ //�Ž�
				rt_device_write(focus_uart_device, 0,minZoom_Data, 5);    //���ͺ�����
		}
	
}


/* ���� ����ģ�� ������Ϊ9600 */
void focus_camera_add(void)
{
		Focus_Camera(1);
		log_i("focus_camera_add ");
}
MSH_CMD_EXPORT(focus_camera_add,focus camera add);












