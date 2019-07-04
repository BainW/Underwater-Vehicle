#ifndef __DATA_TYPE_H
#define __DATA_TYPE_H

#define int8 	 char
#define uint8  unsigned char
	
#define int16  short
#define uint16 unsigned short
	
#define int32  int
#define uint32 unsigned int

#define int64  long long
#define uint64 unsigned long long
	
#define my_abs(x)  (((x)>0)?(x):-(x))

typedef struct
{
		float p;
		float i;
		float d;
}Vector3f_pid;


typedef struct
{
		float x;
		float y;
		float z;
}Vector3f; //3������ float��

typedef struct
{
		short x;  
		short y;
		short z;
}Vector3s;//3������ short�� 16Ϊ������



typedef struct
{
		float Roll;		 //x 
		float Pitch;   //y
		float Yaw;     //z
}Euler3f;//ŷ����3������ short�� 16Ϊ������

typedef struct 
{
		Vector3f Acc; 		//���ٶ�
		Vector3f Gyro; 		//���ٶ�
		Vector3f Speed;   //�ٶ�
		Vector3s Mag;     //�ų�
		Euler3f Euler;		//ŷ����
		float Temperature;	//JY901�¶�
}JY901_Type;

typedef struct 
{
		float Temperature; //CPU �¶�
		float Usage; 		   //CPU ʹ����
}CPU_Type;

typedef struct 
{
		float Temperature; //ˮ��
		uint32 PessureValue; 		 //ѹ��ֵ
		uint32 Init_PessureValue; //��ʼ���ɼ�����ѹ��ֵ
}MS5837_Type;

typedef struct 
{
		float Current; //ˮ��
		float Voltage;
}PowerSource_Type;

typedef  struct{
	
		int32 Depth;  	 //���
		CPU_Type CPU;           //CPU���¶ȡ���ʹ���ʡ�
		JY901_Type JY901;       //��ŷ���ǡ����ٶȡ�
		MS5837_Type MS5837;     //ˮ���¶ȡ�����ȡ�
 		PowerSource_Type PowerSource; //��Դ
}Sensor_Type;


typedef enum {
		System_NORMAL_STATUS = 1,//����ģʽ
		System_DEBUG_STATUS = 2, //����ģʽ
		System_ERROR_STATUS,
}VehicleStatus_Enum;  //ö��ϵͳ״̬

extern Sensor_Type Sensor;
extern  uint8 VehicleMode;   //ROV_Mode or AUV_Mode

/************��Ҫ����***************/


#define LED_EVENT 			(1 << 0)  //LED�¼���־λ
#define KEY_EVENT 			(1 << 1)  //KEY�¼���־λ
#define BUZZ_EVENT 			(1 << 2)  //BUZZER�¼���־λ
#define OLED_EVENT 			(1 << 3)  //OLED�¼���־λ
#define GYRO_EVENT 			(1 << 4)  //Gyroscope�¼���־λ
#define ADC_EVENT 			(1 << 5)  //ADC�¼���־λ
#define PWM_EVENT 			(1 << 6)  //PWM�¼���־λ
#define CAM_EVENT 			(1 << 7)  //Camera�¼���־λ
#define MS5837_EVENT 	  (1 << 8)  //Sensor�¼���־λ

#define PI 3.141592f
#define Rad2Deg(Rad) (Rad * 180.0f / PI) //������ת�Ƕ�ֵ
#define Deg2Rad(Deg) (Deg * PI / 180.0f) //�Ƕ�ֵת������


#define AUV_Mode  0//0
#define ROV_Mode  1//1

#define LOCK   1 //ȫ����   �궨��
#define UNLOCK 2 //ȫ������ 

#define PropellerPower_Med  1500
#define PropellerPower_Min  1000
#define PropellerPower_Max  2000


#endif



