#define LOG_TAG    "oled"

#include "init.h"
#include "math.h"
	
/* �Զ���OLED ����ϵ����: 

	127 ��y
			---------
			|	      |
			|				|
			|				|
			|				|
			|				|
			|				|
		(0,0)-----��x  
							63
*/
/*---------------------- Constant / Macro Definitions -----------------------*/
	
#define Pi 3.14159f //float��
	
/*----------------------- Variable Declarations -----------------------------*/


extern struct rt_event init_event;/* ALL_init �¼����ƿ� */
extern struct SAngle 	stcAngle;

extern struct JY901_t JY901;

float slope = 0.0; //����������ϵ�� ����б�� slope

char *VehicleModeName[2] = {"AUV","ROV"};
volatile MENU_LIST_e MENU = StatusPage; //OLED��ʼҳ��Ϊ ״̬ҳ. volatile��һ���������η���
																				//volatile ������ ����Ϊָ��ؼ��֣�ȷ������ָ�������������Ż���ʡ�ԣ���Ҫ��ÿ��ֱ�Ӷ�ֵ��

/* OLED ���� ��ʼ��. */
oled_t oled = {	 StatusPage,		 //ҳ�� pagenum
								 StatusPage,	   //�ݴ�ҳ�� ���ҳ���Ƿ�ı� pagechange
								 0,              //ҳ��ı��־λ pagechange flag
								{	"StatusPage",
									"GyroscopePage",
									"FlashPage",
									"PicturePage"} //ҳ�� pagename
								
};

/*----------------------- Function Implement --------------------------------*/

void menu_define(void) //�˵�����
{

	if(oled.pagenum >= OLED_Page_MAX || oled.pagenum < StatusPage) oled.pagenum = StatusPage; //����ҳ�淶Χ ��Ϊ��һҳ
	if(oled.pagechange != oled.pagenum){
			buzzer_bibi(1,1);
			rt_kprintf("Current Menu_Page: %s \n",oled.pagename[oled.pagenum-1]);
			OLED_Clear();
			oled.pagechange_flag = 1;
	}
	else {oled.pagechange_flag = 0;}
	oled.pagechange = oled.pagenum;

	switch(oled.pagenum){
			case 1:{
				MENU = StatusPage;	 OLED_StatusPage();			break;
			}
			case 2:{
				MENU = GyroscopePage;OLED_GyroscopePage();break;
			}
			case 3:{
				MENU = FlashPage;		 											break;
			}
			case 4:{
				MENU = PicturePage;	 OLED_PicturePage();		  break;
			}	
	}
}

	
void oled_thread_entry(void* parameter)
{
	Boot_Animation();	//��������
	OLED_Clear();

	while(1)
	{	
			menu_define();//�˵�����ѡ��
			rt_thread_mdelay(1000/pow(MENU,2));  //�˵���Խ�� ˢ������Խ��
	}
	
}

/* ϵͳ��һҳ ��״̬ҳ��boma_value_get*/
void OLED_StatusPage(void)
{
		char str[100];
		OLED_ShowMyChar(119,0,0,16,1); //3G����ͼ��2
	
		//OLED_ShowMyChar(0,32,1,16,1); //Wifiͼ��
	
		sprintf(str,"Mode: [ %s 00%d ] ",VehicleModeName[MODE],boma_value_get());
		OLED_ShowString(0,0, (u8 *)str,12); 
	
		sprintf(str,"Voltage:%.2f v\r\n",get_vol());
		OLED_ShowString(0,16,(u8 *)str,12); 
		sprintf(str,"Temperature:%.2f C\r\n",JY901.Temperature);
		OLED_ShowString(0,48,(u8 *)str,12);
		OLED_Refresh_Gram();//������ʾ��OLED
}

/* OLED�ڶ�ҳ ���������ҳ��*/
void OLED_GyroscopePage(void)
{
		char str[100];
		sprintf(str,"Acc:%.2f %.2f %.2f  ",JY901.Acc[0],JY901.Acc[1],JY901.Acc[2]);
		OLED_ShowString(0,0,(u8 *)str,12); 	
		
		sprintf(str,"Gyro:%.1f %.1f %.1f ",JY901.Gyro[0],JY901.Gyro[1],JY901.Gyro[2]);
		OLED_ShowString(0,16,(u8 *)str,12); 	
		
		sprintf(str,"Ang:%.1f %.1f %.1f  ",JY901.Angle[0],JY901.Angle[1],JY901.Angle[2]);
		OLED_ShowString(0,32,(u8 *)str,12); 	
		 
		sprintf(str,"Mag:%d %d %d  ",JY901.Mag[0],JY901.Mag[1],JY901.Mag[2]);
		OLED_ShowString(0,48,(u8 *)str,12); 
		
	  OLED_Refresh_Gram();//������ʾ��OLED
}

