#define LOG_TAG    "timer"


#include <string.h>

#include "timer.h"
#include "flash.h"
#include <rtthread.h>
#include <elog.h>
#include "drv_MS5837.h"
#include  "Control.h"
/*---------------------- Constant / Macro Definitions -----------------------*/



/*----------------------- Variable Declarations -----------------------------*/



uint8 ov_frame = 0;
uint8 ov_frame_flag = 0;

/*----------------------- Function Implement --------------------------------*/

static void timer1_out(void* parameter)// ��ʱ��1��ʱ����  ����JY901ģ������ת��
{
		static int count = 0;

		count ++;

	
	
		Angle_Control();
		if(20 == count){	//1s
				
				if(1 == ov_frame_flag)
				{
						rt_kprintf("OV Frame:%d\n",ov_frame);
						ov_frame_flag = 0; //����
				}
				count = 0;
				ov_frame = 0;
		}
}



int timer1_init(void)
{
		/* ��ʱ���Ŀ��ƿ� */
		static rt_timer_t timer1;
    /* ������ʱ��1 */
    timer1 = rt_timer_create("timer1",  /* ��ʱ�������� timer1 */
                        timer1_out, 		  /* ��ʱʱ�ص��Ĵ����� */
                        RT_NULL, 			  /* ��ʱ��������ڲ��� */
                        5,      			  /* ��ʱ���ȣ���OS TickΪ��λ����5��OS Tick   --> 50MS*/  
                        RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_HARD_TIMER); /* �����Զ�ʱ�� */
    /* ������ʱ�� */
    if (timer1 != RT_NULL){ 
				
				rt_timer_start(timer1);
				
		}

    return 0;
}
INIT_APP_EXPORT(timer1_init);


/* Get Camera ֡�� */
void get_ov_frame(void)
{
		ov_frame_flag = 1;//���� ��ӡ֡��
}
MSH_CMD_EXPORT(get_ov_frame, get ov frame [fps]);






