#ifndef __BUZZER_H
#define __BUZZER_H

#include "init.h"


void Buzzer_Init(void);
	
void buzzer_once(void);

/* countΪ��Ĵ���  length���ʱ�䳤��  */
void buzzer_bibi(rt_uint8_t count,rt_uint8_t length);

void buzzer_ring_task(void);


#endif



