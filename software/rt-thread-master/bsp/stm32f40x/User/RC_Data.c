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
uint8 Receive_Data_OK = 0;
ControlDataType Control;

/**
  * @brief  Remote_Control_Data_Analysis(�������ݽ���)
  * @param  �����ַ����� uint8 Data
  * @retval None
  * @notice �ӵ��ĸ��ֽڿ�ʼΪ�����ַ�
  */
void Remote_Control_Data_Analysis(uint8 Data) //�������ݽ���
{
		static uint8 RxCheck = 0;	  //βУ����
		static uint8 RxCount = 0;	  //���ռ���
		static uint8 i = 0;	   		  //
	
		Control_Data[RxCount++] = Data;	//���յ������ݴ��뻺������
	
		if(Control_Data[0] == 0xAA){ //���յ���ͷ0xAA
				if(Control_Data[1] == 0x55){ //���յ���ͷ0x55
						if(RxCount >= Control_Data[2]){ //�ж����ݰ�����
								for(i = 0;i <= (RxCount-2);i++){ //�ۼӺ�У��
										RxCheck += Control_Data[i];
								}
								if(RxCheck == Control_Data[RxCount-1]){
										Receive_Data_OK = 1; //�������ݰ��ɹ�
								}
								else {Receive_Data_OK = 0;}
								
								RxCheck = 0; //�����������
								RxCount = 0;	
						}
						else {Receive_Data_OK = 0;RxCount = 0;return;} //���ղ��ɹ�����
				}
				else {Receive_Data_OK = 0;RxCount = 0;return;} //���ղ��ɹ�����
		}
		else {Receive_Data_OK = 0;RxCount = 0;return;} //���ղ��ɹ�����
		
		if(1 == Receive_Data_OK){
			
				Control.Depth_Lock     = Control_Data[3]; //��̬����
				Control.Direction_Lock = Control_Data[4];
				Control.Move					 = Control_Data[5];
				Control.Translation		 = Control_Data[6];
				Control.Vertical 			 = Control_Data[7];
				Control.Rotate 				 = Control_Data[8];
				
				Control.Power 				 = Control_Data[9];  //��������
				Control.Light 				 = Control_Data[10]; //�ƹ����
				
				Control.Focus 				 = Control_Data[11]; //�佹����ͷ����
				Control.Zoom 					 = Control_Data[12];
				
				Control.Yuntai 				 = Control_Data[13]; //��̨����
				Control.Arm						 = Control_Data[14]; //��е�ۿ���
		}
}





			



	
	
	
	
