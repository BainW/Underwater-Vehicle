#ifndef __BUZZER_H
#define __BUZZER_H

#include "DataType.h"


void Buzzer_Init(void);
	
void buzzer_once(void);

/* countΪ��Ĵ���  length���ʱ�䳤��  */
void buzzer_bibi(uint8 count,uint8 length);

void buzzer_ring_task(void);


#endif



