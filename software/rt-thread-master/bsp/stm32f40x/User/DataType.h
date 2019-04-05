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

struct JY901Type
{
		Vector3f Acc; 		//���ٶ�
		Vector3f Gyro; 		//���ٶ�
		Vector3s Mag;     //�ų�
		Euler3f Euler;		//ŷ����
		float Temperature;	//�¶�
};



extern  uint8 VehicleMode;   //ROV_Mode or AUV_Mode

typedef enum {
		System_NORMAL_STATUS = 1,//����ģʽ
		System_DEBUG_STATUS = 2, //����ģʽ
		System_ERROR_STATUS,
}VehicleStatus_Enum;  //ö��ϵͳ״̬




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

#define PI 3.1415926f //float��
#define Rad2Deg(Rad) (Rad * 180.0f / PI)
#define Deg2Rad(Deg) (Deg * PI / 180.0f)


#define AUV_Mode  0//0
#define ROV_Mode  1//1






#endif



