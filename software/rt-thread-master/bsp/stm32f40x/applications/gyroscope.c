/*
 * gyroscope.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ����ģ���ȡ��ת������
 */
#define LOG_TAG    "gyro"

#include <string.h>
#include <stdio.h>
#include <rtthread.h>
#include <elog.h>
#include "gyroscope.h"
#include "drv_MS5837.h"

/*---------------------- Constant / Macro Definitions -----------------------*/

#define PACKET_LENGTH      11    //���ݰ�����

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

struct JY901Type JY901 = {0}; //JY901��ʵֵ�ṹ��


/*----------------------- Function Implement --------------------------------*/

//CopeSerialDataΪ����2�жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void CopeSerial2Data(u8 Data)
{
		static u8 RxBuffer[50];  //���ݰ�
		static u8 RxCheck = 0;	  //βУ����
		static u8 RxCount = 0;	    //���ռ���
		static u8 i = 0;	   		  //���ռ���
	
		RxBuffer[RxCount++] = Data;	//���յ������ݴ��뻺������
	
		if (RxBuffer[0]!=0x55){ //����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
				RxCount=0;					  //��ջ�����
				return;
		}
		if (RxCount < PACKET_LENGTH) {return;}//���ݲ���11�����򷵻�
		
		/*********** ֻ�н�����11���ֽ����� �Ż�������³��� ************/
		for(i = 0;i < 10;i++){
				RxCheck += RxBuffer[i]; //У��λ�ۼ�
		}
		
		if(	RxCheck == RxBuffer[PACKET_LENGTH-1]){//�ж����ݰ�У�� �Ƿ���ȷ
	
				switch(RxBuffer[1]){		//�ж��������������ݣ�Ȼ���俽������Ӧ�Ľṹ���У���Щ���ݰ���Ҫͨ����λ���򿪶�Ӧ������󣬲��ܽ��յ�������ݰ�������
						case 0x50:	memcpy(&stcTime,&RxBuffer[2],8);	 break;//memcpyΪ�������Դ����ڴ濽��������������"string.h"�������ջ��������ַ����������ݽṹ�����棬�Ӷ�ʵ�����ݵĽ�����
						case 0x51:	memcpy(&stcAcc,&RxBuffer[2],8);		 break;
						case 0x52:	memcpy(&stcGyro,&RxBuffer[2],8);	 break;
						case 0x53:	memcpy(&stcAngle,&RxBuffer[2],8);	 break;
						case 0x54:	memcpy(&stcMag,&RxBuffer[2],8);		 break;
						case 0x55:	memcpy(&stcDStatus,&RxBuffer[2],8);break;
						case 0x56:	memcpy(&stcPress,&RxBuffer[2],8);	 break;
						case 0x57:	memcpy(&stcLonLat,&RxBuffer[2],8); break;
						case 0x58:	memcpy(&stcGPSV,&RxBuffer[2],8);	 break;
						case 0x59:	memcpy(&stcQ,&RxBuffer[2],8);			 break;
				}
				RxCount = 0;//��ջ�����
				RxCheck = 0;//У��λ����
		}
		else{  //��������
				RxCount = 0;//��ջ�����
				RxCheck = 0;//У��λ����
				return;
		}
		/*********** ------------------------------------- ************/

}

/* JY901 ����ת�� */
void JY901_Convert(struct JY901Type * pArr) 
{

		pArr->Acc.x  = (float)stcAcc.a[0]/2048;   //32768*16
		pArr->Acc.y  = (float)stcAcc.a[1]/2048;
		pArr->Acc.z  = (float)stcAcc.a[2]/2048;
	
		pArr->Gyro.x = (float)stcGyro.w[0]/2048*125;  //32768*2000
		pArr->Gyro.y = (float)stcGyro.w[1]/2048*125;
		pArr->Gyro.z = (float)stcGyro.w[2]/2048*125;
	
		pArr->Euler.Roll = (float)stcAngle.angle[0]/8192*45;   //32768*180; 
		pArr->Euler.Pitch = (float)stcAngle.angle[1]/8192*45;
		pArr->Euler.Yaw = (float)stcAngle.angle[2]/8192*45;
	
		pArr->Mag.x 	= stcMag.h[0];
		pArr->Mag.y		= stcMag.h[1];
		pArr->Mag.z 	= stcMag.h[2];
	
		pArr->Temperature = (float)stcAcc.T/100;
}






void show_logo(void)
{
		rt_kprintf("      *      \n");
		rt_kprintf(" *  *   *  * \n");
		rt_kprintf("  *   e   *   \n");
		rt_kprintf(" *  *   *  * \n");
		rt_kprintf("      *      \n");	
	
}
MSH_CMD_EXPORT(show_logo,show_logo);


/* Getʱ��  time */
void get_time(void)
{
		//���ݴ����string��       ��ΪRT-Thread rt_kprintf()�����޷���������ͣ�����ֽ����ݴ����String�ͷ���.
		char str[50];
		sprintf(str,"Time:20%d-%d-%d %d:%d:%.3f",stcTime.ucYear,stcTime.ucMonth,stcTime.ucDay,stcTime.ucHour,stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);
		log_i(str);
}
MSH_CMD_EXPORT(get_time,get acceleration[a]);


/* Get���ٶ�  acceleration */
void get_gyroscope(void)
{		
		char str[50];
		sprintf(str,"Acc:%.3f %.3f %.3f",  JY901.Acc.x,  JY901.Acc.y,  JY901.Acc.z);
		log_i(str);
		sprintf(str,"Gyro:%.3f %.3f %.3f", JY901.Gyro.x, JY901.Gyro.y, JY901.Gyro.z);
		log_i(str);
		sprintf(str,"Angle:%.3f %.3f %.3f",JY901.Euler.Roll,JY901.Euler.Pitch,JY901.Euler.Yaw);
		log_i(str);
		sprintf(str,"Mag:%d %d %d",				 JY901.Mag.x,  JY901.Mag.y,   JY901.Mag.z);
		log_i(str);	
	
		return;
}
MSH_CMD_EXPORT(get_gyroscope,get JY901[a]);


/* Get �¶�  Temperature */
float get_temperature(void)
{
		char str[50];
		sprintf(str,"Temperature:%.2f C\r\n",JY901.Temperature);
		log_i(str);	
		return JY901.Temperature;
}
MSH_CMD_EXPORT(get_temperature, get Temperature[T]);








