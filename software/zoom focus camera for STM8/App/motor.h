#ifndef __MOTOR_H
#define __MOTOR_H  

#define ZOOM_MOTOR  0   //����佹�������ID
#define FOCUS_MOTOR 1

/*���� �佹����ͷ ������Ϣ*/ 
#define ZOOM_A1 GPIO_PIN_2     //PA8��Ӧ���������A��   ZOOM  A+ PD2
#define ZOOM_A0 GPIO_PIN_3     //PA9��Ӧ���������B��   ZOOM  A- PD3
#define ZOOM_B1 GPIO_PIN_3    //PA10��Ӧ���������C��  ZOOM  B+  PA3
#define ZOOM_B0 GPIO_PIN_7    //PA11��Ӧ���������D��  ZOOM  B-  PC7

  

/*���� �۽�����ͷ ������Ϣ*/
#define FOCUS_STEP_PORT GPIOC   //��������Ŀ��ƶ˿�ΪA�˿�  
#define FOCUS_A1 GPIO_PIN_3	// FOCUS  A+   PC3
#define FOCUS_A0 GPIO_PIN_4	// FOCUS  A-   PC4
#define FOCUS_B1 GPIO_PIN_5	// FOCUS  B+   PC5
#define FOCUS_B0 GPIO_PIN_6	// FOCUS  B-   PC6

void STEP_Stop(void);
void STEP_Init(void);                      //����������ų�ʼ��

void SetMotor(unsigned char Motor_ID,unsigned char InputData);    //���ݴ��͵�����Inputdata���������������

void MotorNCircle(unsigned char Motor_ID,int n,bool position);    //nΪ�������ת����Ȧ����positionΪ��ת���߷�ת

void MotorNAngle(unsigned char Motor_ID,int n,bool position);    //nΪ�������ת���ĽǶ�Ϊn*120��positionΪ��ת���߷�ת
#endif
