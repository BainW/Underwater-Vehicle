#define LOG_TAG    "debug"

#include "init.h"
#include "PID.h"
#include <string.h>

/*---------------------- Constant / Macro Definitions -----------------------*/		

#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp) + 0) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )

#define PID_USE_NUM  	8

/*----------------------- Variable Declarations. -----------------------------*/
enum 
{
		DEBUG_NULL,
		PC_VCAN,
		PC_ANO,
		//������
		
}PC_TOOL;//��λ������


char *debug_tool_name[3]={"NULL","VCAN","ANO"};

extern rt_device_t debug_uart_device;	
extern struct SAngle 	stcAngle;


u8 debug_tool = PC_ANO; //ɽ�� / ������λ�� ���Ա�־λ
volatile u32 debug_count = 0;

u8 data_to_send[50];//ANO����վ�������ݻ���
u8 ANO_Send_PID_Flag[6]={0};//PID���ͱ�־λ
u16 Percontroller_Databuf[8]={0};//�Զ���ң�������ݻ���


Vector3f_pid PID_Parameter[PID_USE_NUM]={0};
u8 Sort_PID_Cnt=0;
u8 Sort_PID_Flag=0;

/*-----------------------Debug Thread Begin-----------------------------*/

void debug_send_thread_entry(void* parameter)
{
	
		while(1)
		{

				rt_thread_mdelay(1);
				if(debug_count != 0)//����ʱ��
				{
							
						switch(debug_tool)//ѡ����λ��
						{
								case PC_VCAN: Vcan_Send_Data();break;
								case PC_ANO :	ANO_SEND_StateMachine();break;
								default :break;
						}
				}		
		}
}



int Debug_thread_init(void)
{
	  rt_thread_t debug_send_tid;
		/*������̬�߳�*/
    debug_send_tid = rt_thread_create("debug",			 //�߳�����
                    debug_send_thread_entry,									 //�߳���ں�����entry��
                    RT_NULL,							   //�߳���ں���������parameter��
                    1024,										 //�߳�ջ��С����λ���ֽڡ�byte��
                    30,										 	 //�߳����ȼ���priority��
                    10);										 //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (debug_send_tid != RT_NULL){
				rt_thread_startup(debug_send_tid);
		}
		return 0;
}
INIT_APP_EXPORT(Debug_thread_init);

/*-----------------------Debug Thread End-----------------------------*/


void ANO_DT_Data_Receive_Prepare(u8 data)//ANO����վ���ݽ���
{
		static u8 RxBuffer[50];
		static u8 _data_len = 0,_data_cnt = 0;
		static u8 state = 0;
		if(state==0&&data==0xAA)//֡ͷ1
		{
				state=1;
				RxBuffer[0]=data;
		}
		else if(state==1&&data==0xAF)//֡ͷ2
		{
				state=2;
				RxBuffer[1]=data;
		}
		else if(state==2&&data<0XF1)//�����ֽ�
		{
				state=3;
				RxBuffer[2]=data;
		}
		else if(state==3&&data<50)//��Ч���ݳ���
		{
				state = 4;
				RxBuffer[3]=data;
				_data_len = data;
				_data_cnt = 0;
		}
		else if(state==4&&_data_len>0)//���ݽ���
		{
				_data_len--;
				RxBuffer[4+_data_cnt++]=data;
				if(_data_len==0)
					state = 5;
		}
		else if(state==5)//У���
		{
				state = 0;
				RxBuffer[4+_data_cnt]=data;
				ANO_DT_Data_Receive_Anl(RxBuffer,_data_cnt+5);//���ݽ���
		}
		else state = 0;
}



static void ANO_DT_Send_Check(u8 head, u8 check_sum)//ANO����վ��У��
{
		u8 sum = 0,i=0;
		data_to_send[0]=0xAA;
		data_to_send[1]=0xAA;
		data_to_send[2]=0xEF;
		data_to_send[3]=2;
		data_to_send[4]=head;
		data_to_send[5]=check_sum;
		for(i=0;i<6;i++){
				sum += data_to_send[i];
		}
		data_to_send[6]=sum;
		rt_device_write(debug_uart_device, 0,data_to_send, 7);    //���ͺ�����

}


