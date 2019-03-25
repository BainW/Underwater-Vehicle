#define LOG_TAG       "flash"

#include "init.h"
#include <math.h>
#include <spi_flash.h>
#include <spi_flash_sfud.h>
#include <easyflash.h>

#include "PID.h"
#include "debug.h"
#include "flash.h"
#include "drv_ano.h"
#include "PropellerControl.h"

//FLASH��ʼ��ַ 
#define WQ_FLASH_ADDRESS    (0x0000) 	//  W25Q128 FLASH����ʼ��ַ

extern PropellerParamter_Type PropellerParamter;
extern ServoType RoboticArm,YunTai;
extern u8 debug_tool;

typedef struct
{
  u8 No_0xFF[PID_USE_NUM];
}PID_Parameter_Flag;

PID_Parameter_Flag PID_Parameter_Read_Flag;
float Parameter_Table[FLIGHT_PARAMETER_TABLE_NUM];


/* ��ȡ FLASH ��ͨ����*/
int Normal_Paramter_Init_With_Flash(void)
{
		ef_port_read(WQ_FLASH_ADDRESS+4*DEBUG_TOOL_A,(u32 *)(&debug_tool),4);		  
	
		ef_port_read(WQ_FLASH_ADDRESS+4*ROBOTIC_ARM_OPEN_VALUE_A,&RoboticArm.OpenValue,4);		// ��ַ
		ef_port_read(WQ_FLASH_ADDRESS+4*ROBOTIC_ARM_CLOSE_VALUE_A,&RoboticArm.CloseValue,4); // ��ַ
		ef_port_read(WQ_FLASH_ADDRESS+4*ROBOTIC_ARM_CURRENT_VALUE_A,&RoboticArm.CurrentValue,4); // ��ַ
	
		ef_port_read(WQ_FLASH_ADDRESS+4*YUNTAI_OPEN_VALUE_A,&YunTai.OpenValue,4);		// ��ַ
		ef_port_read(WQ_FLASH_ADDRESS+4*YUNTAI_CLOSE_VALUE_A,&YunTai.CloseValue,4); // ��ַ
		ef_port_read(WQ_FLASH_ADDRESS+4*YUNTAI_CURRENT_VALUE_A,&YunTai.CurrentValue,4); // ��ַ
	

	
		log_i("Flash_Read()");
		return 0;
	
}
INIT_APP_EXPORT(Normal_Paramter_Init_With_Flash);

/* FLASH ���� ����ֵ*/
void Flash_Update(void)
{
		ef_port_erase(WQ_FLASH_ADDRESS,4);	//�Ȳ���д  ������Ϊһ������4096 Byte
//------------------------------------------------------------------------------------------//
		ef_port_write(WQ_FLASH_ADDRESS+4*DEBUG_TOOL_A,(u32 *)(&debug_tool),4);		  
	
		ef_port_write(WQ_FLASH_ADDRESS+4*ROBOTIC_ARM_OPEN_VALUE_A,&RoboticArm.OpenValue,4);		// ��ַ
		ef_port_write(WQ_FLASH_ADDRESS+4*ROBOTIC_ARM_CLOSE_VALUE_A,&RoboticArm.CloseValue,4); // ��ַ
		ef_port_write(WQ_FLASH_ADDRESS+4*ROBOTIC_ARM_CURRENT_VALUE_A,&RoboticArm.CurrentValue,4); // ��ַ
	
		ef_port_write(WQ_FLASH_ADDRESS+4*YUNTAI_OPEN_VALUE_A,&YunTai.OpenValue,4);		// ��ַ
		ef_port_write(WQ_FLASH_ADDRESS+4*YUNTAI_CLOSE_VALUE_A,&YunTai.CloseValue,4); // ��ַ
		ef_port_write(WQ_FLASH_ADDRESS+4*YUNTAI_CURRENT_VALUE_A,&YunTai.CurrentValue,4); // ��ַ
	
}


/* list �����Ҫ���� */
void list_value(void)
{

		Normal_Paramter_Init_With_Flash();
		log_i	("variable  name          value");
    log_i("----------------------   ---------");
	  log_i("RoboticArm_OpenValue      %d",RoboticArm.OpenValue);
	  log_i("RoboticArm_CloseValue     %d",RoboticArm.CloseValue);
		log_i("RoboticArm_CurrentValue   %d",RoboticArm.CurrentValue);
		log_i("debug_tool                %s",debug_tool_name[debug_tool]);
    rt_kprintf("                         \n");
}
MSH_CMD_EXPORT(list_value,list some important values);


void ReadFlashParameterThree(u16 Label,float *ReadData1,float *ReadData2,float *ReadData3)
{
		ef_port_read(WQ_FLASH_ADDRESS+4*Label+0,(u32 *)ReadData1,4);		// ��ַ0
		ef_port_read(WQ_FLASH_ADDRESS+4*Label+4,(u32 *)ReadData1,4);		// ��ַ0
		ef_port_read(WQ_FLASH_ADDRESS+4*Label+8,(u32 *)ReadData1,4);		// ��ַ0
}



