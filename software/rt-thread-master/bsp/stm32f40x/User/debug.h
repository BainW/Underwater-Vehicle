#ifndef __DEBUG_
#define __DEBUG_

#include "sys.h"

void ANO_SEND_StateMachine(void);//��������ѭ������

void ANO_Data_Send_Status(void);//���ͻ�����Ϣ����̬������״̬��
void Vcan_Send_Data(void);
void Vcan_Send_Cmd(void *wareaddr, unsigned int waresize);


void ANO_DT_Data_Receive_Prepare(u8 data);//ANO����վ���ݽ���
void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num);//ANO���ݽ���

void Save_Or_Reset_PID_Parameter(void); //PID�����ڲ�Flash����д����


extern char *debug_tool_name[3];





#endif


