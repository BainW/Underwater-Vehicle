#include "PID.h"
#include <rthw.h>
#include "flash.h"
#include "PID.h"
#include "drv_ano.h"

AllControler Total_Controller; //�ܿ�����PID



/*
1ƫ���޷���־��  2�����޷���־��3���ַ����־��   4������
5����            6ƫ�        7�ϴ�ƫ�       8ƫ���޷�ֵ��
9���ַ���ƫ��ֵ��10����ֵ       11�����޷�ֵ��    12���Ʋ���Kp��
13���Ʋ���Ki��   14���Ʋ���Kd�� 15�������������  16�ϴο����������
17������޷��ȣ� 18����ֿ���ʱ�Ļ�������
*/
const float Control_Unit[18][20]=
{
  /*                                          Kp      Ki      Kd            */
  /*1  2  3  4  5  6   7  8    9   10   11    12      13      14     15  16  17   18*/
   //��ӯ������20Aˢ�̼�F330
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 200,  0.50   ,2.5000  ,1.80  ,0  ,0 , 500,  1 ,  1 ,  1 },//Roll_Gyro;������ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 200,  0.75   ,3.5000  ,1.80  ,0  ,0 , 500,  1 ,  1 ,  1 },//Pitch_Gyro;�������ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,250 ,0  ,0 , 100,  1.00   ,0.5000  ,0.00  ,0  ,0 , 300,  1 ,  1 ,  1 },//Yaw_Gyro;ƫ�����ٶ�
		
	{1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   4.00   ,0.0000  ,0.00  ,0  ,0 , 300,  1 ,  1 ,  1 },//Pitch_Angle;�����Ƕ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 5.00   ,0.0000  ,0.00  ,0  ,0 , 300,  1 ,  1 ,  1 },//Yaw_Angle;ƫ����		
	{1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   4.00   ,0.0000  ,0.00  ,0  ,0 , 300,  1 ,  1 ,  1 },//Roll_Angle;�����
		
  /*                                         Kp        Ki      Kd            */
  /*1  2  3  4  5  6   7  8   9   10   11    12        13      14  15  16  17    18*/
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,400 ,0  ,0 ,500 ,  5.0     ,0.300   ,0.1  ,0  ,0 ,500,  1 ,  1 ,  1 },//High_Speed;���������ٶ�
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,200 ,0  ,0 ,100 ,  0.5     ,0.000   ,0    ,0  ,0 ,400,  1 ,  1 ,  1 },//High_Position;���θ߶�λ��



};


void PID_Init(PID_Controler *Controler,Controler_Label Label)
{
		Controler->Err_Limit_Flag=(u8)(Control_Unit[Label][0]);//1ƫ���޷���־
		Controler->Integrate_Limit_Flag=(u8)(Control_Unit[Label][1]);//2�����޷���־
		Controler->Integrate_Separation_Flag=(u8)(Control_Unit[Label][2]);//3���ַ����־
		Controler->Expect=Control_Unit[Label][3];//4����
		Controler->FeedBack=Control_Unit[Label][4];//5����ֵ
		Controler->Err=Control_Unit[Label][5];//6ƫ��
		Controler->Last_Err=Control_Unit[Label][6];//7�ϴ�ƫ��
		Controler->Err_Max=Control_Unit[Label][7];//8ƫ���޷�ֵ
		Controler->Integrate_Separation_Err=Control_Unit[Label][8];//9���ַ���ƫ��ֵ
		Controler->Integrate=Control_Unit[Label][9];//10����ֵ
		Controler->Integrate_Max=Control_Unit[Label][10];//11�����޷�ֵ
		Controler->Kp=Control_Unit[Label][11];//12���Ʋ���Kp
		Controler->Ki=Control_Unit[Label][12];//13���Ʋ���Ki
		Controler->Kd=Control_Unit[Label][13];//14���Ʋ���Ki
		Controler->Control_OutPut=Control_Unit[Label][14];//15�����������
		Controler->Last_Control_OutPut=Control_Unit[Label][15];//16�ϴο����������
		Controler->Control_OutPut_Limit=Control_Unit[Label][16];//17�ϴο����������
		Controler->Scale_Kp=Control_Unit[Label][17];
		Controler->Scale_Ki=Control_Unit[Label][18];
		Controler->Scale_Kd=Control_Unit[Label][19];  
}


void Total_PID_Init(void)
{
		PID_Init(&Total_Controller.Pitch_Angle_Control,Pitch_Angle_Controler);
		PID_Init(&Total_Controller.Pitch_Gyro_Control,Pitch_Gyro_Controler);
		PID_Init(&Total_Controller.Roll_Angle_Control,Roll_Angle_Controler);
		PID_Init(&Total_Controller.Roll_Gyro_Control,Roll_Gyro_Controler);
		PID_Init(&Total_Controller.Yaw_Angle_Control,Yaw_Angle_Controler);
		PID_Init(&Total_Controller.Yaw_Gyro_Control,Yaw_Gyro_Controler);
		PID_Init(&Total_Controller.High_Position_Control,High_Position_Controler);
		PID_Init(&Total_Controller.High_Speed_Control,High_Speed_Controler);

}