/* OLED����ҳ ��ͼ��ҳ��*/
void OLED_PicturePage(void)
{
		static u8 y=0;
		char str[100];
		static int Angle_z = 0,Angle_y = 0;
		
	
		draw_fill_circle(31+Angle_z,31+Angle_y,6,0); //���ʵ��Բ������ˢ������

		draw_line(31,31,slope,0); //�����һ�λ����� ����ˢ��
		OLED_Refresh_Gram();//������ʾ��OLED
	

		Angle_z = JY901.Angle[0]/7;
		Angle_y = JY901.Angle[1]/7;
		slope = tan((float)(JY901.Angle[2]*Pi/180));  //ת�������� ���㶫��������ϵ�� ����б��slope
	
		for(y = 28;y <= 36;y++){ //��Բ���ײ���ȱʧ��
				OLED_DrawPoint(y,0,1);
				OLED_DrawPoint(y,63,1);
		}

		draw_line(31,31,slope,1);
		
		sprintf(str,"Pit:%3.1f  ",JY901.Angle[0]); //������Pitch
		OLED_ShowString(65,0, (u8 *)str,12);
		
		sprintf(str,"Rol:%3.1f  ",JY901.Angle[1]);  //�����Roll
		OLED_ShowString(65,16, (u8 *)str,12);
		
		sprintf(str,"Yaw:%3.1f  ",JY901.Angle[2]); //ƫ����Yaw
		OLED_ShowString(65,32, (u8 *)str,12);
		
		sprintf(str,"k:%.1f   ",slope);
		OLED_ShowString(65,48,(u8 *)str,12); 
		

		OLED_ShowString(29,2 ,(u8 *)"N",12);
		OLED_ShowString(29,51,(u8 *)"S",12);
		OLED_ShowString(3	,28,(u8 *)"W",12);
		OLED_ShowString(55,28,(u8 *)"E",12);
		
		draw_circle(31,31,32);//���̶�Բ
		draw_fill_circle(31+Angle_z,31+Angle_y,6,1); //��ʵ��Բ
	
		OLED_Refresh_Gram();//������ʾ��OLED						
}



/* �������� */
void Boot_Animation(void)
{
		static u8 x=0,y=0;

		for(x = 63;x>=18;x--){
				OLED_DrawPoint(108-0.7*x,x,1);//��б�� x,y����
				OLED_DrawPoint(17 +0.7*x,x,1);
				y = 64-x;
				OLED_DrawPoint(64-0.7*y,y,1);
				OLED_DrawPoint(64+0.7*y,y,1);
				rt_thread_delay(2);
			  OLED_Refresh_Gram();//������ʾ��OLED
		}
		
		for(x = 30;x <= 94;x++){
				OLED_DrawPoint(125-x,47,1);
				OLED_DrawPoint(x,18,1);
				rt_thread_delay(2);
			  OLED_Refresh_Gram();//������ʾ��OLED
		}

		OLED_ShowString(60,20,(u8 *)"E",16);
	  OLED_Refresh_Gram();//������ʾ��OLED
		rt_thread_delay(100);
		
}




void draw_fill_circle(u8 x0,u8 y0,u8 r,u8 dot)//д��ʵ��Բ��(x0,y0),�뾶r
{	
		u8 x = 0,y = 0,R = 0;
		for(x = x0-r;x <= x0+r;x++){
				for(y = y0-r; y <= y0+r ;y++ ){
						R = sqrt(pow(r,2)-pow(x-x0,2))+y0; //Բ����  x,y����		
						if( (y >= y0 && y <= R) || (y < y0 && y >= 2*y0-R )|| dot == 0 ) {  //�������� Բ������	
								OLED_DrawPoint(y,x,dot);
						}	
				}
		}
}


void draw_circle(u8 x0,u8 y0,u8 r) //Բ��(x0,y0),�뾶r
{
		u8 x,y;

		for(x = 0;x <= 63;x++){
				y = sqrt(pow(r,2)-pow(x-x0,2))+y0; //Բ����  x,y����
				OLED_DrawPoint(y,x,1);      //�ϰ�Բ
				OLED_DrawPoint(63-y,x,1);  //�°�Բ
		}
		
}

