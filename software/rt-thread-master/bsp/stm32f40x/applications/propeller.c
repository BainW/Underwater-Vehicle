/*
 * servo.c
 *
 *  Created on: 2019年2月30日
 *      Author: zengwangfa
 *      Notes:  推进器设备
 */
 #define LOG_TAG    "propeller"
 
 
#include <stdlib.h>
#include <string.h>
#include "sys.h"
#include "propeller.h"
#include <elog.h>
#include <rtthread.h>

#include "flash.h"
#include "rc_data.h"
#include "drv_pwm.h"
#include "DeviceThread.h"

/*----------------------- Variable Declarations -----------------------------*/

uint8 Propeller_Init_Flag = 0;

PropellerParameter_Type PropellerParameter = {//初始化推进器参数值【限幅】
		 .PowerMax = 1750,//正向最大值
		 .PowerMed = 1500,//中值
		 .PowerMin = 1250,//反向最小值【反向推力最大】
	
	   .PowerDeadband = 10	//死区值
}; 

PropellerDir_Type    PropellerDir = {1,1,1,1,1,1};     //推进器方向，默认为1
PropellerPower_Type  PropellerPower = {0,0,0,0,0,0,0}; //推进器推力控制器
PropellerError_Type  PropellerError = {0,0,0,0,0,0};   //推进器偏差值

PropellerPower_Type power_test; //调试用的变量

PropellerError_Type Forward   = {0,0,0,0,0,0};
PropellerError_Type Retreat   = {0,0,0,0,0,0};
PropellerError_Type TurnLeft  = {0,0,0,0,0,0};
PropellerError_Type TurnRight = {0,0,0,0,0,0};
Adjust_Parameter AdjustParameter = {1,1,1,1};

extern int16 PowerPercent;
extern int Extractor_Value;//吸取器推进器的值
/*----------------------- Function Implement --------------------------------*/

/*******************************************
* 函 数 名：Propeller_Init
* 功    能：推进器的初始化
* 输入参数：none
* 返 回 值：none
* 注    意：初始化流程：
*           1,接线,上电，哔-哔-哔三声,表示开机正常
*           2,给电调2ms或1ms最高转速信号,哔一声
*           3,给电调1.5ms停转信号,哔一声
*           4,初始化完成，可以开始控制
********************************************/
void Propeller_Init(void)//这边都需要经过限幅在给定  原先为2000->1500
{

	
		rt_thread_mdelay(3000);//等待外部设备初始化成功
	
		TIM1_PWM_CH1_E9 (PropellerPower_Max);  		//最高转速信号   	水平推进器1号  右上	 E9	
		TIM1_PWM_CH2_E11(PropellerPower_Max);  		//最高转速信号    水平推进器2号  左下	 E11
		TIM1_PWM_CH3_E13(PropellerPower_Max); 	  //最高转速信号    水平推进器3号  左上   E13
		TIM1_PWM_CH4_E14(PropellerPower_Max);  		//最高转速信号    水平推进器4号  右下   E14
	
		TIM4_PWM_CH1_D12(PropellerPower_Max); 	 	//最高转速信号  	垂直推进器1号  左中   D12
		TIM4_PWM_CH2_D13(PropellerPower_Max);	    //最高转速信号  	垂直推进器2号  右中   D13
	
		TIM3_PWM_CH3_B0(PropellerPower_Max);	    //最高转速信号  	吸取器   B0
	
	/**/
		//TIM4_PWM_CH3_D14(PropellerPower_Max);	    //最高转速信号  	吸取器   B0
	/**/
		rt_thread_mdelay(2000);  //2s

		TIM1_PWM_CH1_E9 (PropellerPower_Med);			//停转信号
		TIM1_PWM_CH2_E11(PropellerPower_Med);			//停转信号
		TIM1_PWM_CH3_E13(PropellerPower_Med);			//停转信号
		TIM1_PWM_CH4_E14(PropellerPower_Med);			//停转信号
	
		TIM4_PWM_CH1_D12(PropellerPower_Med);		  //停转信号
		TIM4_PWM_CH2_D13(PropellerPower_Med);		  //停转信号
		
		TIM3_PWM_CH3_B0(PropellerPower_Med);	    //停转信号  	吸取器   B0
		
		TIM4_PWM_CH3_D14(1500);		//机械臂中值 1000~2000
		TIM4_PWM_CH4_D15(2000);		//云台中值
		
		rt_thread_mdelay(1000);  //1s
		
		Propeller_Init_Flag = 1;
		log_i("Propeller_init()");


}


