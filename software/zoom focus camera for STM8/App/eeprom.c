/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� eeprom.c
  * @����       �� strongerHuang
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2017��06��29��
  * @ժҪ       �� EEPROMӦ��Դ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2017-06-29 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "eeprom.h"
#include "bsp_i2c.h"
#include "bsp_timer.h"


/**************************** д��1Byte ****************************/
/************************************************
�������� �� EEPROM_WriteByte
��    �� �� EEPROMдһ�ֽ�
��    �� �� Addr --- ��ַ
            Data --- ����
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void EEPROM_WriteByte(uint16_t Addr, uint8_t Data)
{
  /* 1.��ʼ */
  I2C_Start();

  /* 2.�豸��ַ/д */
  I2C_WriteByte(EEPROM_DEV_ADDR | EEPROM_WR);

  /* 3.���ݵ�ַ */
#if (8 == EEPROM_WORD_ADDR_SIZE)
  I2C_WriteByte((uint8_t)(Addr&0x00FF));         //���ݵ�ַ(8λ)
#else
  I2C_WriteByte((uint8_t)(Addr>>8));             //���ݵ�ַ(16λ)
  I2C_WriteByte((uint8_t)(Addr&0x00FF));
#endif

  /* 4.дһ�ֽ����� */
  I2C_WriteByte(Data);

  /* 5.ֹͣ */
  I2C_Stop();
}

/************************************************
�������� �� EEPROM_ReadByte
��    �� �� EEPROM��һ�ֽ�
��    �� �� Addr --- ��ַ
            data --- ����
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void EEPROM_ReadByte(uint16_t Addr, uint8_t *Data)
{
  /* 1.��ʼ */
  I2C_Start();

  /* 2.�豸��ַ/д */
  I2C_WriteByte(EEPROM_DEV_ADDR | EEPROM_WR);

  /* 3.���ݵ�ַ */
#if (8 == EEPROM_WORD_ADDR_SIZE)
  I2C_WriteByte((uint8_t)(Addr&0x00FF));         //���ݵ�ַ(8λ)
#else
  I2C_WriteByte((uint8_t)(Addr>>8));             //���ݵ�ַ(16λ)
  I2C_WriteByte((uint8_t)(Addr&0x00FF));
#endif

  /* 4.���¿�ʼ */
  I2C_Start();

  /* 5.�豸��ַ/�� */
  I2C_WriteByte(EEPROM_DEV_ADDR | EEPROM_RD);

  /* 6.��һ�ֽ����� */
  *Data = I2C_ReadByte(I2C_NOACK);               //ֻ��ȡ1�ֽ�(������Ӧ��)

  /* 7.ֹͣ */
  I2C_Stop();
}

/**************************** д��NByte ****************************/
/************************************************
�������� �� EEPROM_WritePage
��    �� �� EEPROMдҳ
��    �� �� Addr ---- ��ַ
            pData --- ����
            Length --- ����(<=EEPROM_PAGE_SIZE)
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void EEPROM_WritePage(uint16_t Addr, uint8_t *pData, uint8_t Length)
{
  uint16_t cnt;

  /* 1.��ʼ */
  I2C_Start();

  /* 2.�豸��ַ/д */
  I2C_WriteByte(EEPROM_DEV_ADDR | EEPROM_WR);

  /* 3.���ݵ�ַ */
#if (8 == EEPROM_WORD_ADDR_SIZE)
  I2C_WriteByte((uint8_t)(Addr&0x00FF));         //���ݵ�ַ(8λ)
#else
  I2C_WriteByte((uint8_t)(Addr>>8));             //���ݵ�ַ(16λ)
  I2C_WriteByte((uint8_t)(Addr&0x00FF));
#endif

  /* 4.����д���� */
  for(cnt=0; cnt<(Length-1); cnt++)
  {
    I2C_WriteByte(*pData);
    pData++;
  }
  I2C_WriteByte(*pData);

  /* 5.ֹͣ */
  I2C_Stop();
}

