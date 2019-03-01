#define LOG_TAG    "gyro"

#include "init.h"
#include <string.h>
#include "board.h"
#include <rthw.h>

/*---------------------- Constant / Macro Definitions -----------------------*/

#define PACKET_LENGTH      11    //���ݰ�����

/*----------------------- Variable Declarations -----------------------------*/
short res[10] = {0};

struct STime		stcTime;
struct SAcc 		stcAcc;
struct SGyro 		stcGyro;
struct SAngle 	stcAngle;
struct SMag 		stcMag;
struct SDStatus stcDStatus;
struct SPress 	stcPress;
struct SLonLat 	stcLonLat;
struct SGPSV 		stcGPSV;
struct SQ       stcQ;

struct JY901_t JY901 = {0}; //JY901��ʵֵ�ṹ��

/*----------------------- Function Implement --------------------------------*/

//CopeSerialDataΪ����2�жϵ��ú���������ÿ�յ�һ�����ݣ�����һ�����������
void CopeSerial2Data(unsigned char Data)
{
		static unsigned char RxBuffer[50];  //���ݰ�
		static unsigned char RxCheck = 0;	  //βУ����
		static unsigned char RxCount = 0;	    //���ռ���
		static unsigned char i = 0;	   		  //���ռ���
	
		RxBuffer[RxCount++] = Data;	//���յ������ݴ��뻺������
	
		if (RxBuffer[0]!=0x55){ //����ͷ���ԣ������¿�ʼѰ��0x55����ͷ
				RxCount=0;					  //��ջ�����
				return;
		}
		if (RxCount < PACKET_LENGTH) {return;}//���ݲ���11�����򷵻�
		
		/*********** ֻ�н�����11���ֽ����� �Ż�������³��� ************/
		for(i = 0;i < 10;i++){
				RxCheck += RxBuffer[i]; //У��λ�ۼ�
		}
		
		if(	RxCheck == RxBuffer[PACKET_LENGTH-1]){//�ж����ݰ�У�� �Ƿ���ȷ
	
				switch(RxBuffer[1]){		//�ж��������������ݣ�Ȼ���俽������Ӧ�Ľṹ���У���Щ���ݰ���Ҫͨ����λ���򿪶�Ӧ������󣬲��ܽ��յ�������ݰ�������
						case 0x50:	memcpy(&stcTime,&RxBuffer[2],8);	 break;//memcpyΪ�������Դ����ڴ濽��������������"string.h"�������ջ��������ַ����������ݽṹ�����棬�Ӷ�ʵ�����ݵĽ�����
						case 0x51:	memcpy(&stcAcc,&RxBuffer[2],8);		 break;
						case 0x52:	memcpy(&stcGyro,&RxBuffer[2],8);	 break;
						case 0x53:	memcpy(&stcAngle,&RxBuffer[2],8);	 break;
						case 0x54:	memcpy(&stcMag,&RxBuffer[2],8);		 break;
						case 0x55:	memcpy(&stcDStatus,&RxBuffer[2],8);break;
						case 0x56:	memcpy(&stcPress,&RxBuffer[2],8);	 break;
						case 0x57:	memcpy(&stcLonLat,&RxBuffer[2],8); break;
						case 0x58:	memcpy(&stcGPSV,&RxBuffer[2],8);	 break;
						case 0x59:	memcpy(&stcQ,&RxBuffer[2],8);			 break;
				}
				RxCount = 0;//��ջ�����
				RxCheck = 0;//У��λ����
		}
		else{  //��������
				RxCount = 0;//��ջ�����
				RxCheck = 0;//У��λ����
				return;
		}
		/*********** ------------------------------------- ************/

}

/* JY901 ����ת�� */
void JY901_Convert(struct JY901_t * pArr) //
{
		static u8 i = 0;
		for(i = 0;i < 3;i++){	
				pArr->Acc[i] = (float)stcAcc.a[i]/32768*16;
				pArr->Gyro[i] = (float)stcGyro.w[i]/32768*2000;
				pArr->Angle[i] = (float)stcAngle.angle[i]/32768*180;
				pArr->Mag[i] 		= stcMag.h[i];
		}
		pArr->Temperature = (float)stcAcc.T/100;
}


static void time_out(void* parameter)// ��ʱ��1��ʱ����  ����JY901ģ������ת��
{
	  /* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
    rt_enter_critical();
	
		JY901_Convert(&JY901);

		/* ���������� */
    rt_exit_critical();
}


int timer1_init(void)
{
		/* ��ʱ���Ŀ��ƿ� */
		static rt_timer_t timer1;
    /* ������ʱ��1 */
    timer1 = rt_timer_create("timer1",  /* ��ʱ�������� timer1 */
                        time_out, 		  /* ��ʱʱ�ص��Ĵ����� */
                        RT_NULL, 			  /* ��ʱ��������ڲ��� */
                        5,      			  /* ��ʱ���ȣ���OS TickΪ��λ����5��OS Tick   --> 50MS*/  
                        RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_HARD_TIMER); /* �����Զ�ʱ�� */
    /* ������ʱ�� */
    if (timer1 != RT_NULL){ 
				rt_timer_start(timer1);
		}

    return 0;
}
INIT_APP_EXPORT(timer1_init);


void show_logo(void)
{
		rt_kprintf("      *      \n");
		rt_kprintf(" *  *   *  * \n");
		rt_kprintf("  *   e   *   \n");
		rt_kprintf(" *  *   *  * \n");
		rt_kprintf("      *      \n");	
	
}
MSH_CMD_EXPORT(show_logo,show_logo);


/* Getʱ��  time */
void get_time(void)
{
		//���ݴ����string��       ��ΪRT-Thread rt_kprintf()�����޷���������ͣ�����ֽ����ݴ����String�ͷ���.
		char str[50];
		sprintf(str,"Time:20%d-%d-%d %d:%d:%.3f",stcTime.ucYear,stcTime.ucMonth,stcTime.ucDay,stcTime.ucHour,stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);
		log_i(str);
}
MSH_CMD_EXPORT(get_time,get acceleration[a]);


/* Get���ٶ�  acceleration */
void get_gyroscope(void)
{		
		char str[50];
		sprintf(str,"Acc:%.3f %.3f %.3f",JY901.Acc[0],JY901.Acc[1],JY901.Acc[2]);
		log_i(str);
		sprintf(str,"Gyro:%.3f %.3f %.3f",JY901.Gyro[0],JY901.Gyro[1],JY901.Gyro[2]);
		log_i(str);
		sprintf(str,"Angle:%.3f %.3f %.3f",JY901.Angle[0],JY901.Angle[1],JY901.Angle[2]);
		log_i(str);
		sprintf(str,"Mag:%d %d %d",JY901.Mag[0],JY901.Mag[1],JY901.Mag[2]);
		log_i(str);	
	
		return;
}
MSH_CMD_EXPORT(get_gyroscope,get JY901[a]);


/* Get �¶�  Temperature */
float get_temperature(void)
{
		char str[50];
		sprintf(str,"Temperature:%.2f C\r\n",JY901.Temperature);
		log_i(str);	
		return JY901.Temperature;
}
MSH_CMD_EXPORT(get_temperature, get Temperature[T]);








