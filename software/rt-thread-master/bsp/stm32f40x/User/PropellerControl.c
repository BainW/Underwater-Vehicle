/*
 * PropellerControl.c
 *
 *  Created on: 2019��3��20��
 *      Author: 219
 *      Notes:  �ƽ�������
 */
#include <math.h>
#include "propeller.h"
#include "PropellerControl.h"
#include "RC_Data.h"
#include "pwm.h"


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
* ��    �ܣ��ƽ����������
* ���������PowerValue
* �� �� ֵ���޷���� PowerValue
* ע    �⣺���ֵΪPropeller.PowerMax ��ʼ��Ϊ1800
						��СֵΪPropeller.PowerMin ��ʼ��Ϊ1300


********************************************/
uint16 Output_Limit(uint16 *PowerValue)
{
		*PowerValue = (*PowerValue  + PropellerParameter.PowerMed) > PropellerParameter.PowerMax ? PropellerParameter.PowerMax : *PowerValue ;//�����޷�
		*PowerValue = (*PowerValue  + PropellerParameter.PowerMed) < PropellerParameter.PowerMin ? PropellerParameter.PowerMin : *PowerValue ;//�����޷�
	
		return *PowerValue ;
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

		switch(Control.Move){
				case  Forward : robotForward();break;  //ǰ��
				case  BackAway: robotBackAway();break;	 //����
				default:break;
		}
		

		switch(Control.Translation){
				case  MoveLeft : moveLeft()	;break;  //����
				case  MoveRight: moveRight();break;  //����		
				default:break;
		}
		
		
		switch(Control.Vertical){//�п������ݲ������
				case  RiseUp:  ;break;  //����
				case  Dive:    ;break;  //��Ǳ
				
				default:break/*�����PID*/;
		}
	
		switch(Control.Rotate){
				case  TurnLeft : turnLeft(); break;  //��ת
				case  TurnRight: turnRight();break;  //��ת			
				default:break;
		}
		Propeller_Output();  //�ƽ�������޷�
}

/*******************************************
* �� �� ����Propeller_Output
* ��    �ܣ��ƽ����������
* ����������˶�ֵ��MoveValue
* �� �� ֵ��none
* ע    �⣺���ֵΪPropeller.PowerMax ��ʼ��Ϊ2000
						��СֵΪPropeller.PowerMin ��ʼ��Ϊ1000
********************************************/
void Propeller_Output(void)
{

		PropellerPower.rightUp = Output_Limit(&PropellerPower.rightUp);
	
		PropellerPower.leftUp = Output_Limit(&PropellerPower.leftUp);
	
		PropellerPower.rightDown = Output_Limit(&PropellerPower.rightDown);
	
		PropellerPower.leftDown = Output_Limit(&PropellerPower.leftDown);
	
		PropellerPower.leftMiddle = Output_Limit(&PropellerPower.leftMiddle);
	
		PropellerPower.rightMiddle = Output_Limit(&PropellerPower.rightMiddle);
		
		PWM_Update();//PWM�ϴ�
		
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
		Control.Power = Control.Power * 2; //���Ŵ�С
	
		PropellerPower.leftUp =   - Control.Power - 32;
		PropellerPower.rightUp =    Control.Power + 32;
		PropellerPower.leftDown = - Control.Power - 32;
		PropellerPower.rightDown =  Control.Power + 32;
}


void robotBackAway(void)  //����
{

		PropellerPower.Power = Control.Power * 2;
	
		PropellerPower.leftUp =Control.Power+2+PropellerError.leftUpError;
		PropellerPower.rightUp = -Control.Power-30-PropellerError.rightUpError;
		PropellerPower.leftDown = Control.Power+2+PropellerError.leftDownError;
		PropellerPower.rightDown = -Control.Power-30-PropellerError.rightDownError;

}


void turnRight(void)  //��ת
{

		PropellerPower.Power = Control.Power * 2;
	
		PropellerPower.leftUp = -PropellerPower.Power-30-PropellerError.leftUpError;
		PropellerPower.rightUp = -PropellerPower.Power-30-PropellerError.rightUpError;
		PropellerPower.leftDown = -PropellerPower.Power-30-PropellerError.leftDownError;
		PropellerPower.rightDown = -PropellerPower.Power-30-PropellerError.rightDownError;

}

void turnLeft(void)  //��ת
{

		PropellerPower.Power = Control.Power * 2;
	
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



/*******************************************
* �� �� ����Propeller_upDown
* ��    �ܣ��ƽ����������½�
* ���������depth�������
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void Propeller_upDown(int depth)  
{
		static uint16 depth_RightDuty = 0;
		static uint16 depth_LeftDuty = 0;
		if(depth < -500) depth = (-500);
		if(depth >  500) depth =   500;

		depth_RightDuty = PropellerPower_Med - depth;//������
		depth_LeftDuty  = PropellerPower_Med + depth;
	
	  depth_RightDuty = Output_Limit(&depth_RightDuty);
		depth_LeftDuty  = Output_Limit(&depth_LeftDuty );

}