/************************************************
�������� �� EEPROM_WriteNByte
��    �� �� EEPROMдN�ֽ�
��    �� �� Addr ----- ��ַ
            pData ---- ����
            Length --- ����
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void EEPROM_WriteNByte(uint16_t Addr, uint8_t *pData, uint16_t Length)
{
  uint16_t addr_offset;                          //ƫ�Ƶ�ַ
  uint8_t  num_page = 0;                         //ҳ��(Length�ֽڹ�����ҳ)
  uint8_t  num_single = 0;                       //"��"�ֽ���(������ҳ����ֽ���)
  uint8_t  count = 0;                            //ҳʣ���ֽ�����(ƫ�Ƶ�ַ -> ҳĩ)

  addr_offset = Addr % EEPROM_PAGE_SIZE;         //"��ʼ��ַ"ƫ�Ƹ�ҳ��ַ����
  count = EEPROM_PAGE_SIZE - addr_offset;        //ҳʣ���ֽ�����
  num_page = Length / EEPROM_PAGE_SIZE;          //ҳ��
  num_single = Length % EEPROM_PAGE_SIZE;

  /* 1.��ʼ��ַδƫ��(λ��ҳ�׵�ַ) */
  if(0 == addr_offset)
  {
    /* ������С��1ҳ */
    if(0 == num_page)
    {
      EEPROM_WritePage(Addr, pData, Length);     //ҳ�׵�ַ,д��С��1ҳ������
    }
    /* ���������ڵ���1ҳ */
    else
    {
      while(num_page--)
      {                                          //дnum_pageҳ����
        EEPROM_WritePage(Addr, pData, EEPROM_PAGE_SIZE);
        Addr += EEPROM_PAGE_SIZE;
        pData += EEPROM_PAGE_SIZE;
      }
      if(0 != num_single)
      {                                          //д��ҳ��ʣ�µ��ֽ���
        EEPROM_WritePage(Addr, pData, num_single);
      }
    }
  }

  /* 2.��ʼ��ַ��ƫ��(����ҳ�׵�ַ) */
  else
  {
    /* ������С��1ҳ */
    if(0 == num_page)
    {
      /* ��������ҳ */
      if(Length < count)
      {
        EEPROM_WritePage(Addr, pData, Length);   //ҳƫ�Ƶ�ַ,д��С�ڸ�ҳ������
      }
      /* ������ҳ */
      else
      {
        EEPROM_WritePage(Addr, pData, count);    //ҳƫ�Ƶ�ַ,д����ҳ������
        Addr += count;
        pData += count;
                                                 //��ҳ�׵�ַ,д��ʣ�µ�����
        EEPROM_WritePage(Addr, pData, Length - count);
      }
    }
    /* ���������ڵ���1ҳ */
    else
    {
      Length -= count;
      num_page = Length / EEPROM_PAGE_SIZE;      //ʣ�µ�ҳ��(��ȥǰ��д����)
      num_single = Length % EEPROM_PAGE_SIZE;    //���һҳ��Ҫд���ֽ���

      EEPROM_WritePage(Addr, pData, count);      //ҳƫ�Ƶ�ַ,д����ҳ������
      Addr += count;
      pData += count;

      while(num_page--)
      {                                          //дnum_pageҳ����
        TIMDelay_Nms(5);                         //д������ʱ5ms
        EEPROM_WritePage(Addr, pData, EEPROM_PAGE_SIZE);
        Addr += EEPROM_PAGE_SIZE;
        pData += EEPROM_PAGE_SIZE;
      }
      if(0 != num_single)
      {                                          //д��ҳ��ʣ�µ��ֽ���
        TIMDelay_Nms(5);
        EEPROM_WritePage(Addr, pData, num_single);
      }
    }
  }
}

/************************************************
�������� �� EEPROM_ReadNByte
��    �� �� EEPROM��N�ֽ�
��    �� �� Addr ----- ��ַ
            pData ---- ����
            Length --- ����
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void EEPROM_ReadNByte(uint16_t Addr, uint8_t *pData, uint16_t Length)
{
  uint16_t cnt;

  /* 1.��ʼ */
  I2C_Start();

  /* 2.�豸��ַ/д */
  I2C_WriteByte(EEPROM_DEV_ADDR | EEPROM_WR);

  /* 3.���ݵ�ַ */
#if (8 == EEPROM_WORD_ADDR_SIZE)
  I2C_WriteByte((uint8_t)(Addr&0x00FF));         //���ݵ�ַ(8λ)
#else
  I2C_WriteByte((uint8_t)(Addr>>8));             //���ݵ�ַ(16λ)
  I2C_WriteByte((uint8_t)(Addr&0x00FF));
#endif

  /* 4.���¿�ʼ */
  I2C_Start();

  /* 5.�豸��ַ/�� */
  I2C_WriteByte(EEPROM_DEV_ADDR | EEPROM_RD);

  /* 6.��һ�ֽ����� */
  for(cnt=0; cnt<(Length-1); cnt++)
  {
    *pData = I2C_ReadByte(I2C_ACK);              //������ȡ(Length-1)�ֽ�(����Ӧ��)
    pData++;
  }
  *pData = I2C_ReadByte(I2C_NOACK);              //��ȡ���1�ֽ�(������Ӧ��)

  /* 7.ֹͣ */
  I2C_Stop();
}


/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
