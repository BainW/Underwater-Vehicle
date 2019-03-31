#include "init.h"
#include "drv_MS5837.h"
#include "math.h"

 
/*
C1 ѹ�������� SENS|T1
C2  ѹ������  OFF|T1
C3	�¶�ѹ��������ϵ�� TCS
C4	�¶�ϵ����ѹ������ TCO
C5	�ο��¶� T|REF
C6 	�¶�ϵ�����¶� TEMPSENS
*/


double OFF_;
float Aux;
/*
dT ʵ�ʺͲο��¶�֮��Ĳ���
MS_TEMP ʵ���¶�	
*/

int32_t  Cal_C[7];	        //���ڴ��PROM�е�6������1-6
int64_t dT;
float MS_TEMP;
int64_t D1_Pres,D2_Temp;	// ����ѹ��ֵ,�����¶�ֵ


/*
OFF ʵ���¶Ȳ���
SENS ʵ���¶�������
*/
uint64_t OFf,SENS;
uint32_t Pressure,Pressure_old,qqp,Wdodo;				//����ѹ
uint32_t TEMP2,T2,OFF2,SENS2;	//�¶�У��ֵ
uint32_t Pres_BUFFER[20];     //������
uint32_t Temp_BUFFER[10];     //������
uint32_t depth;




/*******************************************************************************
  * @��������	MS561101BA_RESET
  * @����˵��   ��λMS5611
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
void MS583703BA_RESET(void)
{
		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress);//CSB�ӵأ�������ַ��0XEE������ 0X77
	  IIC_Wait_Ack();
    IIC_Send_Byte(MS583703BA_RST);//���͸�λ����
	  IIC_Wait_Ack();
    IIC_Stop();
	
}
/*******************************************************************************
  * @��������	MS5611_init
  * @����˵��   ��ʼ��5611
  * @�������  	��
  * @�������   ��
  * @���ز���   ��
*******************************************************************************/
u8 MS5837_init(void)
 {	 
  u8  inth,intl;
  int i;
  for (i=1;i<=6;i++) 
	{
 
		IIC_Start();
    IIC_Send_Byte(MS583703BA_SlaveAddress);
		IIC_Wait_Ack();
		IIC_Send_Byte(0xA0 + (i*2));
		IIC_Wait_Ack();
    IIC_Stop();

		rt_hw_us_delay(5);
		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress+0x01);  //�������ģʽ

		rt_hw_us_delay(1);
		IIC_Wait_Ack();
		inth = IIC_Read_Byte(1);  		//��ACK�Ķ�����
		
		rt_hw_us_delay(1);
		intl = IIC_Read_Byte(0); 			//���һ���ֽ�NACK
		IIC_Stop();
    Cal_C[i] = (((uint16_t)inth << 8) | intl);
		rt_kprintf("Cal_C[%d]:%d\r\n",i,Cal_C[i]);
	}
	return !Cal_C[0];
 }


/**************************ʵ�ֺ���********************************************
*����ԭ��:unsigned long MS561101BA_getConversion(void)
*��������:    ��ȡ MS561101B ��ת����� 
*******************************************************************************/
unsigned long MS583703BA_getConversion(uint8_t command)
{
		unsigned long conversion = 0;
		u8 temp[3];

		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress); 		//д��ַ
		IIC_Wait_Ack();
		IIC_Send_Byte(command); //дת������
		IIC_Wait_Ack();
		IIC_Stop();

		rt_hw_ms_delay(30);  //��ȡ8196ת��ֵ�ùؼ����������PDF-2ҳ�е�18.08����

		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress); 		//д��ַ
		IIC_Wait_Ack();
		IIC_Send_Byte(0);				// start read sequence
		IIC_Wait_Ack();
		IIC_Stop();
	 
		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress+0x01);  //�������ģʽ
		IIC_Wait_Ack();
		temp[0] = IIC_Read_Byte(1);  //��ACK�Ķ�����  bit 23-16
		temp[1] = IIC_Read_Byte(1);  //��ACK�Ķ�����  bit 8-15
		temp[2] = IIC_Read_Byte(0);  //��NACK�Ķ����� bit 0-7
		IIC_Stop();

		conversion = (unsigned long)temp[0] << 16 | (unsigned long)temp[1] <<8 | (unsigned long)temp[2];
		return conversion;
}


/**************************ʵ�ֺ���********************************************
*����ԭ��:void MS561101BA_GetTemperature(void)
*��������:    ��ȡ �¶�ת����� 
*******************************************************************************/

void MS583703BA_getTemperature(void)
{
	D2_Temp = MS583703BA_getConversion(MS583703BA_D2_OSR_2048);
	
	dT=D2_Temp - (((uint32_t)Cal_C[5])*256);
	MS_TEMP=2000+dT*((uint32_t)Cal_C[6])/8388608;  //�������ڴ˴�û�г��ָ���

}

/***********************************************
  * @brief  ��ȡ��ѹ
  * @param  None
  * @retval None
  * ��ȷ�� 0.1mbar = 10pa
************************************************/
void MS583703BA_getPressure(void)
{
	D1_Pres= MS583703BA_getConversion(MS583703BA_D1_OSR_2048);//2048
	
	OFF_=(uint32_t)Cal_C[2]*65536+((uint32_t)Cal_C[4]*dT)/128;
	SENS=(uint32_t)Cal_C[1]*32768+((uint32_t)Cal_C[3]*dT)/256;

	if(MS_TEMP<2000)  // LOW Temperature
	{
		Aux = (2000-MS_TEMP)*(2000-MS_TEMP);
		T2 = 3*(dT*dT) /0x80000000; 
		OFF2 = (uint32_t)1.5*Aux;
		SENS2 = 5*Aux/8;
		
		OFF_ = OFF_ - OFF2;
		SENS = SENS - SENS2;	
	}
	else{
		Aux = (2000-MS_TEMP)*(2000-MS_TEMP);
	  T2=2*(dT*dT)/137438953472;
		OFF2 = 1*Aux/16;
		SENS2 = 0;
		OFF_ = OFF_ - OFF2;
		SENS = SENS - SENS2;	
		 
	}
  Pressure= ((D1_Pres*SENS/2097152-OFF_)/8192)/10;
	MS_TEMP=(MS_TEMP-T2)/100;
}