void Save_PID_Parameter(void)
{
		u8 i = 0;
		
		Flash_Update(); //��ͨ����һͬд�� �Է��������
		for(i = 0;i < PID_USE_NUM;i++){
				ef_port_write(WQ_FLASH_ADDRESS+12*(PID1_PARAMETER_KP+i*3)+0,(u32 *)&PID_Parameter[i].p,4);//��i+1��PID
				ef_port_write(WQ_FLASH_ADDRESS+12*(PID1_PARAMETER_KP+i*3)+4,(u32 *)&PID_Parameter[i].i,4);
				ef_port_write(WQ_FLASH_ADDRESS+12*(PID1_PARAMETER_KP+i*3)+8,(u32 *)&PID_Parameter[i].d,4);
			
		}
}

int PID_Paramter_Init_With_Flash()//��ʼ����ȡPID����
{
		u8 i = 0;
		Total_PID_Init();//��ʼ��PID������
		for(i = 0;i < PID_USE_NUM;i++){
				ef_port_read(WQ_FLASH_ADDRESS+12*(PID1_PARAMETER_KP+i*3)+0,(u32 *)&PID_Parameter[i].p,4);//��i+1��PID
				ef_port_read(WQ_FLASH_ADDRESS+12*(PID1_PARAMETER_KP+i*3)+4,(u32 *)&PID_Parameter[i].i,4);
				ef_port_read(WQ_FLASH_ADDRESS+12*(PID1_PARAMETER_KP+i*3)+8,(u32 *)&PID_Parameter[i].d,4);
			
				if(isnan(PID_Parameter[i].p)==0
						&&isnan(PID_Parameter[i].i)==0
							&&isnan(PID_Parameter[i].d)==0){ //�ж������Ƿ�isnan(not a number��Ч����)
						PID_Parameter_Read_Flag.No_0xFF[i]=0x01;
				 }
			
		}
		
		if(PID_Parameter_Read_Flag.No_0xFF[0]!=0x00
			 &&PID_Parameter_Read_Flag.No_0xFF[1]!=0x00
				 &&PID_Parameter_Read_Flag.No_0xFF[2]!=0x00
					 &&PID_Parameter_Read_Flag.No_0xFF[3]!=0x00
						 &&PID_Parameter_Read_Flag.No_0xFF[4]!=0x00
							 &&PID_Parameter_Read_Flag.No_0xFF[5]!=0x00
								 &&PID_Parameter_Read_Flag.No_0xFF[6]!=0x00
									 &&PID_Parameter_Read_Flag.No_0xFF[7]!=0x00)//Flash����������������PID����ֵ
		{
				Total_Controller.Roll_Gyro_Control.Kp=PID_Parameter[0].p;
				Total_Controller.Roll_Gyro_Control.Ki=PID_Parameter[0].i;
				Total_Controller.Roll_Gyro_Control.Kd=PID_Parameter[0].d;
		
				Total_Controller.Pitch_Gyro_Control.Kp=PID_Parameter[1].p;
				Total_Controller.Pitch_Gyro_Control.Ki=PID_Parameter[1].i;
				Total_Controller.Pitch_Gyro_Control.Kd=PID_Parameter[1].d;
					
				Total_Controller.Yaw_Gyro_Control.Kp=PID_Parameter[2].p;
				Total_Controller.Yaw_Gyro_Control.Ki=PID_Parameter[2].i;
				Total_Controller.Yaw_Gyro_Control.Kd=PID_Parameter[2].d;
						
				Total_Controller.Roll_Angle_Control.Kp=PID_Parameter[3].p;
				Total_Controller.Roll_Angle_Control.Ki=PID_Parameter[3].i;
				Total_Controller.Roll_Angle_Control.Kd=PID_Parameter[3].d;
				
				Total_Controller.Pitch_Angle_Control.Kp=PID_Parameter[4].p;
				Total_Controller.Pitch_Angle_Control.Ki=PID_Parameter[4].i;
				Total_Controller.Pitch_Angle_Control.Kd=PID_Parameter[4].d;

				Total_Controller.Yaw_Angle_Control.Kp=PID_Parameter[5].p;
				Total_Controller.Yaw_Angle_Control.Ki=PID_Parameter[5].i;
				Total_Controller.Yaw_Angle_Control.Kd=PID_Parameter[5].d;
				
				Total_Controller.High_Speed_Control.Kp=PID_Parameter[6].p;
				Total_Controller.High_Speed_Control.Ki=PID_Parameter[6].i;
				Total_Controller.High_Speed_Control.Kd=PID_Parameter[6].d;
				
				Total_Controller.High_Position_Control.Kp=PID_Parameter[7].p;
				Total_Controller.High_Position_Control.Ki=PID_Parameter[7].i;
				Total_Controller.High_Position_Control.Kd=PID_Parameter[7].d;
				
		}
		return 0;
}
INIT_APP_EXPORT(PID_Paramter_Init_With_Flash);


