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


int32 Expect_Depth = 0;


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

		if(ControlCmd.Move != 128 ||  ControlCmd.Translation != 128){
				PropellerPower.leftUp =    128 - ControlCmd.Move;
				PropellerPower.rightUp =   128 - ControlCmd.Move;   
				PropellerPower.leftDown =  128 - ControlCmd.Translation; 
				PropellerPower.rightDown = 128 - ControlCmd.Translation;
		}
		switch(ControlCmd.Vertical){//�п������ݲ������
			
				case  RiseUp: Expect_Depth-- ; break;  //����
				case  Dive:   Expect_Depth++ ; break;  //��Ǳ
				
				default:break/*�����PID*/;
		}
		ControlCmd.Vertical = 0x00;
	
		
		if(1 == ControlCmd.All_Lock){
				Propeller_Output();  //�ƽ����޷����
		}
		else {
				Propeller_Stop();	//�ƽ�����ֵ����
		}
		



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
		
		PWM_Update(&PropellerPower);//PWMֵ����
	
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
		PropellerPower.Power = ControlCmd.Power * 2; //���Ŵ�С
	
		PropellerPower.leftUp =    - PropellerPower.Power +PropellerError.leftUpError;
		PropellerPower.rightUp =     PropellerPower.Power +PropellerError.rightUpError;   
		PropellerPower.leftDown =  - PropellerPower.Power +PropellerError.leftDownError;  //��
		PropellerPower.rightDown = - PropellerPower.Power +PropellerError.rightDownError;
}
MSH_CMD_EXPORT(robotForward,ag: robotForward);

void robotBackAway(void)  //����
{

		PropellerPower.Power = ControlCmd.Power * 2;
	
		PropellerPower.leftUp =    PropellerPower.Power +PropellerError.leftUpError;
		PropellerPower.rightUp = - PropellerPower.Power +PropellerError.rightUpError;
		PropellerPower.leftDown =  PropellerPower.Power +PropellerError.leftDownError;
		PropellerPower.rightDown = PropellerPower.Power +PropellerError.rightDownError;

}
MSH_CMD_EXPORT(robotBackAway,ag: robotBackAway);


void turnRight(void)  //��ת
{

		PropellerPower.Power = ControlCmd.Power * 2;
	
		PropellerPower.leftUp =     PropellerPower.Power  +PropellerError.leftUpError;
		PropellerPower.rightUp =    0 +PropellerError.rightUpError;
		PropellerPower.leftDown =   0 +PropellerError.leftDownError;
		PropellerPower.rightDown =  -PropellerPower.Power +PropellerError.rightDownError;
}
MSH_CMD_EXPORT(turnRight,ag: turnRight);

void turnLeft(void)  //��ת
{

		PropellerPower.Power = ControlCmd.Power * 2;
	
		PropellerPower.leftUp =     0 +PropellerError.leftUpError;
		PropellerPower.rightUp =   -PropellerPower.Power +PropellerError.rightUpError;
		PropellerPower.leftDown =  -PropellerPower.Power +PropellerError.leftDownError;
		PropellerPower.rightDown =  0 +PropellerError.rightDownError;
}
MSH_CMD_EXPORT(turnLeft,ag: turnLeft);

void moveLeft(void)  //����
{

		PropellerPower.Power = ControlCmd.Power * 2;

		PropellerPower.leftUp =    PropellerPower.Power + PropellerError.leftUpError;
		PropellerPower.rightUp =   PropellerPower.Power + PropellerError.rightUpError;
		PropellerPower.leftDown = -PropellerPower.Power + PropellerError.leftDownError;
		PropellerPower.rightDown = PropellerPower.Power + PropellerError.rightDownError;

}

MSH_CMD_EXPORT(moveLeft,ag: moveLeft);

void moveRight(void)  //����
{
		PropellerPower.Power = ControlCmd.Power * 2;
	
		PropellerPower.leftUp =    -PropellerPower.Power + PropellerError.leftUpError;
		PropellerPower.rightUp =   -PropellerPower.Power + PropellerError.rightUpError;
		PropellerPower.leftDown =   PropellerPower.Power + PropellerError.leftDownError;
		PropellerPower.rightDown = -PropellerPower.Power + PropellerError.rightDownError;
}
MSH_CMD_EXPORT(moveRight,ag: moveRight);


void Propller_stop(void)  //�ƽ�������
{
		PropellerPower.leftUp =    0 + PropellerError.leftUpError;
		PropellerPower.rightUp =   0 + PropellerError.rightUpError;
		PropellerPower.leftDown =  0 + PropellerError.leftDownError;
		PropellerPower.rightDown = 0 + PropellerError.rightDownError;
	
		PropellerPower.leftMiddle = 0 + PropellerError.leftMiddleError;
		PropellerPower.rightMiddle = 0+ PropellerError.rightMiddleError; 
}
MSH_CMD_EXPORT(Propller_stop,ag: propller_stop);


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

