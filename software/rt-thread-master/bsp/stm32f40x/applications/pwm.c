/*
 * pwm.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  PWM�豸
 */
#define LOG_TAG    "pwm"
#include "pwm.h"
#include "sys.h"
#include <elog.h>
#include "propeller.h"
#include "PropellerControl.h"
/*---------------------- Constant / Macro Definitions -----------------------*/		



/*----------------------- Variable Declarations -----------------------------*/




/*----------------------- Function Implement --------------------------------*/


void PWM_Update(void)
{
		TIM_SetCompare1(TIM1,PropellerParameter.PowerMed + PropellerPower.rightUp);     //����	 E9	
		TIM_SetCompare2(TIM1,PropellerParameter.PowerMed + PropellerPower.leftDown);    //����	 E11
		TIM_SetCompare3(TIM1,PropellerParameter.PowerMed + PropellerPower.leftUp); 	    //����   E13
		TIM_SetCompare4(TIM1,PropellerParameter.PowerMed + PropellerPower.rightDown);   //����   E14
	
		TIM_SetCompare1(TIM4,PropellerParameter.PowerMed + PropellerPower.leftMiddle);  //����   D12
		TIM_SetCompare2(TIM4,PropellerParameter.PowerMed + PropellerPower.rightMiddle); //����   D13
	
		PropellerPower.rightUp = 0;
		PropellerPower.leftDown = 0;
		PropellerPower.leftUp = 0;
		PropellerPower.rightDown= 0;
		PropellerPower.leftMiddle = 0;
		PropellerPower.rightMiddle =0;
//		TIM_SetCompare1(TIM1,1500);     //����	 E9	
//		TIM_SetCompare2(TIM1,1500);    //����	 E11
//		TIM_SetCompare3(TIM1,1500); 	    //����   E13
//		TIM_SetCompare4(TIM1,1500);   //����   E14
//	
//		TIM_SetCompare1(TIM4,1500);  //����   D12
//		TIM_SetCompare2(TIM4,1500); //����   D13
}

/*
��1����APB1��APB2��Ƶ��Ϊ1��ʱ��TIM1��TIM8~TIM11��ʱ��ΪAPB2��ʱ�ӣ�TIM2~TIM7��TIM12~TIM14��ʱ��ΪAPB1��ʱ�ӣ�

��2�������APB1��APB2��Ƶ����Ϊ1����ôTIM1��TIM8~TIM11��ʱ��ΪAPB2��ʱ�ӵ�������TIM2~TIM7��TIM12~TIM14��ʱ��ΪAPB1��ʱ�ӵ�������
		 ����ʱ�ӷ�������֪
		 ��Ϊϵͳ��ʼ��SystemInit�������ʼ��APB1����ʱ��Ϊ4��Ƶ��42M��APB2����ʱ��Ϊ2��Ƶ��84M��
		 ����TIM1��TIM8~TIM11��ʱ��ΪAPB2ʱ�ӵ�������168M��TIM2~TIM7��TIM12~TIM14��ʱ��ΪAPB1��ʱ�ӵ�������84M��
*/


//TIM1 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��

void TIM1_PWM_Init(uint32 arr,uint32 psc)
{		 					 
		//�˲������ֶ��޸�IO������
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);  	//TIM1ʱ��ʹ��    
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//ʹ��PORTEʱ��	
		
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource9, GPIO_AF_TIM1);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_TIM1);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_TIM1);
		GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_TIM1);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11 | GPIO_Pin_13 | GPIO_Pin_14;//GPIOE
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
		GPIO_Init(GPIOE,&GPIO_InitStructure);              //��ʼ��P
			
		TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
		TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
		
		TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);//��ʼ����ʱ��1
		
		//��ʼ��TIM14 Channel1 PWMģʽ	 
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ե�
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
		
		TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
		TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
		TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
		TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
		
		TIM_CtrlPWMOutputs(TIM1,ENABLE);

}  

//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(uint32 arr,uint32 psc)
{		 					 
		//�˲������ֶ��޸�IO������
		
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM1ʱ��ʹ��    
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); 	//ʹ��PORTFʱ��	
		
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource13,GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_TIM4);
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_TIM4);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;//GPIOD
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
		GPIO_Init(GPIOD,&GPIO_InitStructure);              //��ʼ��PF9
			
		TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
		TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
		
		TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//��ʼ����ʱ��4
		
		//��ʼ��TIM4 Channel PWMģʽ	 
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ե�
		TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCPolarity_High;
		TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
		TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
		
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 4OC1
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 4OC1
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 4OC1		
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 4OC1
		
		TIM_CtrlPWMOutputs(TIM4,ENABLE);
		

 							  
} 

