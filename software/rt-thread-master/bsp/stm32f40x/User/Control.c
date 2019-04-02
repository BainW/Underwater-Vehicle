#include "Control.h"
#include "PID.h"
#include "gyroscope.h"

short int Yaw_Control = 0;//Yaw����y��ƫ��

void Angle_Control(void)
{
		if(0 == Yaw_Control)
		{
				Total_Controller.Yaw_Angle_Control.FeedBack = JY901.Euler.Yaw;//ƫ���Ƿ���
				PID_Control_Yaw(&Total_Controller.Yaw_Angle_Control);//ƫ���Ƕȿ���
				//ƫ�����ٶȻ���������Դ��ƫ���Ƕȿ��������
				Total_Controller.Yaw_Gyro_Control.Expect = Total_Controller.Yaw_Angle_Control.Control_OutPut;
		}
	  else//����ƫ������˺�ֻ�����ڻ����ٶȿ���
		{
				Total_Controller.Yaw_Angle_Control.Expect = 0;//ƫ����������0,�����нǶȿ���
				Total_Controller.Yaw_Gyro_Control.Expect = Yaw_Control;//ƫ�����ٶȻ�������ֱ����Դ��ң���������
		}
}


void Gyro_Control()//���ٶȻ�
{

  //ƫ����ǰ������
  //Total_Controller.Yaw_Gyro_Control.FeedBack=Yaw_Gyro;


//  PID_Control_Div_LPF(&Total_Controller.Yaw_Gyro_Control);
//  Yaw_Gyro_Control_Expect_Delta=1000*(Total_Controller.Yaw_Gyro_Control.Expect-Last_Yaw_Gyro_Control_Expect)
//    /Total_Controller.Yaw_Gyro_Control.PID_Controller_Dt.Time_Delta;
//  //**************************ƫ����ǰ������**********************************
//  Total_Controller.Yaw_Gyro_Control.Control_OutPut+=Yaw_Feedforward_Kp*Total_Controller.Yaw_Gyro_Control.Expect
//    +Yaw_Feedforward_Kd*Yaw_Gyro_Control_Expect_Delta;//ƫ����ǰ������
//  Total_Controller.Yaw_Gyro_Control.Control_OutPut=constrain_float(Total_Controller.Yaw_Gyro_Control.Control_OutPut,
//                                                                   -Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit,
//                                                                   Total_Controller.Yaw_Gyro_Control.Control_OutPut_Limit);
//  Last_Yaw_Gyro_Control_Expect=Total_Controller.Yaw_Gyro_Control.Expect;
//  

}

