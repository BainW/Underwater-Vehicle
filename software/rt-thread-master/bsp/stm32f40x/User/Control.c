/*
 * ControlCmd.c
 *
 *  Created on: 2019��3��20��
 *      Author: zengwangfa
 *      Notes:  ��λ�ǿ��ơ���ȿ���
 */
 
#define LOG_TAG "Control"

#include <rtthread.h>
#include <elog.h>
#include <stdlib.h>
#include <math.h>

#include "Control.h"
#include "PID.h"
#include "RC_Data.h"

#include "focus.h"
#include "led.h"
#include "servo.h"
#include "PropellerControl.h"
#include "propeller.h"


float Yaw_Control = 0.0f;//Yaw���� ƫ������ 
float Yaw = 0.0f;
int16 Force1 = 0;
int16 Force2 = 0;

extern int16 Power;
void Convert_RockerValue(Rocker_Type *rc) //��ȡҡ��ֵ
{

		rc->X = ControlCmd.Move - 128; 			  //ҡ��ֵ�任��X��ҡ��ֵ -127 ~ +127
		rc->Y = ControlCmd.Translation- 128  ;//					  Y��ҡ��ֵ -127 ~ +127
		
		rc->Angle = Rad2Deg(atan2(rc->X,rc->Y));
		if(rc->Angle < 0){rc->Angle += 360;} //�Ƕȱ任 
																				 /* �Լ����궨�� �Ƕ�˳�� 0~360��*/ 	
		rc->Force = sqrt(rc->X*rc->X+rc->Y*rc->Y);	//�����б��
		
		Force1 = (sqrt(2)/2)*(rc->X + rc->Y);
		Force2 = (sqrt(2)/2)*(rc->X - rc->Y);		
		
		//�ƶ�һ�� -1  1   -1  -1                    //AUV       
		/* ����F = �ƽ�������*����ϵ��*ҡ�˴�˳̶� + ƫ��ֵ */   //ControlCmd.Power
		PropellerPower.leftUp =    (PropellerDir.leftUp    * (Power) * ( Force1) )/70 + PropellerError.leftUp;  //PowerΪ�ƽ���ϵ�� 0~300%
		PropellerPower.rightUp =   (PropellerDir.rightUp   * (Power) * ( Force2) )/70 + PropellerError.rightUp;  //����70Ϊ   128(ҡ�˴�����̶�)*255(��λ���Ķ���ϵ��)/70 = 466��500(�ƽ��������)
		PropellerPower.leftDown =  (PropellerDir.leftDown  * (Power) * ( Force2) )/70 + PropellerError.leftDown ; 
		PropellerPower.rightDown = (PropellerDir.rightDown * (Power) * ( Force1) )/70 + PropellerError.rightDown;
		
		if(rc->Angle < 180 && PropellerPower.rightUp> 10){//�� ��תʱ����������10
				PropellerPower.rightUp = PropellerPower.rightUp -10; //�����ƽ��� ���ڷ���  ��Ҫ�������ⲹ��
		}
		else if(rc->Angle > 180 && PropellerPower.rightUp < -10){//��תʱ
				PropellerPower.rightUp = PropellerPower.rightUp - 10;
		}
		
}







/**
  * @brief  highSpeed Devices_Control(�����豸����)
  * @param  None
  * @retval None
  * @notice 
  */
void control_highSpeed_thread_entry(void *parameter)//���ٿ����߳�
{
		
		rt_thread_mdelay(5000);//�ȴ��ⲿ�豸��ʼ���ɹ�
		while(1)
		{
				Control_Cmd_Get(&ControlCmd); //���������ȡ ������λ������������ڴˡ�Important��
				Convert_RockerValue(&Rocker); //ң������ ת��  Ϊ�ƽ�������

				if(UNLOCK == ControlCmd.All_Lock){ //�������
						Focus_Zoom_Camera(&ControlCmd.Focus);//�佹�۽�����ͷ����
						Depth_Control();     //��ȿ���
						
				}
				Propeller_Control(); //�ƽ�������

				rt_thread_mdelay(10);
		}

}

/**
  * @brief  lowSpeed Devices_Control(�����豸����)
  * @param  None
  * @retval None
  * @notice 
  */
