#ifndef __KEY_H
#define __KEY_H	 

#define KEY0  GPIO_ReadInputPin(GPIOC,GPIO_PIN_3)//��ȡ����0
#define KEY1  GPIO_ReadInputPin(GPIOB,GPIO_PIN_4)//��ȡ����1
#define KEY2 GPIO_ReadInputPin(GPIOB,GPIO_PIN_5)//��ȡ����3(WK_UP) 

 

#define KEY0_PRES   1	//KEY0����
#define KEY1_PRES   2	//KEY1����
#define KEY2_PRES   3	//KEY_UP����(��WK_UP/KEY_UP)


void KEY_Init(void);//IO��ʼ��
char KEY_Scan(char);  	//����ɨ�躯��					    
#endif
