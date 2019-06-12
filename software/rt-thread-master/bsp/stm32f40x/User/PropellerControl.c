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
#include "drv_pwm.h"
#include <rtthread.h>

uint8 turnAngle = 45;    //ת��Ƕ�

int32 Expect_Depth = 0;
uint16 clear_count = 0;
extern uint8 Frame_EndFlag;
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
uint16 Output_Limit(int16 *PowerValue)
{

		//������+500   ������-500
		*PowerValue = (*PowerValue) > (PropellerParameter.PowerMax - PropellerParameter.PowerMed ) ? (PropellerParameter.PowerMax - PropellerParameter.PowerMed ): *PowerValue ;//�����޷�
		*PowerValue = (*PowerValue) < (PropellerParameter.PowerMin - PropellerParameter.PowerMed ) ? (PropellerParameter.PowerMin - PropellerParameter.PowerMed ): *PowerValue ;//�����޷�
	
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
			
				case  RiseUp: Expect_Depth-- ; break;  //����
				case  Dive:   Expect_Depth++ ; break;  //��Ǳ
				
				default:break/*�����PID*/;
		}
		Control.Vertical = 0x00;
		
		switch(Control.Rotate){
				case  TurnLeft : turnLeft(); break;  //��ת
				case  TurnRight: turnRight();break;  //��ת			
				default:break;
		}
		
		Propeller_Output();  //�ƽ����޷����
		
		Control.Move = 0x00;
		Control.Translation = 0x00;
		Control.Rotate = 0x00;

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
		
		PropellerPower.rightUp = Output_Limit(&PropellerPower.rightUp); //PWM�޷�
	
		PropellerPower.leftUp = Output_Limit(&PropellerPower.leftUp);
	
		PropellerPower.rightDown = Output_Limit(&PropellerPower.rightDown);
	
		PropellerPower.leftDown = Output_Limit(&PropellerPower.leftDown);
	
		PropellerPower.leftMiddle = Output_Limit(&PropellerPower.leftMiddle);
	
		PropellerPower.rightMiddle = Output_Limit(&PropellerPower.rightMiddle);
		
		PWM_Update(&PropellerPower);//PWM�ϴ�
	
		if(Frame_EndFlag == 1 && \
			 Control.Move ==0x00 && Control.Translation == 0x00 && Control.Rotate == 0x00){//���յ�һ֡ ���� �޿����� 
					Horizontal_Propeller_Power_Clear();//10�θ���PWM�����
		}
		

		
}

void Horizontal_Propeller_Power_Clear(void)//ˮƽ������������ 10�κ����
{
		clear_count ++;
		if(Control.Move ==0x00 && Control.Translation == 0x00 && Control.Rotate == 0x00){
				if(clear_count >= 15 ){ //10�ζ��޿�������� �ƽ�������
						PropellerPower.rightUp = 0;
						PropellerPower.leftDown = 0;
						PropellerPower.leftUp = 0;
						PropellerPower.rightDown= 0;
					  
						clear_count = 0;
						Frame_EndFlag = 0;
				}
		}
		else {
			clear_count = 0;
		}
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
		PropellerPower.Power = Control.Power * 2; //���Ŵ�С
	
		PropellerPower.leftUp =    - PropellerPower.Power +PropellerError.leftUpError;
		PropellerPower.rightUp =     PropellerPower.Power +PropellerError.rightUpError;   
		PropellerPower.leftDown =  - PropellerPower.Power +PropellerError.leftDownError;  //��
		PropellerPower.rightDown = - PropellerPower.Power +PropellerError.rightDownError;
}
MSH_CMD_EXPORT(robotForward,ag: robotForward);

void robotBackAway(void)  //����
{

		PropellerPower.Power = Control.Power * 2;
	
		PropellerPower.leftUp =    PropellerPower.Power +PropellerError.leftUpError;
		PropellerPower.rightUp = - PropellerPower.Power +PropellerError.rightUpError;
		PropellerPower.leftDown =  PropellerPower.Power +PropellerError.leftDownError;
		PropellerPower.rightDown = PropellerPower.Power +PropellerError.rightDownError;

}
MSH_CMD_EXPORT(robotBackAway,ag: robotBackAway);


void turnRight(void)  //��ת
{

		PropellerPower.Power = Control.Power * 2;
	
		PropellerPower.leftUp =     PropellerPower.Power  +PropellerError.leftUpError;
		PropellerPower.rightUp =    0 +PropellerError.rightUpError;
		PropellerPower.leftDown =   0 +PropellerError.leftDownError;
		PropellerPower.rightDown =  -PropellerPower.Power +PropellerError.rightDownError;
}
MSH_CMD_EXPORT(turnRight,ag: turnRight);

void turnLeft(void)  //��ת
{

		PropellerPower.Power = Control.Power * 2;
	
		PropellerPower.leftUp =     0 +PropellerError.leftUpError;
		PropellerPower.rightUp =   -PropellerPower.Power +PropellerError.rightUpError;
		PropellerPower.leftDown =  -PropellerPower.Power +PropellerError.leftDownError;
		PropellerPower.rightDown =  0 +PropellerError.rightDownError;
}
MSH_CMD_EXPORT(turnLeft,ag: turnLeft);

void moveLeft(void)  //����
{

		PropellerPower.Power = Control.Power * 2;

		PropellerPower.leftUp =    PropellerPower.Power + PropellerError.leftUpError;
		PropellerPower.rightUp =   PropellerPower.Power + PropellerError.rightUpError;
		PropellerPower.leftDown = -PropellerPower.Power + PropellerError.leftDownError;
		PropellerPower.rightDown = PropellerPower.Power + PropellerError.rightDownError;

}

MSH_CMD_EXPORT(moveLeft,ag: moveLeft);

void moveRight(void)  //����
{
		PropellerPower.Power = Control.Power * 2;
	
		PropellerPower.leftUp =    -PropellerPower.Power + PropellerError.leftUpError;
		PropellerPower.rightUp =   -PropellerPower.Power + PropellerError.rightUpError;
		PropellerPower.leftDown =   PropellerPower.Power + PropellerError.leftDownError;
		PropellerPower.rightDown = -PropellerPower.Power + PropellerError.rightDownError;
}
MSH_CMD_EXPORT(moveRight,ag: moveRight);

/*******************************************
* �� �� ����Propeller_upDown
* ��    �ܣ��ƽ����������½�
* ���������depth�������
* �� �� ֵ��none
* ע    �⣺none
********************************************/
void robot_upDown(float depth_output)  
{
		//�޷�
		depth_output = depth_output < -500 ?-500:depth_output;
		depth_output = depth_output >  500 ? 500:depth_output;
		
		
		PropellerPower.leftMiddle   = - depth_output;//������
		PropellerPower.rightMiddle  =   depth_output;
}

