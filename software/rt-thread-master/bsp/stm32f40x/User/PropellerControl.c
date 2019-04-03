
#include <math.h>
#include "propeller.h"
#include "PropellerControl.h"



uint8 turnAngle = 45;    //ת��Ƕ�




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
uint16 Output_Limit(uint16 PowerValue)
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
		

			MoveValue = PropellerParamter.PowerMed  + PropellerPower.rightDown;
			MoveValue = Output_Limit(MoveValue);
			//TIM_SetCompare1(TIM1,MoveValue);			//����00000000
			
			MoveValue = PropellerParamter.PowerMed  + PropellerPower.leftUp;
			MoveValue = Output_Limit(MoveValue);
			//TIM_SetCompare2(TIM1,MoveValue);		 //����0000000000
			
			MoveValue = PropellerParamter.PowerMed  + PropellerPower.leftDown;
			MoveValue = Output_Limit(MoveValue);
			//TIM_SetCompare2(TIM12,MoveValue);	   //	���� 0000000
			
			MoveValue = PropellerParamter.PowerMed + PropellerPower.rightUp;
			MoveValue = Output_Limit(MoveValue);
			//TIM_SetCompare4(TIM1,MoveValue);			//����00000000


}



/*******************************************
* �� �� ����robotForward
* ��    �ܣ�������ǰ��
* ���������none
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void robotForward(void)  //ǰ��
{
			PropellerPower.Power = PropellerPower.Power * 5;
			PropellerPower.leftUp = -PropellerPower.Power-32;
			PropellerPower.rightUp = PropellerPower.Power+5;
			PropellerPower.leftDown = -PropellerPower.Power-32;
			PropellerPower.rightDown = PropellerPower.Power+5;

}

void robotBackAway(void)  //����
{

			PropellerPower.Power = PropellerPower.Power * 5;
			PropellerPower.leftUp = PropellerPower.Power+2+PropellerError.leftUpError;
			PropellerPower.rightUp = -PropellerPower.Power-30-PropellerError.rightUpError;
			PropellerPower.leftDown = PropellerPower.Power+2+PropellerError.leftDownError;
			PropellerPower.rightDown = -PropellerPower.Power-30-PropellerError.rightDownError;

}


void turnRight(void)  //��ת
{

			PropellerPower.Power = PropellerPower.Power * 3;
			PropellerPower.leftUp = -PropellerPower.Power-30-PropellerError.leftUpError;
			PropellerPower.rightUp = -PropellerPower.Power-30-PropellerError.rightUpError;
			PropellerPower.leftDown = -PropellerPower.Power-30-PropellerError.leftDownError;
			PropellerPower.rightDown = -PropellerPower.Power-30-PropellerError.rightDownError;

}

void turnLeft(void)  //��ת
{

			PropellerPower.Power = PropellerPower.Power * 3;
			PropellerPower.leftUp = PropellerPower.Power+0+PropellerError.leftUpError;
			PropellerPower.rightUp = PropellerPower.Power+0+PropellerError.rightUpError;
			PropellerPower.leftDown = PropellerPower.Power+0+PropellerError.leftDownError;
			PropellerPower.rightDown = PropellerPower.Power+0+PropellerError.rightDownError;

}


void moveLeft(void)  //����
{

			PropellerPower.Power = PropellerPower.Power * 4;
			PropellerPower.leftUp = PropellerPower.Power+5+PropellerError.leftUpError;
			PropellerPower.rightUp = PropellerPower.Power+5+PropellerError.rightUpError;
			PropellerPower.leftDown = -PropellerPower.Power-32-PropellerError.leftDownError;
			PropellerPower.rightDown = -PropellerPower.Power-32-PropellerError.rightDownError;

}



void moveRight(void)  //����
{


			PropellerPower.Power = PropellerPower.Power * 4;
			PropellerPower.leftUp = -PropellerPower.Power-32-PropellerError.leftUpError;
			PropellerPower.rightUp = -PropellerPower.Power-32-PropellerError.rightUpError;
			PropellerPower.leftDown = PropellerPower.Power+5+PropellerError.leftDownError;
			PropellerPower.rightDown = PropellerPower.Power+5+PropellerError.rightDownError;

}


