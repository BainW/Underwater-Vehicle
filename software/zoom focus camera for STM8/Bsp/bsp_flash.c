/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� bsp_flash.c
  * @����       �� strongerHuang
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2017��05��18��
  * @ժҪ       �� �ڲ�FLASHԴ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2017-05-18 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "bsp_flash.h"

/************************************************
�������� �� FLASH_Initializes
��    �� �� �ڲ�FLASH��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void FLASH_Initializes(void)
{
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);
}

/************************************************
�������� �� FLASH_WriteNByte
��    �� �� дFlash
��    �� �� pBuffer ----- д���������׵�ַ
            WriteAddr --- Ҫд��Flash�ĵ�ַ
            nByte ------- Ҫд����ֽ���
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void FLASH_WriteNByte(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t nByte)
{
  FLASH_Unlock(FLASH_MEMTYPE_PROG);              //����
  while(FLASH_GetFlagStatus(FLASH_FLAG_PUL) == RESET);
  while(nByte--)
  {
    FLASH_ProgramByte(WriteAddr, *pBuffer);
    WriteAddr++;
    pBuffer++;
    FLASH_WaitForLastOperation(FLASH_MEMTYPE_PROG);
  }
  FLASH_Lock(FLASH_MEMTYPE_PROG);                //����
}


/************************************************
�������� �� FLASH_ReadNByte
��    �� �� ��Flash
��    �� �� pBuffer ---- ����buf
            ReadAddr --- Ҫ��ȡFlash���׵�ַ
            nByte ------ Ҫ��ȡ���ֽ���
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void FLASH_ReadNByte(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t nByte)
{
  while(nByte--)
  {
    *pBuffer = FLASH_ReadByte(ReadAddr);
    ReadAddr++;
    pBuffer++;
  }
}

/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
