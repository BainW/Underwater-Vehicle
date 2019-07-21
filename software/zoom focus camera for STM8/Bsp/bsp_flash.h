/**
  **********************************  STM8S  ***********************************
  * @�ļ���     �� bsp_flash.h
  * @����       �� strongerHuang
  * @��汾     �� V2.2.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2017��05��18��
  * @ժҪ       �� �ڲ�FLASHͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _BSP_FLASH_H
#define _BSP_FLASH_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm8s.h"


/* �궨�� --------------------------------------------------------------------*/

/* �������� ------------------------------------------------------------------*/
void FLASH_Initializes(void);
void FLASH_WriteNByte(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t nByte);
void FLASH_ReadNByte(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t nByte);


#endif /* _BSP_FLASH_H */

/**** Copyright (C)2017 strongerHuang. All Rights Reserved **** END OF FILE ****/
