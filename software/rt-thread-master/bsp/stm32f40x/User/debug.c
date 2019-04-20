/*
 * debug.c
 *
 *  Created on: 2019��2��5��
 *      Author: zengwangfa
 *      Notes:  ������������λ��
 */
#define LOG_TAG    "debug"

#include <rtthread.h>
#include <string.h>
#include <elog.h>
#include "PID.h"
#include "debug.h"
#include "drv_ano.h"
#include "filter.h"
#include "drv_cpu_temp.h"
#include "gyroscope.h"
#include "drv_MS5837.h"
#include "PID.h"
/*---------------------- Constant / Macro Definitions -----------------------*/		



/*----------------------- Variable Declarations. -----------------------------*/

extern rt_device_t debug_uart_device;	
extern uint8 debug_startup_flag;
extern float  volatge;
extern float  Yaw;

enum 
{
		DEBUG_NULL,
		PC_VCAN,
		PC_ANO,
		//������
		
}PC_TOOL;//��λ������

char *debug_tool_name[3]={"NULL","VCAN","ANO"};

uint8 debug_tool = PC_ANO; //ɽ�� / ������λ�� ���Ա�־λ
volatile uint32 debug_count = 0;

/*-----------------------Debug Thread Begin-----------------------------*/

void debug_send_thread_entry(void* parameter)
{
		while(1)
		{
				rt_thread_mdelay(1);
				if(1 == debug_startup_flag)//��debug_uart��ʼ����Ϻ� �Ž�����λ��ͨ��
				{
						switch(debug_tool)//ѡ����λ��
						{
								case PC_VCAN: Vcan_Send_Data();break;
								case PC_ANO :	ANO_SEND_StateMachine();break;
								default :break;
						}
				}		
		}
}



int Debug_thread_init(void)
{
	  rt_thread_t debug_send_tid;
		/*������̬�߳�*/
    debug_send_tid = rt_thread_create("debug",			 //�߳�����
                    debug_send_thread_entry,									 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    1);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (debug_send_tid != RT_NULL){
				rt_thread_startup(debug_send_tid);
		}
		return 0;
}
INIT_APP_EXPORT(Debug_thread_init);

/*-----------------------Debug Thread End-----------------------------*/




/* VCANɽ����λ������ BEGIN */
void Vcan_Send_Cmd(void *wareaddr, unsigned int waresize)
{
		#define CMD_WARE     3
    static uint8 cmdf[2] = {CMD_WARE, ~CMD_WARE};    //���ڵ��� ʹ�õ�ǰ����
    static uint8 cmdr[2] = {~CMD_WARE, CMD_WARE};    //���ڵ��� ʹ�õĺ�����

    rt_device_write(debug_uart_device, 0,cmdf, 2);    //�ȷ���ǰ����
    rt_device_write(debug_uart_device, 0,(uint8 *)wareaddr, waresize);    //��������
    rt_device_write(debug_uart_device, 0,cmdr, 2);    //���ͺ�����
}


void Vcan_Send_Data(void)
{   
		//float temp = 0.0f;
	
		static float list[8]= {0};
		//temp = get_cpu_temp();
	
		list[0] = Total_Controller.Yaw_Angle_Control.Err; 	//����� Roll 
		list[1] = Total_Controller.Yaw_Angle_Control.Control_OutPut;  //������ Pitch
		list[2] = Sensor.JY901.Euler.Yaw; 	  //ƫ���� Yaw
		list[3] = Yaw;    //CPU�¶� temp
		list[4] = 0;//KalmanFilter(&temp);//�������˲�����¶�
		list[5] = 0;//MS_TEMP;//get_vol();
		list[6] = MS5837_Temperature;//MS5837_Pressure;	//KalmanFilter(&vol)
		list[7] = MS5837_Pressure;	//camera_center;
		
		Vcan_Send_Cmd(list,sizeof(list));
}
/* VCANɽ����λ������  END */




/* debug �������߹ر� */
static int debug(int argc, char **argv)
{
    int result = 0;

    if (argc != 2){
				log_e("Proper Usage: debug on /off");//�÷�:������λ������
				result = -RT_ERROR; 
				goto _exit;  
    }
		if( !strcmp(argv[1],"on") ){ //����Ϊ ɽ����λ�� strcmp ����������� ����0
				debug_startup_flag = 1;
				log_v("debug open\r\n");
		}
		else if( !strcmp(argv[1],"off") ){ //����Ϊ ɽ����λ�� strcmp ����������� ����0
				debug_startup_flag = 0;
				log_v("debug off\r\n");
		}
		else {
				log_e("Proper Usage: debug on /off");//�÷�:������λ������
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(debug,ag: debug on);



/* debug ������λ������ */
static int set_debug_tool(int argc,char **argv)
{
		int result = 0;
    if (argc != 2){
				log_e("Proper Usage: debug_by vcan / ano / null");//�÷�:������λ������
				result = -RT_ERROR;
        goto _exit;
    }

		if( !strcmp(argv[1],"vcan") ){ //����Ϊ ɽ����λ�� strcmp ����������� ����0
				debug_tool = PC_VCAN;
				log_v("Debug Tool:VCAN\r\n");
		}

		else if( !strcmp(argv[1],"ano") ){ //����Ϊ ������λ��
				debug_tool = PC_ANO;
				log_v("Debug Tool:ANO\r\n");
		}
		else if( !strcmp(argv[1],"null") ){ //����Ϊ ɽ����λ��
				debug_tool = DEBUG_NULL;
				log_v("Debug Tool:DEBUG_NULL\r\n");
		}
		else {
				log_e("Proper Usage: debug_by vcan / ano / null");//�÷�:������λ������
				goto _exit;
		}
		Flash_Update();
_exit:
    return result;
}
MSH_CMD_EXPORT(set_debug_tool,debug_by vcan / ano / null);

