/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� bsp_i2c.c
  * @����       �� strongerHuang
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2017��06��29��
  * @ժҪ       �� I2C����Դ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2017-06-29 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "bsp_i2c.h"


/************************************************
�������� �� I2C_Delay
��    �� �� I2C��ʱ(�Ǳ�׼��ʱ,�����MCU�ٶ� ���ڴ�С)
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
static void I2C_Delay(void)
{
  uint16_t cnt = 10;

  while(cnt--);
}

/************************************************
�������� �� I2C_GPIO_Configuration
��    �� �� I2C��������(��©���)
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void I2C_GPIO_Configuration(void)
{
  GPIO_Init(PORT_I2C_SCL, (GPIO_Pin_TypeDef)PIN_I2C_SCL, GPIO_MODE_OUT_OD_HIZ_FAST);
  GPIO_Init(PORT_I2C_SDA, (GPIO_Pin_TypeDef)PIN_I2C_SDA, GPIO_MODE_OUT_OD_HIZ_FAST);
}

/************************************************
�������� �� I2C_Initializes
��    �� �� I2C��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void I2C_Initializes(void)
{
  I2C_GPIO_Configuration();

  I2C_SCL_HIGH;                                  //��λ״̬
  I2C_SDA_HIGH;
}

/************************************************
�������� �� I2C_Start
��    �� �� I2C��ʼ
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void I2C_Start(void)
{
  I2C_SCL_HIGH;                                  //SCL��
  I2C_Delay();

  I2C_SDA_HIGH;                                  //SDA�� -> ��
  I2C_Delay();
  I2C_SDA_LOW;                                   //SDA��
  I2C_Delay();

  I2C_SCL_LOW;                                   //SCL��(��д��ַ/����)
  I2C_Delay();
}

/************************************************
�������� �� I2C_Stop
��    �� �� I2Cֹͣ
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void I2C_Stop(void)
{
  I2C_SDA_LOW;                                   //SDA�� -> ��
  I2C_Delay();

  I2C_SCL_HIGH;                                  //SCL��
  I2C_Delay();

  I2C_SDA_HIGH;                                  //SDA��
  I2C_Delay();
}

/************************************************
�������� �� I2C_PutAck
��    �� �� I2C��������Ӧ��(���Ӧ��)λ
��    �� �� I2C_ACK ----- Ӧ��
            I2C_NOACK --- ��Ӧ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void I2C_PutAck(uint8_t Ack)
{
  I2C_SCL_LOW;                                   //SCL��
  I2C_Delay();

  if(I2C_ACK == Ack)
    I2C_SDA_LOW;                                 //Ӧ��
  else
    I2C_SDA_HIGH;                                //��Ӧ��
  I2C_Delay();

  I2C_SCL_HIGH;                                  //SCL�� -> ��
  I2C_Delay();
  I2C_SCL_LOW;                                   //SCL��
  I2C_Delay();
}

/************************************************
�������� �� I2C_GetAck
��    �� �� I2C������ȡӦ��(���Ӧ��)λ
��    �� �� ��
�� �� ֵ �� I2C_ACK ----- Ӧ��
            I2C_NOACK --- ��Ӧ��
��    �� �� strongerHuang
*************************************************/
uint8_t I2C_GetAck(void)
{
  uint8_t ack;

  I2C_SCL_LOW;                                   //SCL�� -> ��
  I2C_Delay();

  I2C_SDA_HIGH;                                  //�ͷ�SDA(��©ģʽ��Ч)
  I2C_Delay();

  I2C_SCL_HIGH;                                  //SCL��(��ȡӦ��λ)
  I2C_Delay();

  if(I2C_SDA_READ)
    ack = I2C_NOACK;                             //��Ӧ��
  else
    ack = I2C_ACK;                               //Ӧ��

  I2C_SCL_LOW;                                   //SCL��
  I2C_Delay();

  return ack;
}

/************************************************
�������� �� I2C_WriteByte
��    �� �� I2Cдһ�ֽ�
��    �� �� Data --- ����
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void I2C_WriteByte(uint8_t Data)
{
  uint8_t cnt;

  for(cnt=0; cnt<8; cnt++)
  {
    I2C_SCL_LOW;                                 //SCL��(SCLΪ�͵�ƽʱ�仯SDA��Ч)
    I2C_Delay();

    if(Data & 0x80)
      I2C_SDA_HIGH;                              //SDA��
    else
      I2C_SDA_LOW;                               //SDA��
    Data <<= 1;
    I2C_Delay();

    I2C_SCL_HIGH;                                //SCL��(��������)
    I2C_Delay();
  }
  I2C_SCL_LOW;                                   //SCL��(�ȴ�Ӧ���ź�)
  I2C_Delay();

  I2C_GetAck();                                  //��ȡӦ��λ
}

/************************************************
�������� �� I2C_ReadByte
��    �� �� I2C��һ�ֽ�
��    �� �� ack --------- ����Ӧ��(���߷�Ӧ��)λ
�� �� ֵ �� data -------- ��ȡ��һ�ֽ�����
��    �� �� strongerHuang
*************************************************/
uint8_t I2C_ReadByte(uint8_t ack)
{
  uint8_t cnt;
  uint8_t data;

  I2C_SCL_LOW;                                   //SCL��
  I2C_Delay();

  I2C_SDA_HIGH;                                  //�ͷ�SDA(��©ģʽ��Ч)

  for(cnt=0; cnt<8; cnt++)
  {
    I2C_SCL_HIGH;                                //SCL��(��ȡ����)
    I2C_Delay();

    data <<= 1;
    if(I2C_SDA_READ)
      data |= 0x01;                              //SDAΪ��(������Ч)

    I2C_SCL_LOW;                                 //SCL��
    I2C_Delay();
  }

  I2C_PutAck(ack);                               //����Ӧ��(���߷�Ӧ��)λ

  return data;                                   //��������
}


/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
