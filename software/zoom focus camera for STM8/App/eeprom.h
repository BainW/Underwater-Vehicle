/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� eeprom.h
  * @����       �� strongerHuang
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2017��06��29��
  * @ժҪ       �� EEPROMӦ��ͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _EEPROM_H
#define _EEPROM_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm8s.h"


/* �궨�� --------------------------------------------------------------------*/
#define EEPROM_DEV_ADDR           0xA0                     //��ַ(�豸��ַ:��A2��A1��A0�й�)

#define EEPROM_WR                 0x00                     //д
#define EEPROM_RD                 0x01                     //��

#define AT24C64                                            //����оƬ�ͺ�(�����оƬ�ͺ��޸�)
/*
1��"��ַ����"����оƬ�ͺŲ�ͬ���в�ͬ
    8λ: AT24C01��AT24C02
   16λ: AT24C04��AT24C08��AT24C16��AT24C32��AT24C64��AT24C128��AT24C256��AT24C512

2��"ҳ����"����оƬ�ͺŲ�ͬ���в�ͬ
  8�ֽ�: AT24C01��AT24C02
 16�ֽ�: AT24C04��AT24C08��AT24C16
 32�ֽ�: AT24C32��AT24C64
 64�ֽ�: AT24C128��AT24C256
128�ֽ�: AT24C512
*/
#if defined (AT24C01) || defined (AT24C02)
  #define EEPROM_WORD_ADDR_SIZE   8                        //��ַ����
  #define EEPROM_PAGE_SIZE        8                        //ҳ����
#endif

#if defined (AT24C04) || defined (AT24C08) || defined (AT24C16)
  #define EEPROM_WORD_ADDR_SIZE   16                       //��ַ����
  #define EEPROM_PAGE_SIZE        16                       //ҳ����
#endif

#if defined (AT24C32) || defined (AT24C64)
  #define EEPROM_WORD_ADDR_SIZE   16                       //��ַ����
  #define EEPROM_PAGE_SIZE        32                       //ҳ����
#endif

#if defined (AT24C128) || defined (AT24C256)
  #define EEPROM_WORD_ADDR_SIZE   16                       //��ַ����
  #define EEPROM_PAGE_SIZE        64                       //ҳ����
#endif

#if defined (AT24C512)
  #define EEPROM_WORD_ADDR_SIZE   16                       //��ַ����
  #define EEPROM_PAGE_SIZE        128                      //ҳ����
#endif



/* �������� ------------------------------------------------------------------*/
void EEPROM_WriteByte(uint16_t Addr, uint8_t Data);
void EEPROM_ReadByte(uint16_t Addr, uint8_t *Data);

void EEPROM_WritePage(uint16_t Addr, uint8_t *pData, uint8_t Length);
void EEPROM_WriteNByte(uint16_t Addr, uint8_t *pData, uint16_t Length);
void EEPROM_ReadNByte(uint16_t Addr, uint8_t *pData, uint16_t Length);


#endif /* _EEPROM_H */

/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
