#ifndef __PROPELLER_H
#define __PROPELLER_H

#include "sys.h"


typedef struct //�ƽ�����ֵ�����ֵ����Сֵ
{
		u16 PowerMed;
		u16 PowerMax;		
		u16 PowerMin;	
	  u16 PowerDeadband;	//����ֵ
	
}PropellerParamter_Type;//�ƽ��������ṹ��


typedef struct//����ʱ�����ƽ�����ƫ���� 
{
		int leftUpError;
		int rightUpError;
		int leftDownError;
		int rightDownError;
		int leftMiddleError;
		int RightMiddleError;

}PropellerError_Type;//�ƽ���ƫ��ֵ


typedef struct//����ʱ�����ƽ�����ƫ���� 
{
	  u16 Power; //��������
	
		u16 leftUp;
		u16 rightUp;
		u16 leftDown;
		u16 rightDown;
		u16 leftMiddle;
		u16 rightMiddle;

}PropellerPower_Type;//�ƽ���ƫ��ֵ

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




