#ifndef __DRV_ANO_
#define __DRV_ANO_

#include "sys.h"
#include "PID.h"
#include "flash.h"

extern Vector3f_pid PID_Parameter[PID_USE_NUM];

void ANO_SEND_StateMachine(void);//��������ѭ������

void ANO_Data_Send_Version(void);//���ͻ�����Ϣ����̬������״̬����
void ANO_Data_Send_Status(void);//���ͻ�����Ϣ����̬������״̬��

void ANO_Data_Send_Voltage_Current(void);//���͵�ѹ����


void ANO_DT_Data_Receive_Prepare(u8 data);//ANO����վ���ݽ���
void ANO_DT_Data_Receive_Anl(u8 *data_buf,u8 num);//ANO���ݽ���


void Save_Or_Reset_PID_Parameter(void);











#endif




