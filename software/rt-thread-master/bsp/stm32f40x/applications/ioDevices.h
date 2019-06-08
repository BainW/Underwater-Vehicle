#ifndef __IO_DEVICES_H
#define __IO_DEVICES_H

#include "DataType.h"
typedef struct 
{
		uint8 time;
		uint8 count;
		uint8 number;	
}Buzzer_Type;

uint8 boma_value_get(void);
uint8 wifi_connect_get(void);


void Buzzer_Init(void);
	
void buzzer_once(void);

/* countΪ��Ĵ���  length���ʱ�䳤��  */
void Buzzer_Set(Buzzer_Type* buzzer,uint8 count,uint8 length);

void Buzzer_Process(Buzzer_Type * buzzer);

extern Buzzer_Type Beep;



#endif



