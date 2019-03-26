#define LOG_TAG    "init"

#include "init.h"
#include <rthw.h>
#include <board.h>
#include <stdio.h>

#include <spi_flash.h>
#include <easyflash.h>
#include <elog_flash.h>
#include <spi_flash_sfud.h>

/*----------------------- Variable Declarations -----------------------------*/

volatile u8 VehicleMode = ROV_Mode;   //ROV_Mode or AUV_Mode

rt_spi_flash_device_t nor_flash;
struct rt_thread thread_sys_monitor;

/*----------------------- Function Implement --------------------------------*/

static rt_err_t exception_hook(void *context);
static void rtt_user_assert_hook(const char* ex, const char* func, rt_size_t line);

/**
 * ϵͳ����߳�
 * @param parameter parameter
 */
void thread_entry_sys_monitor(void* parameter)
{
    while (1)
    {
        IWDG_Feed(); //ι��
				rt_thread_mdelay(500);
    }
}


/**
 * ϵͳ��ʼ���߳�
 * @param parameter parameter
 */
void sys_init_thread(void* parameter){
	
	  /* �����������������󣬽������л��������̣߳�����Ӧ�ж� */
    rt_enter_critical();
	
    /* ��ʼ�� nor_flash Flash �豸 */
    if ((nor_flash = rt_sfud_flash_probe("nor_flash", "spi20")) == NULL) {
				rt_kprintf("Error! No find nor_flash!");
        return;
    }
    /* ��ʼ�� EasyFlash ģ�� */
    easyflash_init();

    /* ��ʼ����־ϵͳ */
    elog_init();
		
		/* ������־��ʽ */
    elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL & ~ELOG_FMT_P_INFO);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_P_INFO));
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG,ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);

    elog_set_filter_lvl(ELOG_LVL_TOTAL_NUM);
		
#ifdef 	ELOG_COLOR_ENABLE  
		/* ʹ����־��ɫ */
    elog_set_text_color_enabled(true);
#endif
    /* ��ʼ��EasyLogger��Flash��� */
    elog_flash_init();
		
    /* ����EasyLogger */
    elog_start();
		
		/* ����Ӳ���쳣���� */
    rt_hw_exception_install(exception_hook);
		
    /* ����RTT���Թ��� */
    rt_assert_set_hook(rtt_user_assert_hook);
		
		/* ���������� */
    rt_exit_critical();
}


/* ����Ӳ���쳣���� */
static rt_err_t exception_hook(void *context) {
    extern long list_thread(void);
    uint8_t _continue = 1;
	

    rt_enter_critical();

#ifdef RT_USING_FINSH
    list_thread();
#endif
    while (_continue == 1);
    return RT_EOK;
}


/* ����RTT���Թ��� */
static void rtt_user_assert_hook(const char* ex, const char* func, rt_size_t line) {
	

    rt_enter_critical();

#ifdef ELOG_ASYNC_OUTPUT_ENABLE
    elog_async_enabled(false);
#endif
    elog_a("rtt", "(%s) has assert failed at %s:%ld.", ex, func, line);

    while(1);
}

int rt_system_init(void)
{
    rt_thread_t sys_thread;  //ϵͳ���ֳ�ʼ���߳�
		rt_thread_t monitor_thread;  //�����߳�
				 
	  monitor_thread = rt_thread_create("monitor",
																			 thread_entry_sys_monitor, 
																			 NULL,
																			 512,
																			 30,
																			 10);
							 
    sys_thread = rt_thread_create("sys_init",
																	 sys_init_thread, 
																	 NULL,
																	 512,
																	 5,
																	 10);
	
    if (monitor_thread != NULL) {
        rt_thread_startup(monitor_thread);
    }
		else {
		  	rt_kprintf("monitoring error!");
		}
    if (sys_thread != NULL) {
        rt_thread_startup(sys_thread);
    }
		else {
				rt_kprintf("sys init error!");
		}
    return 0;
}
INIT_DEVICE_EXPORT(rt_system_init);



void rt_hw_us_delay(u32 us)
{
    rt_uint32_t delta;
    /* �����ʱ������ tick �� */
    us = us * (SysTick->LOAD/(1000000/RT_TICK_PER_SECOND));
    /* ��õ�ǰʱ�� */
    delta = SysTick->VAL;
    /* ѭ����õ�ǰʱ�䣬ֱ���ﵽָ����ʱ����˳�ѭ�� */
    while (delta - SysTick->VAL< us);
}

void rt_hw_ms_delay(u32 ms)
{
		rt_hw_us_delay(1000 * ms);
}

void delay_us(u32 nTimer)
{
	u32 i=0;
	for(i=0;i<nTimer;i++){
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
			__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
	}
}

//void delay_ms(u32 nTimer)
//{
//		u32 i=1000*nTimer;
//		delay_us(i);
//}





