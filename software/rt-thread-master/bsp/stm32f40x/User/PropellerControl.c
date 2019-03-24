#include "init.h"
#include <math.h>
#include "PropellerControl.h"

PropellerParamter_Type PropellerParamter = {//��ʼ���ƽ�������ֵ
		 1500,//��ֵ
		 1800,//���ֵ
		 1200,//��Сֵ
	   100	//����ֵ
}; 

ActionTypeEnum       Posture_Flag; //��������̬��־λ
PropellerPower_Type  PropellerPower; //�ƽ������������
PropellerError_Type  PropellerError; //�ƽ���ƫ��ֵ

u8 turnAngle = 45;    //ת��Ƕ�

unsigned int myabs(int absValue)
{
		return absValue > 0 ? absValue : -absValue;
}



/*******************************************
* �� �� ����askResultant
* ��    �ܣ����������С������
* ���������������Ҫ�ĽǶȺʹ�С forceSize Ϊ��Ҫ�ĺ����Ĵ�С  angleΪת��ĽǶ� 
* �� �� ֵ��force ���������н��Ƕ�����ĵ����ƽ�����������С
* ע    �⣺none
********************************************/
double askResultant(double angle,double forceSize)
{
		double force=0;
		force = forceSize*cos(Deg2Rad(angle));
		return force;
}


/*******************************************
* �� �� ����Output_Limit
* ��    �ܣ��������
* ���������PowerValue
* �� �� ֵ���޷���� PowerValue
* ע    �⣺���ֵΪPropeller.PowerMax ��ʼ��Ϊ1800
						��СֵΪPropeller.PowerMin ��ʼ��Ϊ1300
********************************************/
u16 Output_Limit(u16 PowerValue)
{
		PowerValue = PowerValue > PropellerParamter.PowerMax ? PropellerParamter.PowerMax : PowerValue;//�����޷�
		PowerValue = PowerValue < PropellerParamter.PowerMin ? PropellerParamter.PowerMin : PowerValue;//�����޷�
	
		return PowerValue;
}




/*******************************************
* �� �� ����propeller_control
* ��    �ܣ�ˮƽ���ƽ����Ŀ���
* ���������none
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void Propeller_Control(void)
{
		int MoveValue = 0;
		
		switch(Posture_Flag)
		{
				case  Forward  :  ;break;  //ǰ��
				case  BackAway :  ;break;	 //����
				case  TurnLeft :  ;break;  //��ת
				case  TurnRight:  ;break;  //��ת
				case  MoveLeft :	;break;  //����
				case  MoveRight:  ;break;  //����
				case  Stop     :	;break;  //ֹͣ
				
				default:break;
		}
		Propeller_Output_Limit(MoveValue);  //�ƽ�������޷�
}

/*******************************************
* �� �� ����Propeller_Output_Limit
* ��    �ܣ��ƽ����������
* ����������˶�ֵ��MoveValue
* �� �� ֵ��none
* ע    �⣺���ֵΪPropeller.PowerMax ��ʼ��Ϊ1800
						��СֵΪPropeller.PowerMin ��ʼ��Ϊ1300
********************************************/
void Propeller_Output_Limit(int MoveValue)
{
		
		#if VehicleMode == AUV_Mode
			MoveValue = PropellerParamter.PowerMed + PropellerPower.leftUp;
			MoveValue = Output_Limit(MoveValue);
			TIM_SetCompare1(TIM1,MoveValue);			 //����
		#else
			MoveValue = PropellerParamter.PowerMed  + PropellerPower.rightDown;
			MoveValue = Output_Limit(MoveValue);
			TIM_SetCompare1(TIM1,MoveValue);			//����00000000
		#endif
//---------------------------------------------------------------------------//
		#if VehicleMode == AUV_Mode
			MoveValue = PropellerParamter.PowerMed  + PropellerPower.rightUp;
			MoveValue = Output_Limit(MoveValue);
			TIM_SetCompare2(TIM1,MoveValue);			//����
		#else
			MoveValue = PropellerParamter.PowerMed  + PropellerPower.leftUp;
			MoveValue = Output_Limit(MoveValue);
			TIM_SetCompare2(TIM1,MoveValue);		 //����0000000000
		#endif
//---------------------------------------------------------------------------//
		#if VehicleMode == AUV_Mode
			MoveValue = PropellerParamter.PowerMed  + PropellerPower.leftDown;
			MoveValue = Output_Limit(MoveValue);
			TIM_SetCompare3(TIM1,MoveValue);			//����
		#else
			MoveValue = PropellerParamter.PowerMed  + PropellerPower.leftDown;
			MoveValue = Output_Limit(MoveValue);
			TIM_SetCompare2(TIM12,MoveValue);	   //	���� 0000000
		#endif
//---------------------------------------------------------------------------//
		#if VehicleMode == AUV_Mode
			MoveValue = PropellerParamter.PowerMed + PropellerPower.rightDown;
			MoveValue = Output_Limit(MoveValue);
			TIM_SetCompare4(TIM1,MoveValue);			//����
		#else
			MoveValue = PropellerParamter.PowerMed + PropellerPower.rightUp;
			MoveValue = Output_Limit(MoveValue);
			TIM_SetCompare4(TIM1,MoveValue);			//����00000000
		#endif
}