void PWM_Update(PropellerPower_Type* propeller)
{	
		power_test.rightUp     = PropellerPower_Med + propeller->rightUp;
		power_test.leftDown    = PropellerPower_Med + propeller->leftDown;
		power_test.leftUp      = PropellerPower_Med + propeller->leftUp;
		power_test.rightDown   = PropellerPower_Med + propeller->rightDown;
		
		power_test.leftMiddle  = PropellerPower_Med + propeller->leftMiddle;
		power_test.rightMiddle = PropellerPower_Med + propeller->rightMiddle;
	
		if(1 == Propeller_Init_Flag){
				
				TIM1_PWM_CH1_E9 (power_test.rightUp);     //右上	 E9	
				TIM1_PWM_CH2_E11(power_test.leftDown);    //左下	 E11
				TIM1_PWM_CH3_E13(power_test.leftUp); 	    //左上   E13
				TIM1_PWM_CH4_E14(power_test.rightDown);   //右下   E14
			
				TIM4_PWM_CH1_D12(power_test.leftMiddle);  //左中   D12
				TIM4_PWM_CH2_D13(power_test.rightMiddle); //右中   D13
			

			
		}

}



int power_value = 1500;
/**
  * @brief  Extractor_Control(吸取器控制)
  * @param  控制指令 0x00：不动作  0x01：吸取  0x02：关闭
  * @retval None
  * @notice 
  */
void Extractor_Control(uint8 *action)
{

		switch(*action)
		{
				case 0x01:power_value = Extractor_Value; //设定吸力值
									break;
				case 0x02:power_value = PropellerPower_Med; //推进器中值 停转
									break;
				default:break;
		}

		//TIM4_PWM_CH3_D14(power_value);
}


PropellerPower_Type power_test_msh; //调试用的变量

/*【推进器】 修改 【正向最大值】MSH方法 */
static int Propoller_Test(int argc, char **argv)
{

    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: Propoller_Test <0~100>");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 100){
				if(1 == Propeller_Init_Flag){

						power_test_msh.rightUp     = PropellerPower_Med + atoi(argv[1]);
						power_test_msh.leftDown    = PropellerPower_Med + atoi(argv[1]);
						power_test_msh.leftUp      = PropellerPower_Med + atoi(argv[1]);
						power_test_msh.rightDown   = PropellerPower_Med + atoi(argv[1]);
						
						power_test_msh.leftMiddle  = PropellerPower_Med + atoi(argv[1]);
						power_test_msh.rightMiddle = PropellerPower_Med + atoi(argv[1]);
					
				}
				log_i("Current propeller power:  %d",atoi(argv[1]) );
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(Propoller_Test,ag: Propoller_Test <0~100>);



/*【推进器】 修改 【正向最大值】MSH方法 */
static int propeller_maxvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_maxvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 2000){
				PropellerParameter.PowerMax = atoi(argv[1]);
				Flash_Update();
				log_i("Current propeller max_value_set:  %d",PropellerParameter.PowerMax);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_maxvalue_set,ag: propeller set 1600);


/*【推进器】 修改 【正向最大值】MSH方法 */
static int propeller_medvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_medvalue_set 1500");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 1500){
				PropellerParameter.PowerMed = atoi(argv[1]);
				Flash_Update();
				log_i("Current propeller med_value_set:  %d",PropellerParameter.PowerMed);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_medvalue_set,ag: propeller set 1500);