void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num)//ANO���ݽ���
{
		u8 sum = 0,i=0;
		static u8 status = 1;
		for(i=0;i<(num-1);i++)  sum += *(data_buf+i);
		if(!(sum==*(data_buf+num-1)))    return; //�ж�sum
		if(!(*(data_buf)==0xAA && *(data_buf+1)==0xAF))     return;//�ж�֡ͷ
		if(*(data_buf+2)==0X01)
		{
				if(*(data_buf+4)==0X01) {;}
				if(*(data_buf+4)==0X02) {;}
				if(*(data_buf+4)==0X03) {;}
		}
		
		if(*(data_buf+2)==0X02)
		{
				if(*(data_buf+4)==0X01)		//��ȡ��ǰPID����
				{
						ANO_Send_PID_Flag[0]=1;
						ANO_Send_PID_Flag[1]=1;
						ANO_Send_PID_Flag[2]=1;
						ANO_Send_PID_Flag[3]=1;
						ANO_Send_PID_Flag[4]=1;
						ANO_Send_PID_Flag[5]=1;
						LED_Turn(70,status);	//��ʼ��Ϊ�ߵ�ƽ ��Ϩ��
		//      Bling_Set(&Light_1,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
		//      Bling_Set(&Light_2,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
		//      Bling_Set(&Light_3,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
				}
				if(*(data_buf+4)==0X02)
				{
						;
				}
				if(*(data_buf+4)==0XA0)     //��ȡ�汾��Ϣ
				{
						;
				}
				if(*(data_buf+4)==0XA1)     //�ָ�Ĭ�ϲ���
				{
						Sort_PID_Flag=3;
						LED_Turn(69,status);	//��ʼ��Ϊ�ߵ�ƽ ��Ϩ��
		//				Bling_Set(&Light_1,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
		//				Bling_Set(&Light_2,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
		//				Bling_Set(&Light_3,1000,50,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
				}
		}
		
		if(*(data_buf+2)==0X10)                             //����PID1
		{
				Total_Controller.Roll_Gyro_Control.Kp  = 0.001*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
				Total_Controller.Roll_Gyro_Control.Ki  = 0.001*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
				Total_Controller.Roll_Gyro_Control.Kd  = 0.01*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
				Total_Controller.Pitch_Gyro_Control.Kp   = 0.001*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
				Total_Controller.Pitch_Gyro_Control.Ki   = 0.001*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
				Total_Controller.Pitch_Gyro_Control.Kd   = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
				Total_Controller.Yaw_Gyro_Control.Kp    = 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
				Total_Controller.Yaw_Gyro_Control.Ki    = 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
				Total_Controller.Yaw_Gyro_Control.Kd    = 0.01*( (vs16)(*(data_buf+20)<<8)|*(data_buf+21) );
				ANO_DT_Send_Check(*(data_buf+2),sum);
		}
		if(*(data_buf+2)==0X11)                             //����PID2
		{
				Total_Controller.Roll_Angle_Control.Kp  = 0.001*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
				Total_Controller.Roll_Angle_Control.Ki  = 0.001*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
				Total_Controller.Roll_Angle_Control.Kd  = 0.01*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
				Total_Controller.Pitch_Angle_Control.Kp   = 0.001*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
				Total_Controller.Pitch_Angle_Control.Ki   = 0.001*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
				Total_Controller.Pitch_Angle_Control.Kd   = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
				Total_Controller.Yaw_Angle_Control.Kp    = 0.001*( (vs16)(*(data_buf+16)<<8)|*(data_buf+17) );
				Total_Controller.Yaw_Angle_Control.Ki    = 0.001*( (vs16)(*(data_buf+18)<<8)|*(data_buf+19) );
				Total_Controller.Yaw_Angle_Control.Kd    = 0.01*( (vs16)(*(data_buf+20)<<8)|*(data_buf+21) );
				ANO_DT_Send_Check(*(data_buf+2),sum); 
		}
		if(*(data_buf+2)==0X12)                             //����PID3
		{
				Total_Controller.High_Speed_Control.Kp    = 0.001*( (vs16)(*(data_buf+4)<<8)|*(data_buf+5) );
				Total_Controller.High_Speed_Control.Ki    = 0.001*( (vs16)(*(data_buf+6)<<8)|*(data_buf+7) );
				Total_Controller.High_Speed_Control.Kd    = 0.01*( (vs16)(*(data_buf+8)<<8)|*(data_buf+9) );
				Total_Controller.High_Position_Control.Kp = 0.001*( (vs16)(*(data_buf+10)<<8)|*(data_buf+11) );
				Total_Controller.High_Position_Control.Ki = 0.001*( (vs16)(*(data_buf+12)<<8)|*(data_buf+13) );
				Total_Controller.High_Position_Control.Kd = 0.01*( (vs16)(*(data_buf+14)<<8)|*(data_buf+15) );
				ANO_DT_Send_Check(*(data_buf+2),sum); 
		}
		if(*(data_buf+2)==0X13)                             //����PID4
		{
				ANO_DT_Send_Check(*(data_buf+2),sum);
		}
		if(*(data_buf+2)==0X14)                             //����PID5
		{
	 
				ANO_DT_Send_Check(*(data_buf+2),sum);
		}
		if(*(data_buf+2)==0X15)                             //����PID6
		{
				ANO_DT_Send_Check(*(data_buf+2),sum);
				Sort_PID_Cnt++;
				Sort_PID_Flag=1;
			
		}
}

