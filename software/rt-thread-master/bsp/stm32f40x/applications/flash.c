#define LOG_TAG       "flash"

#include "init.h"

#include <spi_flash.h>
#include <spi_flash_sfud.h>
#include <easyflash.h>


extern  servo_t servo_motor;
extern u8 debug_tool;


/* FLASH ����ʱ��ȡ ����ֵ*/
int Flash_Read(void)
{
		ef_port_read(FLASH_INIT_ADDRESS,&servo_motor.open_value,4);		// ��ַ0
		ef_port_read(FLASH_INIT_ADDRESS+4,&servo_motor.close_value,4);		// ��ַ4
		ef_port_read(FLASH_INIT_ADDRESS+8,(u32 *)(&debug_tool),4);		// ��ַ8
	
		log_i("Flash_Read()");
		return 0;
	
}INIT_APP_EXPORT(Flash_Read);

/* FLASH ���� ����ֵ*/
void Flash_Update(void)
{
		ef_port_erase(FLASH_INIT_ADDRESS,4);	//�Ȳ���д  ������Ϊһ������4096 Byte
		ef_port_write(FLASH_INIT_ADDRESS,&servo_motor.open_value,4);		//��0����ַ��д������
		ef_port_write(FLASH_INIT_ADDRESS+4,&servo_motor.close_value,4);	//��4����ַ��д������
	
		ef_port_write(FLASH_INIT_ADDRESS+8,(u32 *)(&debug_tool),4);		// ��ַ8
}


/* list �����Ҫ���� */
void list_value(void)
{

		Flash_Read();
		log_i	("variable name     value");
    log_i("---------------  ---------");
	  log_i("ser_OpenValue      %d",servo_motor.open_value);
	  log_i("ser_CloseValue     %d",servo_motor.close_value);
		log_i("debug_tool         %s",debug_tool_name[debug_tool]);
    rt_kprintf("                         \n");
}
MSH_CMD_EXPORT(list_value,list some important values);














