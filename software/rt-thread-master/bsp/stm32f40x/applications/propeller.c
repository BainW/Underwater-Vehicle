/*
 * servo.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �ƽ����豸
 */
#include "propeller.h"
#include <elog.h>
#include <rtthread.h>
#include "sys.h"



PropellerParamter_Type PropellerParamter = {//��ʼ���ƽ�������ֵ
		 .PowerMed = 1500,//��ֵ
		 .PowerMax = 2000,//�������ֵ
		 .PowerMin = 1000,//������Сֵ�������������
	   .PowerDeadband = 10	//����ֵ
}; 

ActionTypeEnum       Posture_Flag; //��������̬��־λ
PropellerPower_Type  PropellerPower; //�ƽ������������
PropellerError_Type  PropellerError = {1,1,1,1,1,1}; //�ƽ���ƫ��ֵ




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

		TIM_SetCompare1(TIM1, PropellerPower_Min);  		//���ת���ź�   	ˮƽ�ƽ���1��
		TIM_SetCompare2(TIM1, PropellerPower_Min);  		//���ת���ź�    ˮƽ�ƽ���2��
		TIM_SetCompare3(TIM1, PropellerPower_Min); 		  //���ת���ź�    ˮƽ�ƽ���3��
		TIM_SetCompare4(TIM1, PropellerPower_Min);  		//���ת���ź�    ˮƽ�ƽ���4��
	
		TIM_SetCompare1(TIM4, PropellerPower_Min); 	 	//���ת���ź�  	��ֱ�ƽ���1��
		TIM_SetCompare2(TIM4, PropellerPower_Min);	  //���ת���ź�  	��ֱ�ƽ���2��

		TIM_SetCompare3(TIM4, PropellerPower_Min);		//ͣת�ź�
		TIM_SetCompare4(TIM4, 1500);		//����


		rt_thread_mdelay(500);  //0.5s

		TIM_SetCompare1(TIM1, PropellerPower_Med);			//ͣת�ź�
		TIM_SetCompare2(TIM1, PropellerPower_Med);			//ͣת�ź�
		TIM_SetCompare3(TIM1, PropellerPower_Med);			//ͣת�ź�
		TIM_SetCompare4(TIM1, PropellerPower_Med);			//ͣת�ź�
	
		TIM_SetCompare1(TIM4, PropellerPower_Med);		//ͣת�ź�
		TIM_SetCompare2(TIM4, PropellerPower_Med);		//ͣת�ź�

		TIM_SetCompare3(TIM4, PropellerPower_Med);		//ͣת�ź�
		TIM_SetCompare4(TIM4, 2500);		//����
		
		rt_thread_mdelay(500);  //0.5s
		
		log_i("Propeller_Init()");
}

