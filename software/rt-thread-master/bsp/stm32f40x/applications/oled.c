#include "init.h"
	


/*---------------------- Constant / Macro Definitions -----------------------*/
extern unsigned char Flash_Logo[];



void Constant_Character()  //�̶���Ļ��ʾ����ʼ��һ�μ��� ��ռ����Դ
{
		OLED_ShowString(0,0, "Mode:",12); 
	
	
	
	
}
	
void oled_thread_entry(void* parameter)
{
	OLED_Init();
	rt_kprintf("OLED_Init()\n");
	Boot_Animation();	
	OLED_Clear();

	Constant_Character(); //�̶���Ļ��ʾ����ʼ��һ�μ��� ��ռ����Դ


	while(1)
	{
			if(boma_value_get() != AUV_Mode){
					OLED_ShowString(50,0,"[ ROV ]",12);
			}
			else {
					OLED_ShowString(50,0,"[ AUV ]",12);				
			}
		  OLED_Refresh_Gram();//������ʾ��OLED

			rt_thread_delay(30);
	}
	
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

		OLED_ShowString(60,20,"E",16);
	  OLED_Refresh_Gram();//������ʾ��OLED
		rt_thread_delay(100);
		

	
	
}

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

    if (oled_tid != RT_NULL)
     rt_thread_startup(oled_tid);
		return 0;
}
INIT_APP_EXPORT(oled_thread_init);