/*******************************************
* �� �� ����robotUp
* ��    �ܣ�������ǰ��
* ���������none
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void robotForward(void)  //ǰ��
{
		#if VehicleMode == AUV_Mode
			PropellerPower.leftUp = -askResultant(45,PropellerPower.Power/2)-23-   PropellerError.leftUpError;
			PropellerPower.rightUp = askResultant(45,PropellerPower.Power/2)+3+    PropellerError.rightUpError;
			PropellerPower.leftDown = askResultant(45,PropellerPower.Power/2)+     PropellerError.leftDownError;
			PropellerPower.rightDown = -askResultant(45,PropellerPower.Power/2)-28-PropellerError.rightDownError;
		#else
			PropellerPower.Power = PropellerPower.Power * 5;
			PropellerPower.leftUp = -PropellerPower.Power-32;
			PropellerPower.rightUp = PropellerPower.Power+5;
			PropellerPower.leftDown = -PropellerPower.Power-32;
			PropellerPower.rightDown = PropellerPower.Power+5;
		#endif
}

void robotBackAway(void)  //����
{
		#if VehicleMode == AUV_Mode	
			PropellerPower.leftUp = askResultant(45,PropellerPower.Power/2)+3+PropellerError.leftUpError;
			PropellerPower.rightUp = -askResultant(45,PropellerPower.Power/2)-26-PropellerError.rightUpError;
			PropellerPower.leftDown = -askResultant(45,PropellerPower.Power/2)-28-PropellerError.leftDownError;
			PropellerPower.rightDown = askResultant(45,PropellerPower.Power/2)+PropellerError.rightDownError;
		#else
			PropellerPower.Power = PropellerPower.Power * 5;
			PropellerPower.leftUp = PropellerPower.Power+2+PropellerError.leftUpError;
			PropellerPower.rightUp = -PropellerPower.Power-30-PropellerError.rightUpError;
			PropellerPower.leftDown = PropellerPower.Power+2+PropellerError.leftDownError;
			PropellerPower.rightDown = -PropellerPower.Power-30-PropellerError.rightDownError;
		#endif
}


void turnRight(void)  //��ת
{
		#if VehicleMode == AUV_Mode
			PropellerPower.leftUp = -askResultant(45,PropellerPower.Power/4)-28;
			PropellerPower.rightUp = -askResultant(45,PropellerPower.Power/2)-20;
			PropellerPower.leftDown = askResultant(45,PropellerPower.Power/4)+3;
			PropellerPower.rightDown = askResultant(45,PropellerPower.Power/4)+1;
		#else
			PropellerPower.Power = PropellerPower.Power * 3;
			PropellerPower.leftUp = -PropellerPower.Power-30-PropellerError.leftUpError;
			PropellerPower.rightUp = -PropellerPower.Power-30-PropellerError.rightUpError;
			PropellerPower.leftDown = -PropellerPower.Power-30-PropellerError.leftDownError;
			PropellerPower.rightDown = -PropellerPower.Power-30-PropellerError.rightDownError;
		#endif
}

void turnLeft(void)  //��ת
{
		#if VehicleMode == AUV_Mode
			PropellerPower.leftUp = askResultant(45,PropellerPower.Power/2)-5;
			PropellerPower.rightUp = askResultant(45,PropellerPower.Power/3);
			PropellerPower.leftDown = -askResultant(45,PropellerPower.Power/4)-30;
			PropellerPower.rightDown = -askResultant(45,PropellerPower.Power/4)-30;
		#else
			PropellerPower.Power = PropellerPower.Power * 3;
			PropellerPower.leftUp = PropellerPower.Power+0+PropellerError.leftUpError;
			PropellerPower.rightUp = PropellerPower.Power+0+PropellerError.rightUpError;
			PropellerPower.leftDown = PropellerPower.Power+0+PropellerError.leftDownError;
			PropellerPower.rightDown = PropellerPower.Power+0+PropellerError.rightDownError;
		#endif
}


void moveLeft(void)  //����
{
		#if VehicleMode == AUV_Mode
			PropellerPower.leftUp = askResultant(45,PropellerPower.Power/2)+5+PropellerError.leftUpError;
			PropellerPower.rightUp = askResultant(45,PropellerPower.Power/2)+6+PropellerError.rightUpError;
			PropellerPower.leftDown = askResultant(45,PropellerPower.Power/2)+3+PropellerError.leftDownError;
			PropellerPower.rightDown = askResultant(45,PropellerPower.Power/2)+3+PropellerError.rightDownError;
		#else
			PropellerPower.Power = PropellerPower.Power * 4;
			PropellerPower.leftUp = PropellerPower.Power+5+PropellerError.leftUpError;
			PropellerPower.rightUp = PropellerPower.Power+5+PropellerError.rightUpError;
			PropellerPower.leftDown = -PropellerPower.Power-32-PropellerError.leftDownError;
			PropellerPower.rightDown = -PropellerPower.Power-32-PropellerError.rightDownError;
		#endif
}



void moveRight(void)  //����
{
		#if VehicleMode == AUV_Mode
			PropellerPower.leftUp = -askResultant(45,PropellerPower.Power/2)-25-PropellerError.leftUpError;
			PropellerPower.rightUp = -askResultant(45,PropellerPower.Power/2)-25-PropellerError.rightUpError;
			PropellerPower.leftDown = -askResultant(45,PropellerPower.Power/2)-25-PropellerError.leftDownError;
			PropellerPower.rightDown = -askResultant(45,PropellerPower.Power/2)-27-PropellerError.rightDownError;

		#else
			PropellerPower.Power = PropellerPower.Power * 4;
			PropellerPower.leftUp = -PropellerPower.Power-32-PropellerError.leftUpError;
			PropellerPower.rightUp = -PropellerPower.Power-32-PropellerError.rightUpError;
			PropellerPower.leftDown = PropellerPower.Power+5+PropellerError.leftDownError;
			PropellerPower.rightDown = PropellerPower.Power+5+PropellerError.rightDownError;
		#endif
}



/*******************************************
* �� �� ����Propeller_Init
* ��    �ܣ��ƽ����ĳ�ʼ��
* ���������none
* �� �� ֵ��none
* ע    �⣺��ʼ�����̣�
*           1,����,�ϵ磬��-��-������,��ʾ��������
*           2,�����2ms��1ms���ת���ź�,��һ��
*           3,�����1.5msͣת�ź�,��һ��
*           4,��ʼ����ɣ����Կ�ʼ����
********************************************/
void Propeller_Init(void)
{
		TIM_SetCompare1(TIM12,1000); 	 	//���ת���ź�  	��ֱ�ƽ���1��
		TIM_SetCompare2(TIM12,1000);	  //���ת���ź�  	��ֱ�ƽ���2��
		TIM_SetCompare1(TIM1,1000);  		//���ת���ź�   	ˮƽ�ƽ���1��
		TIM_SetCompare2(TIM1,1000);  		//���ת���ź�    ˮƽ�ƽ���2��
		TIM_SetCompare3(TIM1,1000); 		//���ת���ź�    ˮƽ�ƽ���3��
		TIM_SetCompare4(TIM1,1000);  		//���ת���ź�    ˮƽ�ƽ���4��
	
		rt_thread_mdelay(1000);   						 	//1s
		TIM_SetCompare1(TIM12,PropellerParamter.PowerMed);		//ͣת�ź�
		TIM_SetCompare2(TIM12,PropellerParamter.PowerMed);		//ͣת�ź�
		TIM_SetCompare1(TIM1, PropellerParamter.PowerMed);			//ͣת�ź�
		TIM_SetCompare2(TIM1, PropellerParamter.PowerMed);			//ͣת�ź�
		TIM_SetCompare3(TIM1, PropellerParamter.PowerMed);			//ͣת�ź�
		TIM_SetCompare4(TIM1, PropellerParamter.PowerMed);			//ͣת�ź�
		rt_thread_mdelay(1000);  
}

