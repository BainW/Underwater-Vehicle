#ifndef __DEBUG_
#define __DEBUG_

#include "sys.h"


extern char *debug_tool_name[3];


/* VCANɽ����λ������ BEGIN */
void Vcan_Send_Cmd(void *wareaddr, unsigned int waresize);
void Vcan_Send_Data(void);


#endif


