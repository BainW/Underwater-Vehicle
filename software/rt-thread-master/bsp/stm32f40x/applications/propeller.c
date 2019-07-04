/*
 * servo.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �ƽ����豸
 */
 #define LOG_TAG    "propeller"
 
 
#include <stdlib.h>
#include <string.h>
#include "sys.h"
#include "propeller.h"
#include <elog.h>
#include <rtthread.h>

#include "flash.h"
#include "RC_Data.h"

#define Propeller_MedValue 1500
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


PropellerPower_Type power_test; //�����õı���

void PWM_Update(PropellerPower_Type* power)
{	
		power_test.rightUp = Propeller_MedValue + power->rightUp;
		power_test.leftDown = Propeller_MedValue + power->leftDown;
		power_test.leftUp = Propeller_MedValue + power->leftUp;
		power_test.rightDown = Propeller_MedValue + power->rightDown;
		
		power_test.leftMiddle = Propeller_MedValue + power->leftMiddle;
		power_test.rightMiddle = Propeller_MedValue + power->rightMiddle;
		if(1 == Propeller_Init_Flag && 1 == ControlCmd.All_Lock){
				
				TIM_SetCompare1(TIM1,power_test.rightUp);     //����	 E9	
				TIM_SetCompare2(TIM1,power_test.leftDown);    //����	 E11
				TIM_SetCompare3(TIM1,power_test.leftUp); 	    //����   E13
				TIM_SetCompare4(TIM1,power_test.rightDown);   //����   E14
			
				TIM_SetCompare1(TIM4,power_test.leftMiddle);  //����   D12
				TIM_SetCompare2(TIM4,power_test.rightMiddle); //����   D13
		}

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
void Propeller_Init(void)//��߶���Ҫ�����޷��ڸ���  ԭ��Ϊ2000->1500
{

		TIM_SetCompare1(TIM1, 2000);  		//���ת���ź�   	ˮƽ�ƽ���1��
		TIM_SetCompare2(TIM1, 2000);  		//���ת���ź�    ˮƽ�ƽ���2��
		TIM_SetCompare3(TIM1, 2000); 		  //���ת���ź�    ˮƽ�ƽ���3��
		TIM_SetCompare4(TIM1, 2000);  		//���ת���ź�    ˮƽ�ƽ���4��
	
		TIM_SetCompare1(TIM4, 2000); 	 	//���ת���ź�  	��ֱ�ƽ���1��
		TIM_SetCompare2(TIM4, 2000);	  //���ת���ź�  	��ֱ�ƽ���2��

		TIM_SetCompare3(TIM4, 1900);		//��ֵ
		//TIM_SetCompare4(TIM4, 1700);		//����	

		rt_thread_mdelay(2000);  //2s


		TIM_SetCompare1(TIM1, 1500);			//ͣת�ź�
		TIM_SetCompare2(TIM1, 1500);			//ͣת�ź�
		TIM_SetCompare3(TIM1, 1500);			//ͣת�ź�
		TIM_SetCompare4(TIM1, 1500);			//ͣת�ź�
	
		TIM_SetCompare1(TIM4, 1500);		  //ͣת�ź�
		TIM_SetCompare2(TIM4, 1500);		  //ͣת�ź�

		TIM_SetCompare3(TIM4, 1900);		//��ֵ
		//TIM_SetCompare4(TIM4, 1000);		//����
		rt_thread_mdelay(1000);  //1s
		
	  log_i("Propeller_Init()");
		Propeller_Init_Flag = 1;
}


/*���ƽ����� ��Stop��MSH���� */
void Propeller_Stop(void)
{
		memset(&PropellerPower,0,sizeof(PropellerPower));

}
MSH_CMD_EXPORT(Propeller_Stop,ag: Propeller Stop);



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

