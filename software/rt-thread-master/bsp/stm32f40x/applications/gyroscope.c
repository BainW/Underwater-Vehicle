#include "init.h"
#include <string.h>
#include "board.h"
#include <rthw.h>

/*----------------------- Variable Declarations -----------------------------*/
		short res[10] = {0};
struct STime		stcTime;
struct SAcc 		stcAcc;
struct SGyro 		stcGyro;
struct SAngle 	stcAngle;
struct SMag 		stcMag;
struct SDStatus stcDStatus;
struct SPress 	stcPress;
struct SLonLat 	stcLonLat;
struct SGPSV 		stcGPSV;
struct SQ       stcQ;

struct JY901_t JY901; //JY901��ʵֵ�ṹ��

/*----------------------- Function Implement --------------------------------*/

//CopeSerialDataΪ����2�жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void CopeSerial2Data(unsigned char Data)
{
		static unsigned char RxBuffer[250];
		static unsigned char RxCnt = 0;	

		RxBuffer[RxCnt++]=Data;	//���յ������ݴ��뻺������
		if (RxBuffer[0]!=0x55){ //����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
				RxCnt=0;
				return;
		}
		if (RxCnt<11) {return;}//���ݲ���11�����򷵻�
		else{
				switch(RxBuffer[1]){//�ж��������������ݣ�Ȼ���俽������Ӧ�Ľṹ���У���Щ���ݰ���Ҫͨ����λ���򿪶�Ӧ������󣬲��ܽ��յ�������ݰ�������
						case 0x50:	memcpy(&stcTime,&RxBuffer[2],8);break;//memcpyΪ�������Դ����ڴ濽��������������"string.h"�������ջ��������ַ����������ݽṹ�����棬�Ӷ�ʵ�����ݵĽ�����
						case 0x51:	memcpy(&stcAcc,&RxBuffer[2],8);break;
						case 0x52:	memcpy(&stcGyro,&RxBuffer[2],8);break;
						case 0x53:	memcpy(&stcAngle,&RxBuffer[2],8);break;
						case 0x54:	memcpy(&stcMag,&RxBuffer[2],8);break;
						case 0x55:	memcpy(&stcDStatus,&RxBuffer[2],8);break;
						case 0x56:	memcpy(&stcPress,&RxBuffer[2],8);break;
						case 0x57:	memcpy(&stcLonLat,&RxBuffer[2],8);break;
						case 0x58:	memcpy(&stcGPSV,&RxBuffer[2],8);break;
						case 0x59:	memcpy(&stcQ,&RxBuffer[2],8);break;
				}
				RxCnt=0;//��ջ�����
		}

}
static void JY901_Convert(void* parameter)// ��ʱ��1��ʱ����  ����JY901ģ������ת��
{
		static u8 i = 0;


		for(i = 0;i < 3;i++){	
				JY901.Acc[i] = (float)stcAcc.a[i]/32768*16;
				JY901.Gyro[i] = (float)stcGyro.w[i]/32768*2000;
				JY901.Angle[i] = (float)stcAngle.angle[i]/32768*180;
				JY901.Mag[i] 		= stcMag.h[i];
		}
		JY901.Temperature = (float)stcAcc.T/100;

}


int timer_init(void)
{
		/* ��ʱ���Ŀ��ƿ� */
		static rt_timer_t timer1;
    /* ������ʱ��1 */
    timer1 = rt_timer_create("timer1",  /* ��ʱ�������� timer1 */
                        JY901_Convert, 	/* ��ʱʱ�ص��Ĵ����� */
                        RT_NULL, 			  /* ��ʱ��������ڲ��� */
                        5,      			  /* ��ʱ���ȣ���OS TickΪ��λ����5��OS Tick   --> 50MS*/  
                        RT_TIMER_FLAG_PERIODIC); /* �����Զ�ʱ�� */
    /* ������ʱ�� */
    if (timer1 != RT_NULL) rt_timer_start(timer1);

    return 0;
}
INIT_APP_EXPORT(timer_init);





/* Getʱ��  time */
void get_time(void)
{
		//���ݴ����string��       ��ΪRT-Thread rt_kprintf()�����޷���������ͣ�����ֽ����ݴ����String�ͷ���
		char str[100];
		sprintf(str,"Time:20%d-%d-%d %d:%d:%.3f\r\n",stcTime.ucYear,stcTime.ucMonth,stcTime.ucDay,stcTime.ucHour,stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);
		rt_kprintf(str);
}
MSH_CMD_EXPORT(get_time,get acceleration[a]);

/* Get���ٶ�  acceleration */
void *get_acc(void)
{		
		char str[100];
		sprintf(str,"Acc:%.3f %.3f %.3f\r\n",JY901.Acc[0],JY901.Acc[1],JY901.Acc[2]);
		rt_kprintf(str);
		return JY901.Acc;
}
MSH_CMD_EXPORT(get_acc,get acceleration[a]);

/* Get ������ٶ�  gyroscope*/
void *get_gyro(void)
{
		char str[100];
		sprintf(str,"Gyro:%.3f %.3f %.3f\r\n",JY901.Gyro[0],JY901.Gyro[1],JY901.Gyro[2]);
		rt_kprintf(str);	
		return JY901.Gyro;
}
MSH_CMD_EXPORT(get_gyro, get gyroscope[w]);

/* Get �Ƕ�  angle */
void *get_angle(void)
{ 
		char str[100];
		sprintf(str,"Angle:%.3f %.3f %.3f\r\n",JY901.Angle[0],JY901.Angle[1],JY901.Angle[2]);
		rt_kprintf(str);		
		return JY901.Angle;
}
MSH_CMD_EXPORT(get_angle, get angle[o]);


/* Get �ų�  magnetic */
void *get_mag(void)
{
		char str[100];
		sprintf(str,"Mag:%d %d %d\r\n",JY901.Mag[0],JY901.Mag[1],JY901.Mag[2]);
		rt_kprintf(str);	
		return JY901.Mag;
}
MSH_CMD_EXPORT(get_mag, get magnetic[B]);

/* Get �¶�  Temperature */
float get_temperature(void)
{
		char str[100];
		sprintf(str,"Temperature:%.2f C\r\n",JY901.Temperature);
		rt_kprintf(str);	
		return JY901.Temperature;
}
MSH_CMD_EXPORT(get_temperature, get Temperature[T]);








