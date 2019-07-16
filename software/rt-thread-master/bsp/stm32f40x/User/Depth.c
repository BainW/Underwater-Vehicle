/*
 * Control.c
 *
 *  Created on: 2019��7��15��
 *      Author: zengwangfa
 *      Notes:  ��ȿ���
 */
 
 
 
 #include "Depth.h"
 #include <math.h>
#include "propeller.h"
#include "PropellerControl.h"
#include "RC_Data.h"
#include "drv_pwm.h"
#include "PID.h"
#include "Control.h"
 
 /*******************************************
* �� �� ����Depth_Control
* ��    �ܣ���ȴ�ֱ�˶�����
* ���������ҡ�˽ṹ��
* �� �� ֵ��none
* ע    �⣺none
********************************************/

void Depth_Control(Rocker_Type *rc)
{
		
		if(UNLOCK == ControlCmd.All_Lock){ //����
		
				if(AUV_Mode == VehicleMode){	 //AUV��ȿ���λ������
						switch(ControlCmd.Vertical){//�п������ݲ������
								case RiseUp: 
										 Expect_Depth-=3 ; 
										 if(Expect_Depth < 0) {Expect_Depth= 0;}//���������е����ֵ������ֵ��������
										 break;  //����
							
								case Dive:   
											if(Total_Controller.High_Position_Control.Control_OutPut < 450){ //���������Χ ֹͣ�ۻ�
													Expect_Depth+=3 ;
											}
											
										 break;  //��Ǳ
								default:break/*�����PID*/;
						}
						ControlCmd.Vertical = 0x00;
				}
		}

			/* ��ֱ���� */
		if(ROV_Mode == VehicleMode)	 //ROV�� ҡ��ģ��������
		{
				if(rc->Z > 5){
					 Expect_Depth -=( (float)rc->Z /80); 
					 if(Expect_Depth < 0) {//���������е����ֵ������ֵ��������
							Expect_Depth= 0;
						}
				}
				else if(rc->Z < -5){
						if(Total_Controller.High_Position_Control.Control_OutPut < 450){ //���������Χ ֹͣ�ۻ�
								Expect_Depth += (fabs((float)rc->Z)/80);
						}
				}
		}
	
		Depth_PID_Control(Expect_Depth,Sensor.DepthSensor.Depth);//��ȿ��� ���ܡ��������ֽڿ���
}