void ANO_Data_Send_Status(void)//���ͻ�����Ϣ����̬������״̬��
{
		u8 _cnt=0;
		vs16 _temp;
		vs32 _temp2;
		u8 sum = 0;
		u8 i;
		data_to_send[_cnt++]=0xAA;
		data_to_send[_cnt++]=0xAA;
		data_to_send[_cnt++]=0x01;
		data_to_send[_cnt++]=0;
		
		_temp = (int)(JY901.Euler.Roll*100);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int)(JY901.Euler.Pitch*100);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int)(-JY901.Euler.Yaw*100);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		
		_temp2 = 0;//��λcm
		data_to_send[_cnt++]=BYTE3(_temp2);
		data_to_send[_cnt++]=BYTE2(_temp2);
		data_to_send[_cnt++]=BYTE1(_temp2);
		data_to_send[_cnt++]=BYTE0(_temp2);
		
		data_to_send[_cnt++]=0x01;//����ģʽ
		data_to_send[_cnt++]=1;//����0������1
		
		data_to_send[3] = _cnt-4;
		sum = 0;
		for(i=0;i<_cnt;i++)
			sum += data_to_send[i];
		data_to_send[_cnt++]=sum;
		rt_device_write(debug_uart_device, 0,data_to_send, _cnt);    //���ͺ�����
}

/* ANO��������վ���� */
void ANO_DT_Send_Senser(s16 a_x,s16 a_y,s16 a_z,s16 g_x,s16 g_y,s16 g_z,s16 m_x,s16 m_y,s16 m_z)//���ʹ�����ԭʼ������
{
		u8 _cnt=0;
		vs16 _temp;
		u8 sum = 0;
		u8 i = 0;
		data_to_send[_cnt++]=0xAA;
		data_to_send[_cnt++]=0xAA;
		data_to_send[_cnt++]=0x02;
		data_to_send[_cnt++]=0;
		
		_temp = a_x;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = a_y;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = a_z;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		
		_temp = g_x;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = g_y;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = g_z;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		
		_temp = m_x;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = m_y;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = m_z;
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		
		data_to_send[3] = _cnt-4;
		
		sum = 0;
		for(i=0;i<_cnt;i++){
				sum += data_to_send[i];
		}
		data_to_send[_cnt++] = sum;
		
		rt_device_write(debug_uart_device, 0,data_to_send, _cnt);    //���ͺ�����
}


void ANO_DT_Send_PID(u8 group,float p1_p,float p1_i,float p1_d,float p2_p,float p2_i,float p2_d,float p3_p,float p3_i,float p3_d)//����PID����
{
		u8 _cnt=0;
		u8 sum = 0,i=0;
		int16_t _temp;
		
		data_to_send[_cnt++]=0xAA;
		data_to_send[_cnt++]=0xAA;
		data_to_send[_cnt++]=0x10+group-1;
		data_to_send[_cnt++]=0;
		
		
		_temp = (int16_t)(p1_p * 1000);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int16_t)(p1_i  * 1000);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int16_t)(p1_d  * 100);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int16_t)(p2_p  * 1000);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int16_t)(p2_i  * 1000);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int16_t)(p2_d * 100);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int16_t)(p3_p  * 1000);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int16_t)(p3_i  * 1000);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		_temp = (int16_t)(p3_d * 100);
		data_to_send[_cnt++]=BYTE1(_temp);
		data_to_send[_cnt++]=BYTE0(_temp);
		
		data_to_send[3] = _cnt-4;
		
		for(i=0;i<_cnt;i++){
			sum += data_to_send[i];
		}
		
		data_to_send[_cnt++]=sum;
		rt_device_write(debug_uart_device, 0,data_to_send, _cnt);    //���ͺ�����
}


