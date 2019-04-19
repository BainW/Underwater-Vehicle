/*
 * gyroscope.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  ����ģ���ȡ��ת������ �Լ� ���÷���
 */
#define LOG_TAG    "gyro"

#include <string.h>
#include <stdio.h>
#include <rtthread.h>
#include <elog.h>
#include "gyroscope.h"


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

uint8 gyroscope_save_array[5] 		={0xFF,0xAA,0x00,0x00,0x00};	 //0x00-���ñ���  0x01-�ָ��������ò�����
uint8 gyroscope_package_array[5] ={0xFF,0xAA,0x02,0x1F,0x00};	 //���ûش������ݰ���0x1F 0x00 Ϊ <ʱ��> <���ٶ�> <���ٶ�> <�Ƕ�> <�ų�>��
uint8 gyroscope_rate_array[5] 		={0xFF,0xAA,0x03,0x06,0x00};	 //�������� 0x05-5Hz  0x06-10Hz(Ĭ��)  0x07-20Hz
uint8 gyroscope_led_array[5] 		={0xFF,0xAA,0x1B,0x00,0x00}; 	 //�����ڶ�λ 0x00-����LED  0x01-�ر�LED   
uint8 gyroscope_baud_array[5] 		={0xFF,0xAA,0x04,0x02,0x00}; 	 //0x06 - 115200

extern rt_device_t gyro_uart_device;	
/*----------------------- Function Implement --------------------------------*/

//CopeSerialDataΪ����2�жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void CopeSerial2Data(uint8 Data)
{
		static uint8 RxBuffer[50];  //���ݰ�
		static uint8 RxCheck = 0;	  //βУ����
		static uint8 RxCount = 0;	    //���ռ���
		static uint8 i = 0;	   		  //���ռ���
	
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





/* ���� ����ģ�� �������� */
void gyroscope_save(void)
{
			rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //������ٶ�У׼
			log_i("JY901 Save successed!");
}
MSH_CMD_EXPORT(gyroscope_save,gyroscope_save);



///*  ����ģ��  ��λ */
//void gyroscope_reset(void)
//{
//		gyroscope_save_array[3] = 0x01;
//		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //����
//		log_i("JY901 Reset!");
//}
//MSH_CMD_EXPORT(gyroscope_reset,gyroscope reset);


/* ���� ����ģ�� ���ݰ� */
void gyroscope_package_open(void)
{
		gyroscope_save_array[3] = 0x00;
		rt_device_write(gyro_uart_device, 0, gyroscope_package_array, 5);   //ON package �����ش����ݰ�
		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //SAVE
		log_i("Open successed! JY901: 1.Time  2.Acc  3.Gyro  4.Angle  5.Mag OPEN!");
}
MSH_CMD_EXPORT(gyroscope_package_open,gyroscope package open);


/* ���� ����ģ�� LED */
static int gyroscope_led(int argc, char **argv)
{
	  int result = 0;
    if (argc != 2){
        log_i("Proper Usage: gyroscope_led on/off\n");
				result = -RT_ERROR;
        goto _exit;
    }

		if( !strcmp(argv[1],"on") ){
				gyroscope_led_array[3] = 0x00;
				log_i("gyroscope_led on\n");
		}
		else if( !strncmp(argv[1],"off",3) ){
				gyroscope_led_array[3] = 0x01;
				log_i("gyroscope_led off\n");
		}
		else {
				log_e("Error! Proper Usage: gyroscope_led on/off\n");goto _exit;
		}
		rt_device_write(gyro_uart_device, 0, gyroscope_led_array, 5);   //ON LED
		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //����
		
_exit:
    return result;
}
MSH_CMD_EXPORT(gyroscope_led, gyroscope_led on/off);
///* ���� ����ģ�� ���ٶ�У׼ */
//void gyroscope_Acc_calibration_enter(void)
//{
//			uint8 Acc_calibration_enter[5]={0xFF,0xAA,0x01,0x01,0x00};
//			rt_device_write(gyro_uart_device, 0, Acc_calibration_enter, 5);   //ON LED
//			log_i("Acc_calibrationing... ");
//			rt_thread_mdelay(500);
//			log_i("calibration OK, Next -> [gyroscope_save]");
//}
//MSH_CMD_EXPORT(gyroscope_Acc_calibration_enter,gyroscope_Acc_calibration_enter);

///* ���� ����ģ�� �ų� У׼ */
//void gyroscope_Mag_calibration_enter(void)
//{
//			uint8 Mag_calibration_enter[5]={0xFF,0xAA,0x01,0x02,0x00};
//			rt_device_write(gyro_uart_device, 0, Mag_calibration_enter, 5);   //����ų�У׼
//			log_i("Mag_calibrationing... ");
//			rt_thread_mdelay(2000);
//			log_i("After completing the rotation of the three axes... ");
//			log_i("Nest -> [gyroscope_Mag_calibration_exit] ");

//}
//MSH_CMD_EXPORT(gyroscope_Mag_calibration_enter,gyroscope_Mag_calibration_enter);


///* �˳� ����ģ�� �ų�У׼ */
//void gyroscope_Mag_calibration_exit(void)
//{
//			uint8 Mag_calibration_exit[5]={0xFF,0xAA,0x01,0x00,0x00};       
//			rt_device_write(gyro_uart_device, 0, Mag_calibration_exit, 5);   //�˳��ų�У׼
//			rt_thread_mdelay(100);
//			gyroscope_save();                                           //������
//			log_i("Mag_calibration OK & Saved! ");
//}
//MSH_CMD_EXPORT(gyroscope_Mag_calibration_exit,gyroscope_Mag_calibration_exit);



///* ���� ����ģ�� ������Ϊ9600 */
//void gyroscope_baud_9600(void)
//{
//		gyroscope_baud_array[3] = 0x02;
//		rt_device_write(gyro_uart_device, 0, gyroscope_baud_array, 5);   //ON LED
//		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //����
//		log_i("JY901 baud:9600 ");
//}
//MSH_CMD_EXPORT(gyroscope_baud_9600,Modify JY901 baud rate);

/* ���� ����ģ�� ������Ϊ9600 */
void gyroscope_baud_115200(void)
{
		gyroscope_baud_array[3] = 0x06;
		rt_device_write(gyro_uart_device, 0, gyroscope_baud_array, 5);  //115200
		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //����
		log_i("JY901 baud:115200 ");
}
MSH_CMD_EXPORT(gyroscope_baud_115200,Modify JY901 baud rate);