void control_lowSpeed_thread_entry(void *parameter)//���ٿ����߳�
{

		rt_thread_mdelay(5000);//�ȴ��ⲿ�豸��ʼ���ɹ�
		
		while(1)
		{

				Light_Control(&ControlCmd.Light);  //̽�յƿ���
				YunTai_Control(&ControlCmd.Yuntai); //��̨����
				RoboticArm_Control(&ControlCmd.Arm);//��е�ۿ���	
			
				rt_thread_mdelay(20);
		}
}



int control_thread_init(void)
{
		rt_thread_t control_lowSpeed_tid;
		rt_thread_t control_highSpeed_tid;
		/*������̬�߳�*/
    control_lowSpeed_tid = rt_thread_create("control_low",//�߳�����
                    control_lowSpeed_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

			/*������̬�߳�*/
    control_highSpeed_tid = rt_thread_create("control_high",//�߳�����
                    control_highSpeed_thread_entry,				 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    2048,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    10,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms
	
    if (control_lowSpeed_tid != RT_NULL && control_highSpeed_tid != RT_NULL  ){
				rt_thread_startup(control_lowSpeed_tid);
				rt_thread_startup(control_highSpeed_tid);
				log_i("Control_Init()");
		}
		else {
				log_e("Control Error!");
		}
		return 0;
}
INIT_APP_EXPORT(control_thread_init);





void Angle_Control(void)
{
	
		if(Sensor.JY901.Euler.Yaw < 0) Yaw = (180+Sensor.JY901.Euler.Yaw) + 180;//�ǶȲ���
		if(Sensor.JY901.Euler.Yaw > 0) Yaw = (float)Sensor.JY901.Euler.Yaw;            //�ǶȲ���
		Total_Controller.Yaw_Angle_Control.Expect = (float)Yaw_Control;//ƫ�����ٶȻ�������ֱ����Դ��ң���������
		Total_Controller.Yaw_Angle_Control.FeedBack = (float)Yaw;//ƫ���Ƿ���
	
		PID_Control_Yaw(&Total_Controller.Yaw_Angle_Control);//ƫ���Ƕȿ���
	

		//ƫ�����ٶȻ���������Դ��ƫ���Ƕȿ��������
		//Total_Controller.Yaw_Gyro_Control.Expect = Total_Controller.Yaw_Angle_Control.Control_OutPut;
}



void Depth_Control(void)
{
		
		Total_Controller.High_Position_Control.Expect = (float)Expect_Depth; //���������ң��������
		Total_Controller.High_Position_Control.FeedBack = (float)Sensor.Depth;  //��ǰ��ȷ���
		PID_Control(&Total_Controller.High_Position_Control);//�߶�λ�ÿ�����
	
		robot_upDown(Total_Controller.High_Position_Control.Control_OutPut);		//��ֱ�ƽ�������
}



void Gyro_Control(void)//���ٶȻ�
{

//  	ƫ����ǰ������
//  	Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro;


//		PID_Control_Div_LPF(&Total_Controller.Yaw_Gyro_Control);
//		Yaw_Gyro_Control_Expect_Delta=1000*(Total_Controller.Yaw_Gyro_Control.Expect-Last_Yaw_Gyro_Control_Expect)
//			/Total_Controller.Yaw_Gyro_Control.PID_Controller_Dt.Time_Delta;
//		//**************************ƫ����ǰ������**********************************
//		Total_Controller.Yaw_Gyro_Control.Control_OutPut+=Yaw_Feedforward_Kp*Total_Controller.Yaw_Gyro_Control.Expect
//			+Yaw_Feedforward_Kd*Yaw_Gyro_Control_Expect_Delta;//ƫ����ǰ������
//		Total_Controller.Yaw_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Yaw_Gyro_Control.Control_OutPut,
//																																		 -Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit,
//																																		 Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit);
//		Last_Yaw_Gyro_Control_Expect=Total_Controller.Yaw_Gyro_Control.Expect;
//		

}

/*����� ������yaw MSH���� */
static int depth(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1])<100){
				Expect_Depth = atoi(argv[1]);
		}
		else {
				log_e("Error! The  value is out of range!");
		}

		
_exit:
    return result;
}
MSH_CMD_EXPORT(depth,ag: depth 10);



/*����е�ۡ���� ����yaw MSH���� */
static int yaw(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		Yaw_Control = atoi(argv[1]); //ASCII to Integer
		
_exit:
    return result;
}
MSH_CMD_EXPORT(yaw,ag: yaw 100);

