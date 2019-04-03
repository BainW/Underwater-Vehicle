#include "init.h"
#include "drv_MS5837.h"
#include "drv_i2c.h"
#include <math.h>
#include <rtthread.h>
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

float MS5837_Pressure_Init = 0.0f;

uint32 Cal_C[7];	        //���ڴ��PROM�е�6������1-6
int64  dT;
float  MS_TEMP;
int64  D1_Pres,D2_Temp;	// ����ѹ��ֵ,�����¶�ֵ


/*
OFF ʵ���¶Ȳ���
SENS ʵ���¶�������
*/
uint64  OFf,SENS;
uint32  MS5837_Pressure,Pressure_old,qqp,Wdodo;				//����ѹ
uint32  TEMP2,T2,OFF2,SENS2,OFF3,SENS3;	//�¶�У��ֵ



/**
  * @brief  MS583703BA ��λ
  * @param  None
  * @retval None
  */
void MS583703BA_RESET(void)
{
		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress);//CSB�ӵأ�������ַ��0XEE������ 0X77
	  IIC_Wait_Ack();
    IIC_Send_Byte(MS583703BA_RST);//���͸�λ����
	  IIC_Wait_Ack();
    IIC_Stop();
	
}
/**
  * @brief  MS5837_CRC4У��(4bitУ��)
  * @param  MS5837 PROM�궨��������
  * @retval ����CRCУ����
  */
uint8 MS5837_CRC4(uint32 n_prom[]) // n_prom defined as 8x unsigned int (n_prom[8])
{
		int32  cnt; // simple counter
		uint32 n_rem=0; // crc remainder
		uint8  n_bit;
		n_prom[0]=((n_prom[0]) & 0x0FFF); // CRC byte is replaced by 0
		n_prom[7]=0; // Subsidiary value, set to 0
		for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
		{ 			// choose LSB or MSB
				if (cnt%2==1) n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
				else n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
				for (n_bit = 8; n_bit > 0; n_bit--)
				{
						if (n_rem & (0x8000)) n_rem = (n_rem << 1) ^ 0x3000;
						else n_rem = (n_rem << 1);
				}
		}
		n_rem= ((n_rem >> 12) & 0x000F); // final 4-bit remainder is CRC code
		return (n_rem ^ 0x00);
}


/**
  * @brief  MS5837_Get_PROM
  * @param  None
  * @retval ����MS5837_Get_PROM(�����궨����)�Ƿ�ɹ���־��1�ɹ���0ʧ��
  */
uint8 MS5837_Get_PROM(void)
{	 
		u8  inth,intl,i;
		u8  CRC_Check = 0;
		for (i=0;i<=6;i++) 
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
				//rt_kprintf("Cal_C[%d]:%d\r\n",i,Cal_C[i]);
		}
		CRC_Check = (u8)((Cal_C[0]&0xF000)>>12);
		rt_kprintf("CRC:%d   CRC_Check:%d\r\n",MS5837_CRC4(Cal_C),CRC_Check);

		if(CRC_Check == MS5837_CRC4(Cal_C)){

				return 1;
		}
		else {return 0;}
}



/**
  * @brief  MS5837_Init
  * @param  None
  * @retval ��ʼ���Ƿ�ɹ���־��1�ɹ���0ʧ��
  */
uint8 MS5837_Init(void){
		
		uint8 MS5837_Init_Flag = 0;
	
		IIC_Init();	         //��ʼ��IIC PD0 PD1
		rt_thread_mdelay(100);
		MS583703BA_RESET();	 // Reset Device  ��λMS5837
		rt_thread_mdelay(100);
	
		if(MS5837_Get_PROM()){//��ȡPROM����
				MS5837_Init_Flag = 1; //��ʼ���ɹ�
				//rt_kprintf("MS5837_Init Success!\r\n"); 
		}
		else {
				MS5837_Init_Flag = 0; //��ʼ��ʧ��
				return 0;
				//rt_kprintf("MS5837_Init Failed!\r\n");
		}
		rt_thread_mdelay(10);
		
		MS583703BA_getTemperature();
		rt_kprintf("MS5837_Pressure_Init:%d\r\n",MS583703BA_getPressure());
		return MS5837_Init_Flag;
}

/**
  * @brief  MS583703BAת�����
  * @param  ����ֵ(�¶ȡ���ѹ)
  * @retval ����MS5837��ʼ���Ƿ�ɹ���־��1�ɹ���0ʧ��
  */
uint64 MS583703BA_getConversion(uint8_t command)
{
		uint64 conversion = 0;
		uint8 temp[3];

		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress); 		//д��ַ
		IIC_Wait_Ack();
		IIC_Send_Byte(command); //дת������
		IIC_Wait_Ack();
		IIC_Stop();

		rt_hw_ms_delay(20);  //��ȡ8196ת��ֵ�ùؼ����������PDF-2ҳ�е�18.08����

		IIC_Start();
		IIC_Send_Byte(MS583703BA_SlaveAddress); //д��ַ
		IIC_Wait_Ack();
		IIC_Send_Byte(0);			//����ת��
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


/**
  * @brief  MS583703BAת���¶Ƚ��
  * @param  None
  * @retval None
  */
void MS583703BA_getTemperature(void)
{
	D2_Temp = MS583703BA_getConversion(MS583703BA_D2_OSR_2048);//4096  ���������Լ��(300+)     
	
	dT=D2_Temp - (((uint32)Cal_C[5])*256);
	MS_TEMP=2000+dT*((uint32)Cal_C[6])/8388608;  //�������ڴ˴�û�г��ָ���
}

/**
  * @brief  MS583703BAת����ѹ���
  * @param  None
  * @retval None
  */
u32 MS583703BA_getPressure(void)
{
		D1_Pres= MS583703BA_getConversion(MS583703BA_D1_OSR_4096);//2048
		OFF_=(uint32_t)Cal_C[2]*65536+((uint32_t)Cal_C[4]*dT)/128;
		SENS=(uint32_t)Cal_C[1]*32768+((uint32_t)Cal_C[3]*dT)/256;

		if(MS_TEMP<2000)  // LOW Temperature
		{
				Aux = (2000-MS_TEMP)*(2000-MS_TEMP);
				T2 = 3*(dT*dT) /0x80000000; 
				OFF2 = (uint32_t)1.5*Aux;
				SENS2 = 5*Aux/8;
				
				OFF3 = OFF_ - OFF2;
				SENS3 = SENS - SENS2;	
		}
		else{
				Aux = (2000-MS_TEMP)*(2000-MS_TEMP);
				T2=2*(dT*dT)/137438953472;
				OFF2 = 1*Aux/16;
				SENS2 = 0;
				OFF3 = OFF_ - OFF2;
				SENS3 = SENS - SENS2;	
			 
		}
		MS5837_Pressure = ((D1_Pres*SENS3/2097152-OFF3)/8192)/10;
		MS_TEMP = (MS_TEMP-T2)/100;
		
		return MS5837_Pressure;
}

