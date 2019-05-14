#include "bsp_timer.h"
#include "motor.h"
#include "bsp_timer.h"

//unsigned char F_Rotation[8]={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09} ;  //��˫8�ķ�ת�� �������ಽ�������
//unsigned char R_Rotation[8]={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08} ;  //��˫8����ת��

unsigned char F_Rotation[4]={0x09,0x0A,0x06,0x05} ;  //4����ת��  Forward
unsigned char R_Rotation[4]={0x05,0x06,0x0A,0x09} ;  //4�ķ�ת��  Reverse


/******************************************************************************
* Name: 	 STEP_Init
*
* Desc: 	 ��ʼ�������������߲����������
* Param:   ��
* Return:  ��
* Note:    ������Ϊ �佹���۽����ƵĲ���������ų�ʼ��
* Author:  Dwfish
* -------------------------------------
* Log: 	 2018/12/27, Create this function by Dwfish
 ******************************************************************************/
void STEP_Init()
{
     CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //HSI = 16M (1��Ƶ)  //ʱ������
     
     GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)ZOOM_A1, GPIO_MODE_OUT_PP_LOW_FAST); //��ʼ�������������߲����������
     GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)ZOOM_A0, GPIO_MODE_OUT_PP_LOW_FAST);
     GPIO_Init(GPIOA, (GPIO_Pin_TypeDef)ZOOM_B1, GPIO_MODE_OUT_PP_LOW_FAST);
     GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)ZOOM_B0, GPIO_MODE_OUT_PP_LOW_FAST);
     
     GPIO_Init(FOCUS_STEP_PORT, (GPIO_Pin_TypeDef)FOCUS_A1, GPIO_MODE_OUT_PP_LOW_FAST);
     GPIO_Init(FOCUS_STEP_PORT, (GPIO_Pin_TypeDef)FOCUS_A0, GPIO_MODE_OUT_PP_LOW_FAST);
     GPIO_Init(FOCUS_STEP_PORT, (GPIO_Pin_TypeDef)FOCUS_B1, GPIO_MODE_OUT_PP_LOW_FAST);
     GPIO_Init(FOCUS_STEP_PORT, (GPIO_Pin_TypeDef)FOCUS_B0, GPIO_MODE_OUT_PP_LOW_FAST);
     
     GPIO_WriteLow(GPIOD,ZOOM_A1);   //�����
     GPIO_WriteLow(GPIOD,ZOOM_A0);
     GPIO_WriteLow(GPIOA,ZOOM_B1);
     GPIO_WriteLow(GPIOC,ZOOM_B0);
     
     GPIO_WriteLow(FOCUS_STEP_PORT,FOCUS_A1);
     GPIO_WriteLow(FOCUS_STEP_PORT,FOCUS_A0);
     GPIO_WriteLow(FOCUS_STEP_PORT,FOCUS_B1);
     GPIO_WriteLow(FOCUS_STEP_PORT,FOCUS_B0);
     
}

/******************************************************************************
* Name: 	 SetMotor
*
* Desc: 	 ʹ�������ת��
* Param:   ������:Motor_ID[0Ϊ�佹��1Ϊ�۽�]     ����:InputDataѡ����λʹ��
* Return:  ��
* Note:    �����Լ�����Ľ��ı������Ӧ����
* Author:  Dwfish
* -------------------------------------
* Log: 	 2018/12/27, Create this function by Dwfish
 ******************************************************************************/
