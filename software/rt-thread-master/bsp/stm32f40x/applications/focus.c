#include "focus.h"
#include <rtthread.h>
#include <elog.h>
#include "uart.h"



/*---------------------- Constant / Macro Definitions -----------------------*/


/*----------------------- Variable Declarations -----------------------------*/

uint8 addFocus_Data[6] = {0xAA,0x55,0x02,0x01,0x00,0x00}; //�Ŵ�
uint8 minFocus_Data[6] = {0xAA,0x55,0x02,0x02,0x00,0x01}; //��С

uint8 addZoom_Data[6]  = {0xAA,0x55,0x02,0x00,0x01,0x00}; //�۽�
uint8 minZoom_Data[6]  = {0xAA,0x55,0x02,0x00,0x02,0x01}; //�Ž�

/*----------------------- Function Implement --------------------------------*/

/**
  * @brief  Focus_Camera(����ͷ�۽�)
  * @param  �����ַ����� [0x01�Ŵ�0x02��С]
  * @retval None
  * @notice 
  */
void Focus_Camera(uint8 focus)
{
		if(0x01 == focus){ //�Ŵ�
				rt_device_write(focus_uart_device, 0,addFocus_Data, 6);    //���ͺ�����
		}
		else if(0x02 == focus ){ //��С
				rt_device_write(focus_uart_device, 0,minFocus_Data, 6);    //���ͺ�����
		}
}

/**
  * @brief  Zoom_Camera(����ͷ�佹)
  * @param  �����ַ����� [0x01�۽���0x02�Ž�]
  * @retval None
  * @notice 
  */
void Zoom_Camera(uint8 zoom)
{
	
		if(0x01 == zoom ){ //�۽�
				rt_device_write(focus_uart_device, 0,addZoom_Data, 6);    //���ͺ�����
				
		}
		else if(0x02 == zoom){ //�Ž�
				rt_device_write(focus_uart_device, 0,minZoom_Data, 6);    //���ͺ�����
		}
	
}


/* ���� �Ŵ� */
void focus_camera_add(void)
{
		Focus_Camera(1);
		log_i("focus_camera_add ");
}
MSH_CMD_EXPORT(focus_camera_add,focus camera add);












