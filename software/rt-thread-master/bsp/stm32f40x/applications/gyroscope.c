#include "init.h"
#include <string.h>

/*----------------------- Variable Declarations -----------------------------*/
extern  int sprintf(char *buf, const char *format, ...);

char str[100];

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

/*----------------------- Function Implement --------------------------------*/

//CopeSerialDataΪ����2�жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void CopeSerial2Data(unsigned char Data)
{
		static unsigned char RxBuffer[250];
		static unsigned char RxCnt = 0;	

	
		RxBuffer[RxCnt++]=Data;	//���յ������ݴ��뻺������
		if (RxBuffer[0]!=0x55) //����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
		{
				RxCnt=0;
				return;
		}
		if (RxCnt<11) {return;}//���ݲ���11�����򷵻�
		else
		{
				switch(RxBuffer[1])//�ж��������������ݣ�Ȼ���俽������Ӧ�Ľṹ���У���Щ���ݰ���Ҫͨ����λ���򿪶�Ӧ������󣬲��ܽ��յ�������ݰ�������
				{
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

/* Getʱ��  time */
void get_time(void)
{
		//���ݴ����string��
		sprintf(str,"Time:20%d-%d-%d %d:%d:%.3f\r\n",stcTime.ucYear,stcTime.ucMonth,stcTime.ucDay,stcTime.ucHour,stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);
		rt_kprintf(str);
}
MSH_CMD_EXPORT(get_time,get acceleration[a]);

/* Get���ٶ�  acceleration */
void *get_acc(void)
{
		sprintf(str,"Acc:%.3f %.3f %.3f\r\n",(float)stcAcc.a[0]/32768*16,(float)stcAcc.a[1]/32768*16,(float)stcAcc.a[2]/32768*16);
		rt_kprintf(str);
		return stcAcc.a;
}
MSH_CMD_EXPORT(get_acc,get acceleration[a]);



/* Get ������ٶ�  gyroscope*/
void *get_gyro(void)
{
		sprintf(str,"Gyro:%.3f %.3f %.3f\r\n",(float)stcGyro.w[0]/32768*2000,(float)stcGyro.w[1]/32768*2000,(float)stcGyro.w[2]/32768*2000);
		rt_kprintf(str);	
		return stcGyro.w;
}
MSH_CMD_EXPORT(get_gyro, get gyroscope[w]);

/* Get �Ƕ�  angle */
void *get_angle(void)
{ 
		sprintf(str,"Angle:%.3f %.3f %.3f\r\n",(float)stcAngle.Angle[0]/32768*180,(float)stcAngle.Angle[1]/32768*180,(float)stcAngle.Angle[2]/32768*180);
		rt_kprintf(str);		
		return stcAngle.Angle;
}
MSH_CMD_EXPORT(get_angle, get angle[o]);


/* Get �ų�  magnetic */
void *get_mag(void)
{
		sprintf(str,"Mag:%d %d %d\r\n",stcMag.h[0],stcMag.h[1],stcMag.h[2]);
		rt_kprintf(str);	
		return stcMag.h;
}
MSH_CMD_EXPORT(get_mag, get magnetic[B]);

/* Get �¶�  Temperature */
float get_temp(void)
{
		sprintf(str,"Temperature:%.2f C\r\n",(float)stcAcc.T/0x100);
		rt_kprintf(str);	
		return (float)stcAcc.T/0x100;
}
MSH_CMD_EXPORT(get_temp, get magnetic[T]);








