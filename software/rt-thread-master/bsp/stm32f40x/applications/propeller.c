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
#include <stdlib.h>
#include "sys.h"
#include "flash.h"

uint8 Propeller_Init_Flag = 0;
PropellerParameter_Type PropellerParameter = {//��ʼ���ƽ�������ֵ
		 .PowerMax = 1700,//�������ֵ
		 .PowerMed = 1500,//��ֵ
		 .PowerMin = 1300,//������Сֵ�������������
	
	   .PowerDeadband = 10	//����ֵ
}; 

ActionType_Enum      Posture_Flag; //��������̬��־λ

PropellerPower_Type  PropellerPower = {0,0,0,0,0,0,0}; //�ƽ�������������
PropellerError_Type  PropellerError = {0,0,0,0,0,0}; //�ƽ���ƫ��ֵ


void PWM_Update(PropellerPower_Type* power)
{
		TIM_SetCompare1(TIM1,PropellerParameter.PowerMed + power->rightUp);     //����	 E9	
		TIM_SetCompare2(TIM1,PropellerParameter.PowerMed + power->leftDown);    //����	 E11
		TIM_SetCompare3(TIM1,PropellerParameter.PowerMed + power->leftUp); 	    //����   E13
		TIM_SetCompare4(TIM1,PropellerParameter.PowerMed + power->rightDown);   //����   E14
	
		TIM_SetCompare1(TIM4,PropellerParameter.PowerMed + power->leftMiddle);  //����   D12
		TIM_SetCompare2(TIM4,PropellerParameter.PowerMed + power->rightMiddle); //����   D13
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
void Propeller_Init(void)//��߶���Ҫ�����޷��ڸ���
{

		TIM_SetCompare1(TIM1, PropellerPower_Min);  		//���ת���ź�   	ˮƽ�ƽ���1��
		TIM_SetCompare2(TIM1, PropellerPower_Min);  		//���ת���ź�    ˮƽ�ƽ���2��
		TIM_SetCompare3(TIM1, PropellerPower_Min); 		  //���ת���ź�    ˮƽ�ƽ���3��
		TIM_SetCompare4(TIM1, PropellerPower_Min);  		//���ת���ź�    ˮƽ�ƽ���4��
	
		TIM_SetCompare1(TIM4, PropellerPower_Min); 	 	//���ת���ź�  	��ֱ�ƽ���1��
		TIM_SetCompare2(TIM4, PropellerPower_Min);	  //���ת���ź�  	��ֱ�ƽ���2��

		TIM_SetCompare3(TIM4, PropellerPower_Med);		//��ֵ
		TIM_SetCompare4(TIM4, 1700);		//����	


		rt_thread_mdelay(500);  //0.5s

		TIM_SetCompare1(TIM1, PropellerPower_Med);			//ͣת�ź�
		TIM_SetCompare2(TIM1, PropellerPower_Med);			//ͣת�ź�
		TIM_SetCompare3(TIM1, PropellerPower_Med);			//ͣת�ź�
		TIM_SetCompare4(TIM1, PropellerPower_Med);			//ͣת�ź�
	
		TIM_SetCompare1(TIM4, PropellerPower_Med);		//ͣת�ź�
		TIM_SetCompare2(TIM4, PropellerPower_Med);		//ͣת�ź�

		TIM_SetCompare3(TIM4, PropellerPower_Med);		//��ֵ
		//TIM_SetCompare4(TIM4, 1000);		//����
		
		rt_thread_mdelay(500);  //0.5s
		
	  log_i("Propeller_Init()");
		Propeller_Init_Flag = 1;
}

/*���ƽ����� �޸� ���������ֵ��MSH���� */
static int propeller_maxvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_maxvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 2000){
				PropellerParameter.PowerMax = atoi(argv[1]);
				Flash_Update();
				log_d("Current propeller max_value_set:  %d",PropellerParameter.PowerMax);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_maxvalue_set,ag: propeller set 1600);


/*���ƽ����� �޸� ���������ֵ��MSH���� */
static int propeller_medvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_medvalue_set 1500");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 1500){
				PropellerParameter.PowerMed = atoi(argv[1]);
				Flash_Update();
				log_d("Current propeller med_value_set:  %d",PropellerParameter.PowerMed);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_medvalue_set,ag: propeller set 1500);

/*���ƽ����� �޸� ���������ֵ��MSH���� */
static int propeller_minvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_minvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 1500){
				PropellerParameter.PowerMin = atoi(argv[1]);
				Flash_Update();
				log_d("Current propeller min_value_set:  %d",PropellerParameter.PowerMin);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_minvalue_set,ag: propeller set 1200);

