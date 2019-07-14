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
#include "PID.h"
#include "Return_Data.h"
int32 Expect_Depth = 0;




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

		if(UNLOCK == ControlCmd.All_Lock){ //����
				switch(ControlCmd.Vertical){//�п������ݲ������
						case RiseUp: 
							   Expect_Depth-- ; 
								 if(Expect_Depth < 0) Expect_Depth= 0;//���������е����ֵ����������
								 break;  //����
					
						case Dive:   
									if(Total_Controller.High_Position_Control.Control_OutPut < 450){ //���������Χ ֹͣ�ۻ�
											Expect_Depth++ ;
									}
									
								 break;  //��Ǳ
						default:break/*�����PID*/;
				}

				switch(ControlCmd.Rotate){
						case  TurnLeft : 
									if(Rocker.Force == 0){turnLeft();} //������ҡ�����ȼ��ߣ�����ҡ��������ʱ����ҡ�� ������תʧЧ
									break;  //����
									
						case  TurnRight: 
									if(Rocker.Force == 0){turnRight();} //������ҡ�����ȼ��ߣ�����ҡ��������ʱ����ҡ�� ������תʧЧ
									break; //��Ǳ
						default:break;
				}

		}
		else {
				Propeller_Stop();		 //�ƽ�����ֵ����
		}
		//Propeller_Output();  //�ƽ����޷����
		
		//ControlCmd.Vertical = 0x00;

		

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
void turnRight(void)  //����
{
		PropellerPower.leftUp =     PropellerDir.leftUp*(70) + PropellerError.leftUp;
		PropellerPower.rightUp =    0 + PropellerError.rightUp;
		PropellerPower.leftDown =   PropellerDir.leftDown*(70) + PropellerError.leftDown;
		PropellerPower.rightDown =  0 + PropellerError.rightDown;
}
MSH_CMD_EXPORT(turnRight,ag: turnRight);


void turnLeft(void)  //����
{

		PropellerPower.leftUp =    0 + PropellerError.leftUp;
		PropellerPower.rightUp =   PropellerDir.rightUp*(70) + PropellerError.rightUp;
		PropellerPower.leftDown =  0 + PropellerError.leftDown;
		PropellerPower.rightDown = PropellerDir.rightDown*(70) + PropellerError.rightDown;
}
MSH_CMD_EXPORT(turnLeft,ag: turnLeft);


void Propller_stop(void)  //�ƽ���ͣת
{
		PropellerPower.leftUp =    0 + PropellerError.leftUp;
		PropellerPower.rightUp =   0 + PropellerError.rightUp;
		PropellerPower.leftDown =  0 + PropellerError.leftDown;
		PropellerPower.rightDown = 0 + PropellerError.rightDown;
	
		PropellerPower.leftMiddle = 0 + PropellerError.leftMiddle;
		PropellerPower.rightMiddle = 0+ PropellerError.rightMiddle; 
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
		//�޷� �������ƽ��� �趨���������ֵ-ͣתֵ(��ֵ)
	
	
		depth_output = depth_output < -(PropellerParameter.PowerMax - PropellerParameter.PowerMed ) ? -(PropellerParameter.PowerMax - PropellerParameter.PowerMed ):depth_output;
		depth_output = depth_output >  (PropellerParameter.PowerMax - PropellerParameter.PowerMed ) ?  (PropellerParameter.PowerMax - PropellerParameter.PowerMed ):depth_output;
		
		PropellerPower.leftMiddle   = - depth_output + PropellerError.leftMiddle;//������
		PropellerPower.rightMiddle  =   depth_output + PropellerError.rightMiddle;
		
	
		if( PropellerPower.rightMiddle > 10){//�� ��תʱ����������10
				PropellerPower.leftMiddle = PropellerPower.leftMiddle -10; //�����ƽ��� ���ڷ���  ��Ҫ�������ⲹ��
		}
		else if( PropellerPower.leftMiddle < -10){//��תʱ
				PropellerPower.rightMiddle = PropellerPower.rightMiddle - 10;
		}
}