/* ʹ�ò�ͬ����ϵ Ϊ�˽�������� xӳ��yʱֻ�ܶ��һ�Ĺ�ϵ */
void draw_line(u8 x0,u8 y0,float k,u8 dot) //���̶���(x0,y0),б��k   dot:0,���;1,���	  
{
		u8 x,y;
		//y = (k*(x-x0)+y0); ֱ�ߺ���
/* ���º���ʹ�ø�����ϵ: 
	                127 ��y
			-----------------
			|								|
			|								|
			|								|
			|								|
		��---------------(0,0)x  
			63
*/
		for(x = 0;x <= 63;x++){
				y = sqrt(pow(20,2)-pow(x-31,2))+31+1; //Բ����  x,y����
			
				if( (JY901.Angle[2] >-135 && JY901.Angle[2] <-90 ) ||(JY901.Angle[2] >90 && JY901.Angle[2] < 145 ) || dot == 0 ){ //�ϰ�Բ
						if(  ((x-x0)/k+y0) >= 31 && ((x-x0)/k+y0) < y ) {  //�������� Բ������
								OLED_DrawPoint(x,((x-x0)/k+y0),dot);}
				}
				
				if( (JY901.Angle[2] < -45 && JY901.Angle[2] > -90) || (JY901.Angle[2] < 90 && JY901.Angle[2] > 45) || dot == 0 ){ //�ϰ�Բ
						if(  ((x-x0)/k+y0) <= 31 && ((x-x0)/k+y0)> 63-y ) {  //�������� Բ������
								OLED_DrawPoint(x,((x-x0)/k+y0),dot);}
				}
		}
/* ���º���ʹ�ø�����ϵ: 
	127 ��y
			---------
			|	      |
			|				|
			|				|
			|				|
			|				|
			|				|
		(0,0)-----��x  
							63
*/
		for(x = 0;x <= 63;x++){
				y = sqrt(pow(20,2)-pow(x-31,2))+31+1; //Բ����  x,y����
		
				if( (JY901.Angle[2] >=-45 && JY901.Angle[2] <= 0) || (JY901.Angle[2] >=-180 && JY901.Angle[2] <= -135)  || dot == 0 ){  // JY901.Angle[2] < 0
						if( (k*(x-x0)+y0) >= 31 && (k*(x-x0)+y0) < y ) {  //�������� Բ������   �ϰ�Բ
								OLED_DrawPoint((k*(x-x0)+y0),x,dot);}
				}
				
				if( (JY901.Angle[2] > 0 && JY901.Angle[2] <= 45) || (JY901.Angle[2] >=135 && JY901.Angle[2] <= 180)  || dot == 0 ){  // JY901.Angle[2] < 0
						if(((k*(x-x0)+y0)< 31 && (k*(x-x0)+y0) > 63-y)) {  //�������� Բ������  �°�Բ
								OLED_DrawPoint((k*(x-x0)+y0),x,dot);} 
				}
		}

}
/* OLED �̳߳�ʼ�� */
int oled_thread_init(void)
{
    rt_thread_t oled_tid;
		/*������̬�߳�*/
    oled_tid = rt_thread_create("oled", //�߳�����
                    oled_thread_entry,	//�߳���ں�����entry��
                    RT_NULL,				    //�߳���ں���������parameter��
                    2048,							  //�߳�ջ��С����λ���ֽڡ�byte��
                    15,								  //�߳����ȼ���priority��
                    10);							  //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (oled_tid != RT_NULL){
				OLED_Init();
				log_i("OLED_Init()");
				rt_thread_startup(oled_tid);
				rt_event_send(&init_event, OLED_EVENT);
				oled.pagechange = oled.pagenum;  //��ʼ���ݴ�ҳ��
		}
		return 0;
}
INIT_APP_EXPORT(oled_thread_init);



/* OLED ��һҳ */
void next_oled_page(void)
{
		oled.pagenum ++;
}
MSH_CMD_EXPORT(next_oled_page,next_oled_page[page++]);

/* OLED ��һҳ */
void last_oled_page(void)
{
		oled.pagenum --;
}
MSH_CMD_EXPORT(last_oled_page,last_oled_page[page--]);


void get_slope(void)
{
		char str[100];
		sprintf(str,"k= %f\n",slope);
		rt_kprintf(str);
}
MSH_CMD_EXPORT(get_slope,get_slope[k]);

