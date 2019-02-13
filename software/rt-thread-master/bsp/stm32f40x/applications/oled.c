#include "init.h"
	
/*---------------------- Constant / Macro Definitions -----------------------*/
extern struct SAcc stcAcc;

extern u8 Flash_Logo[];
extern int page_num ;

typedef enum
{
		StatusPage,
		GyroscopePage,
		FlashPage,   
		PicturePage

}
MENU_LIST_e; //�˵�

char *PageName[20] = {"StatusPage","GyroscopePage","FlashPage","PicturePage"};

volatile MENU_LIST_e MENU = StatusPage; //OLED��ʼҳ��Ϊ ״̬ҳ. volatile��һ���������η���
//volatile ������ ����Ϊָ��ؼ��֣�ȷ������ָ�������������Ż���ʡ�ԣ���Ҫ��ÿ��ֱ�Ӷ�ֵ��
/*----------------------- Function Implement --------------------------------*/

void menu_define(void) //�˵�����
{
	static int page_change = 0; //�ݴ�ҳ��

	if(page_num > 3) page_num = 0;
	if(page_num < 0) page_num = 0;
	if(page_change != page_num){
			buzzer_bibi(1,1);
			rt_kprintf("\n Current Menu_Page: %s \n",PageName[page_num]);
			OLED_Clear();
	}
	page_change = page_num;

	switch(page_num){
			case 0:{
				MENU = StatusPage;OLED_StatusPage();			break;
			}
			case 1:{
				MENU = GyroscopePage;OLED_GyroscopePage();break;
			}
			case 2:{
				MENU = FlashPage;		 											break;
			}
			case 3:{
				MENU = PicturePage;											  break;
			}	
	}
}

	
void oled_thread_entry(void* parameter)
{


	Boot_Animation();	//��������
	OLED_Clear();
	
	while(1)
	{	
			menu_define();
			rt_thread_mdelay(100);
	}
	
}

void OLED_StatusPage(void)
{
		OLED_ShowString(0,0, (u8 *)"Mode:",12); 
	  OLED_ShowString(0,16,(u8 *) "Vol:",12); 	
	
		if(boma_value_get() != AUV_Mode){
				OLED_ShowString(50,0,(u8 *)"[ ROV ]",12);
		}
		else {
  			OLED_ShowString(50,0,(u8 *)"[ AUV ]",12);			
		}		
		//��ʾ�¶� 
		OLED_ShowString(0,48,(u8 *)"Temperature:",12);
		OLED_ShowNum(75,48,stcAcc.T/0x100,2,12); 
		OLED_ShowString(90,48,(u8 *)" C",12);
	
		OLED_Refresh_Gram();//������ʾ��OLED
}



/* �������� */
void Boot_Animation(void)
{
		static u8 x=0,y=0;

		for(x = 63;x>=18;x--){
				OLED_DrawPoint(108-0.7*x,x,1);//��б��
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

/* OLED �̳߳�ʼ�� */
int oled_thread_init(void)
{
    rt_thread_t oled_tid;
		/*������̬�߳�*/
    oled_tid = rt_thread_create("oled", //�߳�����
                    oled_thread_entry,	//�߳���ں�����entry��
                    RT_NULL,				    //�߳���ں���������parameter��
                    1024,							  //�߳�ջ��С����λ���ֽڡ�byte��
                    10,								  //�߳����ȼ���priority��
                    10);							  //�̵߳�ʱ��Ƭ��С��tick��= 100ms

    if (oled_tid != RT_NULL){
				OLED_Init();
				LOG_I("OLED_Init()");
				rt_thread_startup(oled_tid);
		}
		return 0;
}
INIT_APP_EXPORT(oled_thread_init);



/* OLED ��һҳ */
void next_oled_page(void)
{
		//���ݴ����string��
		buzzer_once();	
		page_num ++;
}
MSH_CMD_EXPORT(next_oled_page,next_oled_page[page++]);

/* OLED ��һҳ */
void last_oled_page(void)
{
		buzzer_once();	
		page_num --;
}
MSH_CMD_EXPORT(last_oled_page,last_oled_page[page--]);



