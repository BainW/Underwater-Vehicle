#ifndef __PROPELLER_H
#define __PROPELLER_H

#include "DataType.h"


typedef struct //�ƽ�����ֵ�����ֵ����Сֵ
{
		uint16 PowerMed;
		uint16 PowerMax;		
		uint16 PowerMin;	
	  uint16 PowerDeadband;	//����ֵ
	
}PropellerParamter_Type;//�ƽ��������ṹ��


typedef struct//����ʱ�����ƽ�����ƫ���� 
{
		int leftUpError;  //����
		int rightUpError;
		int leftDownError; //����
		int rightDownError;
		int leftMiddleError; //����
		int rightMiddleError;

}PropellerError_Type;//�ƽ���ƫ��ֵ


typedef struct
{
	  uint16 Power; //��������
	
		uint16 leftUp;
		uint16 rightUp;
		uint16 leftDown;
		uint16 rightDown;
		uint16 leftMiddle;
		uint16 rightMiddle;

}PropellerPower_Type;//�����ƽ�������

typedef enum
{
		Forward = 1, //ǰ��
		BackAway,    //����
		TurnLeft,    //��ת
		TurnRight,   //��ת
		MoveLeft,    //����
		MoveRight,   //����
		RiseUp,
		Dive,
		Stop
		
}ActionTypeEnum; //����ָ��ö��


extern PropellerParamter_Type PropellerParamter; 
extern ActionTypeEnum       Posture_Flag; //��������̬��־λ
extern PropellerPower_Type  PropellerPower; //�ƽ������������
extern PropellerError_Type  PropellerError; //�ƽ���ƫ��ֵ


void Propeller_Init(void);





#endif