void ANO_DT_Send_RCData(u16 thr,u16 yaw,u16 rol,u16 pit,u16 aux1,u16 aux2,u16 aux3,u16 aux4,u16 aux5,u16 aux6)//����ң����ͨ������
{
		u8 _cnt=0;
		u8 i=0;
		u8 sum = 0;
		data_to_send[_cnt++]=0xAA;
		data_to_send[_cnt++]=0xAA;
		data_to_send[_cnt++]=0x03;
		data_to_send[_cnt++]=0;
		data_to_send[_cnt++]=BYTE1(thr);
		data_to_send[_cnt++]=BYTE0(thr);
		data_to_send[_cnt++]=BYTE1(yaw);
		data_to_send[_cnt++]=BYTE0(yaw);
		data_to_send[_cnt++]=BYTE1(rol);
		data_to_send[_cnt++]=BYTE0(rol);
		data_to_send[_cnt++]=BYTE1(pit);
		data_to_send[_cnt++]=BYTE0(pit);
		data_to_send[_cnt++]=BYTE1(aux1);
		data_to_send[_cnt++]=BYTE0(aux1);
		data_to_send[_cnt++]=BYTE1(aux2);
		data_to_send[_cnt++]=BYTE0(aux2);
		data_to_send[_cnt++]=BYTE1(aux3);
		data_to_send[_cnt++]=BYTE0(aux3);
		data_to_send[_cnt++]=BYTE1(aux4);
		data_to_send[_cnt++]=BYTE0(aux4);
		data_to_send[_cnt++]=BYTE1(aux5);
		data_to_send[_cnt++]=BYTE0(aux5);
		data_to_send[_cnt++]=BYTE1(aux6);
		data_to_send[_cnt++]=BYTE0(aux6);
		
		data_to_send[3] = _cnt-4;
		
		sum = 0;
		for(i=0;i<_cnt;i++)
			sum += data_to_send[i];
		
		data_to_send[_cnt++]=sum;
		rt_device_write(debug_uart_device, 0,data_to_send, _cnt);    //���ͺ�����
}

void ANO_SEND_StateMachine(void)//��������ѭ������
{
		static u16 ANO_Cnt = 0;
		ANO_Cnt++;
	
		if(ANO_Cnt == 1){		
				ANO_Data_Send_Status();
		}
	
		else if(ANO_Cnt == 2){
				ANO_DT_Send_Senser((int16_t)JY901.Acc.x,(int16_t)JY901.Acc.y,(int16_t)JY901.Acc.z,
													 (int16_t)JY901.Gyro.x,(int16_t)JY901.Gyro.y,(int16_t)JY901.Gyro.z,
													 (int16_t)JY901.Mag.x,(int16_t)JY901.Mag.y,(int16_t)JY901.Mag.z);
		}
		
		else if(ANO_Cnt==3)
		{

				ANO_DT_Send_RCData(Percontroller_Databuf[2],Percontroller_Databuf[3],
													 Percontroller_Databuf[0],Percontroller_Databuf[1],
													 Percontroller_Databuf[4],Percontroller_Databuf[5],
													 Percontroller_Databuf[6],Percontroller_Databuf[7],0,0);

		}
		else if(ANO_Cnt == 4
          &&ANO_Send_PID_Flag[0] == 0
            &&ANO_Send_PID_Flag[1] == 0
              &&ANO_Send_PID_Flag[2] == 0
                &&ANO_Send_PID_Flag[3] == 0
                  &&ANO_Send_PID_Flag[4] == 0
                    &&ANO_Send_PID_Flag[5] == 0)//��ǰ��ֹ���Ͷ���
		{
				ANO_Cnt=0;
		}
	
		else if(ANO_Cnt == 5 && ANO_Send_PID_Flag[0] == 1){
				ANO_DT_Send_PID(1,Total_Controller.Roll_Gyro_Control.Kp,
												Total_Controller.Roll_Gyro_Control.Ki,
												Total_Controller.Roll_Gyro_Control.Kd,
												Total_Controller.Pitch_Gyro_Control.Kp,
												Total_Controller.Pitch_Gyro_Control.Ki,
												Total_Controller.Pitch_Gyro_Control.Kd,
												Total_Controller.Yaw_Gyro_Control.Kp,
												Total_Controller.Yaw_Gyro_Control.Ki,
												Total_Controller.Yaw_Gyro_Control.Kd);
				ANO_Send_PID_Flag[0]=0;
		}
		
		else if(ANO_Cnt == 6 && ANO_Send_PID_Flag[1] == 1)
		{
				ANO_DT_Send_PID(2,Total_Controller.Roll_Angle_Control.Kp,
												Total_Controller.Roll_Angle_Control.Ki,
												Total_Controller.Roll_Angle_Control.Kd,
												Total_Controller.Pitch_Angle_Control.Kp,
												Total_Controller.Pitch_Angle_Control.Ki,
												Total_Controller.Pitch_Angle_Control.Kd,
												Total_Controller.Yaw_Angle_Control.Kp,
												Total_Controller.Yaw_Angle_Control.Ki,
												Total_Controller.Yaw_Angle_Control.Kd);
				ANO_Send_PID_Flag[1]=0;
		}
		
		else if(ANO_Cnt == 7 && ANO_Send_PID_Flag[2] == 1)
		{
				ANO_DT_Send_PID(3,Total_Controller.High_Speed_Control.Kp,
												Total_Controller.High_Speed_Control.Ki,
												Total_Controller.High_Speed_Control.Kd,
												Total_Controller.High_Position_Control.Kp,
												Total_Controller.High_Position_Control.Ki,
												Total_Controller.High_Position_Control.Kd,
												0,
												0,
												0);
				ANO_Send_PID_Flag[2]=0;
		}
		
		else if(ANO_Cnt == 8 && ANO_Send_PID_Flag[3] == 1)
		{
				ANO_DT_Send_PID(4,1,
												1,
												1,
												1,
												1,
												1,
												0,0,0);
				ANO_Send_PID_Flag[3]=0;
		}
		else if(ANO_Cnt==9&&ANO_Send_PID_Flag[4]==1)
		{
				ANO_DT_Send_PID(5,1,
												1,
												1,
												1,		
												1,
												1,				
												1,					
												1,						
												1);
				ANO_Send_PID_Flag[4]=0;
		}
		else if(ANO_Cnt == 10 && ANO_Send_PID_Flag[5] == 1)
		{
				ANO_DT_Send_PID(6,  //����֡ PID
												0,
												0,
												0,
												0,
												0,
												0,
												0,
												0,
												0);
				ANO_Send_PID_Flag[5]=0;
				ANO_Cnt=0;
		}
							
}



