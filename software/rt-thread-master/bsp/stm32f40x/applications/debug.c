#include "init.h"

/*---------------------- Constant / Macro Definitions -----------------------*/		

#define VCAN_UART "uart3"


/*----------------------- Variable Declarations. -----------------------------*/

extern rt_device_t debug_uart_device;	
extern struct JY901_t JY901; //JY901��ʵֵ�ṹ��
extern struct SAngle 	stcAngle;

void ware_test(void)
{   

	static int list[8]= {0};
	list[0] = (int)JY901.Angle[0]; //������ Pitch
	list[1] = (int)JY901.Angle[1]; //����� Roll
	list[2] = (int)JY901.Angle[2]; //ƫ���� Yaw
	list[3] = (int)(stcAngle.angle[2]);//-(Servo_Duty-Servo_Duty_Md);
	list[4] = 90;//corner_meet_rn;//edge_start[1];//
	list[5] = 0;
	list[6] = 0;//camera_forecast_center;//
	list[7] = 0;//camera_center;
	
             
	vcan_sendware(list,sizeof(list));
}



void vcan_sendware(void *wareaddr, unsigned int waresize)
{
		#define CMD_WARE     3
    u8 cmdf[2] = {CMD_WARE, ~CMD_WARE};    //���ڵ��� ʹ�õ�ǰ����
    u8 cmdr[2] = {~CMD_WARE, CMD_WARE};    //���ڵ��� ʹ�õĺ�����

    rt_device_write(debug_uart_device, 0,cmdf, 2);    //�ȷ���ǰ����
    rt_device_write(debug_uart_device, 0,(u8 *)wareaddr, waresize);    //��������
    rt_device_write(debug_uart_device, 0,cmdr, 2);    //���ͺ�����
}






/*debug ɽ����λ������ MSH���� */
static int debug(int argc, char **argv)
{
    int result = 0;
		short count = 0;
    if (argc != 2){
				rt_kprintf("Usage: debug 10  [notes:debug 10s]\n");
				result = -RT_ERROR; goto _exit;
    }
		count = atoi(argv[1])*1000/5;
		while(count--){
				ware_test();
				rt_thread_mdelay(5);
		}
		
		rt_kprintf("Debug compeleted.");
_exit:
    return result;
}
MSH_CMD_EXPORT(debug,ag: debug 10);