void SetMotor(unsigned char Motor_ID,unsigned char InputData)    //���ݴ��͵�����Inputdata����PB12-PB15�������
{
    if(InputData & 0x08){
				switch(Motor_ID){
					case ZOOM_MOTOR : GPIO_WriteHigh(GPIOD, ZOOM_A1 );	break;
					case FOCUS_MOTOR : GPIO_WriteHigh(FOCUS_STEP_PORT, FOCUS_A1 );break;
				}					
    }
    else {
		  	switch(Motor_ID){
					case ZOOM_MOTOR : GPIO_WriteLow(GPIOD, ZOOM_A1 );	break;
					case FOCUS_MOTOR : GPIO_WriteLow(FOCUS_STEP_PORT, FOCUS_A1 );break;
				}	
    }
	 
    if(InputData & 0x04){
				switch(Motor_ID){
					case ZOOM_MOTOR : GPIO_WriteHigh(GPIOD, ZOOM_A0 );	break;
					case FOCUS_MOTOR : GPIO_WriteHigh(FOCUS_STEP_PORT, FOCUS_A0 );break;
				}	  
    }
   else{
		  	switch(Motor_ID){
					case ZOOM_MOTOR : GPIO_WriteLow(GPIOD, ZOOM_A0 );	break;
					case FOCUS_MOTOR : GPIO_WriteLow(FOCUS_STEP_PORT, FOCUS_A0 );break;
				}	
    }
		
    if(InputData & 0x02){
				switch(Motor_ID){
					case ZOOM_MOTOR : GPIO_WriteHigh(GPIOA, ZOOM_B1 );	break;
					case FOCUS_MOTOR : GPIO_WriteHigh(FOCUS_STEP_PORT, FOCUS_B1 );break;
				}	   
    }
    else{
		  	switch(Motor_ID){
					case ZOOM_MOTOR : GPIO_WriteLow(GPIOA, ZOOM_B1 );	break;
					case FOCUS_MOTOR : GPIO_WriteLow(FOCUS_STEP_PORT, FOCUS_B1 );break;
				}	
    }
    if(InputData & 0x01){
				switch(Motor_ID){
					case ZOOM_MOTOR : GPIO_WriteHigh(GPIOC, ZOOM_B0 );	break;
					case FOCUS_MOTOR : GPIO_WriteHigh(FOCUS_STEP_PORT, FOCUS_B0 );break;
				}	  
    }
    else{
				switch(Motor_ID){
					case ZOOM_MOTOR : GPIO_WriteLow(GPIOC, ZOOM_B0 );	break;
					case FOCUS_MOTOR : GPIO_WriteLow(FOCUS_STEP_PORT, FOCUS_B0 );break;
				}	
    }
}

/******************************************************************************
* Name: 	 MotorNCircle
*
* Desc: 	 ���ת�� N Ȧ
* Param:   ������:Motor_ID     ת��nȦ     ������position��0��1����
* Return:  ��
* Note:    ��
* Author:  Dwfish
* -------------------------------------
* Log: 	 2018/12/27, Create this function by Dwfish
 ******************************************************************************/
void MotorNCircle(unsigned char Motor_ID,int n,bool position)
{
   int i=0;
   int j=0;
   int k=0;
   for(j=0;j<n;j++)
   {
      for(i=0;i<4*128;i++)   //������4*128
		 {
          for(k=0;k<4;k++)
				 {
              if(TRUE == position)
                 SetMotor(Motor_ID,R_Rotation[k]);  //1��ת
             else
                 SetMotor(Motor_ID,F_Rotation[k]);    
                TIMDelay_N10us(100);
          }
      }
    }
}

/******************************************************************************
* Name: 	 MotorNAngle
*
* Desc: 	 ���ת�� N ��
* Param:   ������:Motor_ID     ת��n��     ������position��0��1����
* Return:  ��
* Note:    �ڶ���forѭ����i����Ӧ��i<1��"1"��������Ҫ�����Լ�ʵ�ʲ����������
* Author:  Dwfish
* -------------------------------------
* Log: 	 2018/12/27, Create this function by Dwfish
 ******************************************************************************/

void MotorNAngle(unsigned char Motor_ID,int n,bool position) //nΪ�������ת���ĽǶ�Ϊn*120��positionΪ��ת���߷�ת
{
   int i=0;
   int j=0;
   int k=0;
   for(j=0;j<n;j++)
   {
      for(i=0;i<1;i++)     
      {
          for(k=0;k<4;k++)   
          {
              if(TRUE == position)
                 SetMotor(Motor_ID,R_Rotation[k]);  
              else
                 SetMotor(Motor_ID,F_Rotation[k]);  
             	TIMDelay_N10us(100);      
          }
      }
    }
}