void Save_Or_Reset_PID_Parameter()
{
		if(Sort_PID_Flag == 1)//������վ����PID����д��Flash
		{
				PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
				PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
				PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
				
				PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
				PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
				PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
				
				PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
				PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
				PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
				
				PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
				PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
				PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
				
				PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
				PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
				PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
				
				PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
				PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
				PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
				
				PID_Parameter[6].p=Total_Controller.High_Speed_Control.Kp;
				PID_Parameter[6].i=Total_Controller.High_Speed_Control.Ki;
				PID_Parameter[6].d=Total_Controller.High_Speed_Control.Kd;
				
				PID_Parameter[7].p=Total_Controller.High_Position_Control.Kp;
				PID_Parameter[7].i=Total_Controller.High_Position_Control.Ki;
				PID_Parameter[7].d=Total_Controller.High_Position_Control.Kd;			

				//Save_PID_Parameter();
				
				Sort_PID_Flag=0;
		}
		else if(Sort_PID_Flag==2)//����λPID��������д��Flash
		{
				//Total_PID_Init();//��PID��������Ϊ����Control_Unit���������
				PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
				PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
				PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
				
				PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
				PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
				PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
				
				PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
				PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
				PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
				
				PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
				PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
				PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
				
				PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
				PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
				PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
				
				PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
				PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
				PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
				
				PID_Parameter[6].p=Total_Controller.High_Speed_Control.Kp;
				PID_Parameter[6].i=Total_Controller.High_Speed_Control.Ki;
				PID_Parameter[6].d=Total_Controller.High_Speed_Control.Kd;
				
				PID_Parameter[7].p=Total_Controller.High_Position_Control.Kp;
				PID_Parameter[7].i=Total_Controller.High_Position_Control.Ki;
				PID_Parameter[7].d=Total_Controller.High_Position_Control.Kd;
				
				//Save_PID_Parameter();
				
				Sort_PID_Flag=0;
				
				ANO_Send_PID_Flag[0]=1;//�ظ�Ĭ�ϲ����󣬽����µ����ݷ����õ���վ
				ANO_Send_PID_Flag[1]=1;
				ANO_Send_PID_Flag[2]=1;
				ANO_Send_PID_Flag[3]=1;
				ANO_Send_PID_Flag[4]=1;
				ANO_Send_PID_Flag[5]=1;
		}
		else if(Sort_PID_Flag==3)//����λPID��������д��Flash
		{
			//Total_PID_Init();//��PID��������Ϊ����Control_Unit���������
				PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
				PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
				PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
				
				PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
				PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
				PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
				
				PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
				PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
				PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
				
				PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
				PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
				PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
				
				PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
				PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
				PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
				
				PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
				PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
				PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
				
				PID_Parameter[6].p=Total_Controller.High_Speed_Control.Kp;
				PID_Parameter[6].i=Total_Controller.High_Speed_Control.Ki;
				PID_Parameter[6].d=Total_Controller.High_Speed_Control.Kd;
				
				PID_Parameter[7].p=Total_Controller.High_Position_Control.Kp;
				PID_Parameter[7].i=Total_Controller.High_Position_Control.Ki;
				PID_Parameter[7].d=Total_Controller.High_Position_Control.Kd;
			 
			//Save_PID_Parameter();
			
				Sort_PID_Flag=0;
				
				ANO_Send_PID_Flag[0]=1;//�ظ�Ĭ�ϲ����󣬽����µ����ݷ����õ���վ
				ANO_Send_PID_Flag[1]=1;
				ANO_Send_PID_Flag[2]=1;
				ANO_Send_PID_Flag[3]=1;
				ANO_Send_PID_Flag[4]=1;
				ANO_Send_PID_Flag[5]=1;
		}
		else {
				return;
		}
}