/*【推进器】 修改 【正向最大值】MSH方法 */
static int propeller_minvalue_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: propeller_minvalue_set 1600");
				result = -RT_ERROR;
        goto _exit;
    }
		if(atoi(argv[1]) <= 1500){
				PropellerParameter.PowerMin = atoi(argv[1]);
				Flash_Update();
				log_i("Current propeller min_value_set:  %d",PropellerParameter.PowerMin);
		}
		
		else {
				log_e("Error! The value is out of range!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_minvalue_set,ag: propeller set 1200);



/*【推进器】 修改 【方向】MSH方法 */
static int propeller_dir_set(int argc, char **argv) //只能是 -1 or 1
{
    int result = 0;
    if (argc != 7){ //6个推进器
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");   //其标志只能是 1  or  -1 
        log_e("Error! Proper Usage: propeller_dir_set <1 1 1 1 1 1>  ");
				result = -RT_ERROR;
        goto _exit;
    }
		
		if(abs(atoi(argv[1])) == 1 && abs(atoi(argv[2])) == 1  && abs(atoi(argv[3])) == 1  && \
			 abs(atoi(argv[4])) == 1  && abs(atoi(argv[5])) == 1  && abs(atoi(argv[6])) == 1  ) {
				 
				PropellerDir.rightUp     = atoi(argv[1]);
				PropellerDir.leftDown    = atoi(argv[2]);
				PropellerDir.leftUp      = atoi(argv[3]);
				PropellerDir.rightDown   = atoi(argv[4]);
				PropellerDir.leftMiddle  = atoi(argv[5]);
				PropellerDir.rightMiddle = atoi(argv[6]);
				 
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");   //其标志只能是 1  or  -1 
				log_i("Propeller:    %d           %d          %d          %d            %d             %d",\
				 PropellerDir.rightUp,PropellerDir.leftDown,PropellerDir.leftUp,PropellerDir.rightDown,PropellerDir.leftMiddle ,PropellerDir.rightMiddle);
				Flash_Update();//FLASH更新
				rt_kprintf("\n");

		}
		
		else {
				log_e("Error! Input Error!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_dir_set,propeller <1 1 1 1 1 1>);


/*【推进器】 修改 【动力】MSH方法 */
static int propeller_power_set(int argc, char **argv) //只能是 0~3.0f
{
    int result = 0;
    if (argc != 2){ //6个推进器
        log_e("Error! Proper Usage: propeller_power_set <0~300> % ");
				result = -RT_ERROR;
        goto _exit;
    }
		
	  if( atoi(argv[1]) >=0 && atoi(argv[1]) <=300  ) {
				 
				PowerPercent = atoi(argv[1]); //百分制
				Flash_Update();

				log_i("Propeller_Power: %d %%",PowerPercent);
		}
		
		else {
				log_e("Error! Input Error!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_power_set,propeller_power_set <0~300> %);



/*【吸取器】推进器 修改 【推力】 MSH方法 */
static int extractor_value_set(int argc, char **argv)
{
    int result = 0;
    if (argc != 2){
        log_e("Error! Proper Usage: extractor_value_set <1000~2000>");
				result = -RT_ERROR;
        goto _exit;
    }

		if(atoi(argv[1]) <= 2000 ){		
				Extractor_Value = atoi(argv[1]);
				Flash_Update();
				log_i("Write_Successed! extractor_value  %d",Extractor_Value);
		}
		else {
				log_e("Error! The value is out of range!");
		}

_exit:
    return result;
}
MSH_CMD_EXPORT(extractor_value_set,ag: extractor_value_set <1000~2000>);



uint8 is_in_range(short value)
{
		return abs(value) < 100?1:0;
}

/*【推进器】 修改 【偏差值】MSH方法 */
static int propeller_error_set(int argc, char **argv){ 
    int result = 0;
    if (argc != 7){ //6个推进器
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");   //其标志只能是 1  or  -1 
        log_e("Error! Proper Usage: propeller_dir_set <-100~+100>  ");
				result = -RT_ERROR;
        goto _exit;
    }
		
		if(is_in_range(atoi(argv[1])) && is_in_range(atoi(argv[2]))  && is_in_range(atoi(argv[3]))   && \
			 is_in_range(atoi(argv[4])) && is_in_range(atoi(argv[5])) && is_in_range(atoi(argv[6]))  ) {
				 
				PropellerError.rightUp     = atoi(argv[1]);
				PropellerError.leftDown    = atoi(argv[2]);
				PropellerError.leftUp      = atoi(argv[3]);
				PropellerError.rightDown   = atoi(argv[4]);
				PropellerError.leftMiddle  = atoi(argv[5]);
				PropellerError.rightMiddle = atoi(argv[6]);
				 
				log_i("Propeller: rightUp      leftDown     leftUp     rightDown     leftMiddle    rightMiddle");
				log_i("Propeller:    %d           %d          %d          %d            %d             %d",\
				 PropellerError.rightUp,PropellerError.leftDown,PropellerError.leftUp,PropellerError.rightDown,PropellerError.leftMiddle ,PropellerError.rightMiddle);
				Flash_Update();//FLASH更新
				rt_kprintf("\n");

		}
		
		else {
				log_e("Error! Input Error!");
		}
_exit:
    return result;
}
MSH_CMD_EXPORT(propeller_error_set,propeller_error_set <-100~+100>);
