/*
 * RC_Data.c
 *
 *  Created on: 2019��4��5��
 *      Author: zengwangfa
 *      Notes:  ң�������ݽ��� Remote Control receiving Data
 */

#include "RC_Data.h"


ReceiveDataType ReceiveData = {
		.THR = 1500,
		.YAW = 1500,
		.ROL = 1500,
	  .PIT = 1500
};

uint8 Control_Data[20] = {0};


void Remote_Control_Data_Analysis(uint8 Data)
{
		static uint8 RxBuffer[50];  //���ݰ�
		static uint8 RxCheck = 0;	  //βУ����
		static uint8 RxCount = 0;	   //���ռ���
		static uint8 state = 0;
		static uint8 i = 0;	   		  //���ռ���

	
		RxBuffer[RxCount++] = Data;	//���յ������ݴ��뻺������
	
		if(Control_Data[0] != 0xAA){
				RxCount = 0;
				return;
		}
}





			



	
	
	
	