/* VCANɽ����λ������ BEGIN */
void Vcan_Send_Cmd(void *wareaddr, unsigned int waresize)
{
		#define CMD_WARE     3
    u8 cmdf[2] = {CMD_WARE, ~CMD_WARE};    //���ڵ��� ʹ�õ�ǰ����
    u8 cmdr[2] = {~CMD_WARE, CMD_WARE};    //���ڵ��� ʹ�õĺ�����

    rt_device_write(debug_uart_device, 0,cmdf, 2);    //�ȷ���ǰ����
    rt_device_write(debug_uart_device, 0,(u8 *)wareaddr, waresize);    //��������
    rt_device_write(debug_uart_device, 0,cmdr, 2);    //���ͺ�����
}



void Vcan_Send_Data(void)
{   

		static int list[8]= {0};
		list[0] = (int)JY901.Euler.Roll; 	//����� Roll 
		list[1] = (int)JY901.Euler.Pitch; //������ Pitch
		list[2] = (int)JY901.Euler.Yaw; 	//ƫ���� Yaw
		list[3] = 180;//-(Servo_Duty-Servo_Duty_Md);
		list[4] = 120;//corner_meet_rn;//edge_start[1];//
		list[5] = 60;
		list[6] = 30;	//camera_forecast_center;//
		list[7] = 0;	//camera_center;
		
		Vcan_Send_Cmd(list,sizeof(list));
}
/* VCANɽ����λ������  END */




/* debug ɽ����λ������ MSH���� */
static int debug(int argc, char **argv)
{
    int result = 0;

    if (argc != 2){
				rt_kprintf("Usage: debug 10  [notes:debug 10s]\n");
				result = -RT_ERROR; 
				goto _exit;  
    }

		debug_count = (atoi(argv[1]))*1000;  //1ms����һ��
		rt_kprintf("Debug %d sending. ",atoi(argv[1]));
_exit:
    return result;
}
MSH_CMD_EXPORT(debug,ag: debug 10);



/* debug ������λ������ */
static int set_debug_tool(int argc,char **argv)
{
		int result = 0;
    if (argc != 2){
				log_e("Proper Usage: debug_by vcan / ano / null");//�÷�:������λ������
				result = -RT_ERROR;
        goto _exit;
    }

		if( !strcmp(argv[1],"vcan") ){ //����Ϊ ɽ����λ�� strcmp ����������� ����0
				debug_tool = PC_VCAN;
		}

		else if( !strcmp(argv[1],"ano") ){ //����Ϊ ������λ��
				debug_tool = PC_ANO;
		}
		else if( !strcmp(argv[1],"null") ){ //����Ϊ ������λ��
				debug_tool = PC_ANO;
		}
		else {
				log_e("Proper Usage: debug_by vcan / ano / null");//�÷�:������λ������
				goto _exit;
		}
		Flash_Update();
_exit:
    return result;
}
MSH_CMD_EXPORT(set_debug_tool,debug_by vcan / ano / null);

