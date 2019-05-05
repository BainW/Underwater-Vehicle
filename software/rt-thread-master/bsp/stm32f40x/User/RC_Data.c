/*
 * RC_Data.c
 *
 *  Created on: 2019��4��5��
 *      Author: zengwangfa
 *      Notes:  ң�������ݽ��� Remote Control receiving Data
 */
#include "RC_Data.h"
#include "led.h"
ReceiveDataType ReceiveData = {
		.THR = 1500,
		.YAW = 1500,
		.ROL = 1500,
	  .PIT = 1500
};
#define MAX_DATA_LENS 16  //��Ч���ݰ����ȡ������� ��ͷ������λ��У��λ��
uint8 Control_Data[30] = {0};
uint8 Receive_Data_OK = 0;
ControlDataType Control;
uint8 Control_RxCheck = 0;	  //βУ����
uint8 Control_RxCount = 0;	  //���ռ���
/**
  * @brief  Remote_Control_Data_Analysis(�������ݽ���)
  * @param  �����ַ����� uint8 Data
  * @retval None
  * @notice �ӵ��ĸ��ֽڿ�ʼΪ�����ַ�
  */
void Remote_Control_Data_Analysis(uint8 Data) //�������ݽ���
{

		static uint8 i = 0;	
	
		Control_Data[Control_RxCount++] = Data;	//���յ������ݴ��뻺������
		if(Control_RxCount <= (MAX_DATA_LENS+8)){ //�������ݳ���δ������ͷ�Ͱ���3���ֽ�,+4)          
				if(Control_Data[0] == 0xAA){ //���յ���ͷ0xAA
						if(Control_RxCount > 3){
								if(Control_Data[1] == 0x55){ //���յ���ͷ0x55
										if(Control_Data[2] > MAX_DATA_LENS+4){Receive_Data_OK = 0;Control_RxCount = 0;return;} //���ճ���Э���ַ���
											
										if(Control_RxCount >= Control_Data[2]+4){ //���յ����ݰ�����λ����ʼ�ж�ʲôʱ��ʼ����У��
												for(i = 0;i <= (Control_RxCount-2);i++){ //�ۼӺ�У��
														Control_RxCheck += Control_Data[i];
												}
												if(Control_RxCheck == Control_Data[Control_RxCount-1]){
														Receive_Data_OK = 1; //�������ݰ��ɹ�
												}
												else {Receive_Data_OK = 0;}
												 
												Control_RxCheck = 0; //�����������
												Control_RxCount = 0;	
										}
								}
								else {Receive_Data_OK = 0;Control_RxCount = 0;Control_Clear();return;} //���ղ��ɹ�����
						}//
				}
				else {Receive_Data_OK = 0;Control_RxCount = 0;Control_Clear();return;} //���ղ��ɹ�����
		}
		else {Receive_Data_OK = 0;Control_RxCount = 0;Control_Clear();return;} //���ղ��ɹ�����
		
		if(1 == Receive_Data_OK){
			
				Control.Depth_Lock     = Control_Data[3]; //��̬����
				Control.Direction_Lock = Control_Data[4];
				Control.Move					 = Control_Data[5];
				Control.Translation		 = Control_Data[6];
				Control.Vertical 			 = Control_Data[7];
				Control.Rotate 				 = Control_Data[8];
				
				Control.Power 				 = Control_Data[9];  //�������� ����[0~500]
				Control.Light 				 = Control_Data[10]; //�ƹ����
				
				Control.Focus 				 = Control_Data[11]; //�佹����ͷ����
				
				Control.Yuntai 				 = Control_Data[12]; //��̨����
				Control.Arm						 = Control_Data[13]; //��е�ۿ���
		}
		else{
				Control_Clear();
		}
}

void Control_Clear(void)
{
		Control.Depth_Lock     = 0x00; //��̬����
		Control.Direction_Lock = 0x00;
		Control.Move					 = 0x00;
		Control.Translation		 = 0x00;
		Control.Vertical 			 = 0x00;
		Control.Rotate 				 = 0x00;
		
		//Control.Power 				 = 0x00;  //��������
		Control.Light 				 = 0x00; //�ƹ����
		
		Control.Focus 				 = 0x00; //�佹����ͷ����
		
		Control.Yuntai 				 = 0x00; //��̨����
		Control.Arm						 = 0x00; //��е�ۿ���
}

			



	
	
	
	
