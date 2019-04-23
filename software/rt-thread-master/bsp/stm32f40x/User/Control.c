/*
 * Control.c
 *
 *  Created on: 2019��3��20��
 *      Author: zengwangfa
 *      Notes:  ��λ�ǿ��ơ���ȿ���
 */
#include "Control.h"
#include "PID.h"
#include <rtthread.h>
#include <stdlib.h>

float Yaw_Control = 0.0f;//Yaw���� ƫ������ 
float Yaw = 0.0f;

/* ԭ��
 *  N���� 0��/-0��
 *  W���� 90��
 *  S���� 180��/-180
 *  E���� -90��
*/
/** �涨   ��ʱ��
 *  N���� 0��
 *  W���� 90��
 *  S���� 180��
 *  E���� 270��
 */


void Angle_Control(void)
{
	
		if(Sensor.JY901.Euler.Yaw < 0) Yaw = (180+Sensor.JY901.Euler.Yaw) + 180;//�ǶȲ���
		if(Sensor.JY901.Euler.Yaw > 0) Yaw = Sensor.JY901.Euler.Yaw;            //�ǶȲ���
		Total_Controller.Yaw_Angle_Control.Expect = Yaw_Control;//ƫ�����ٶȻ�������ֱ����Դ��ң���������
		Total_Controller.Yaw_Angle_Control.FeedBack = Yaw;//ƫ���Ƿ���
		PID_Control_Yaw(&Total_Controller.Yaw_Angle_Control);//ƫ���Ƕȿ���
	

		//ƫ�����ٶȻ���������Դ��ƫ���Ƕȿ��������
		//Total_Controller.Yaw_Gyro_Control.Expect = Total_Controller.Yaw_Angle_Control.Control_OutPut;
	
}


void Gyro_Control()//���ٶȻ�
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

/*����е�ۡ���� ����yaw MSH���� */
static int yaw(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        rt_kprintf("Error! Proper Usage: RoboticArm_openvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }

		Yaw_Control = atoi(argv[1]);

		
_exit:
    return result;
}
MSH_CMD_EXPORT(yaw,ag: yaw 100);

