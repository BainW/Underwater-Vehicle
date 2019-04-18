/*
 * uart.c
 *
 *  Created on: 2019��2��30��
 *      Author: zengwangfa
 *      Notes:  �����豸��д����
 */
#define LOG_TAG    "uart"

#include "gyroscope.h"
#include <elog.h>
#include <string.h>
#include <rtthread.h>
#include "drv_ano.h"
#include "wifi.h"

/*---------------------- Constant / Macro Definitions -----------------------*/

#define GYRO_UART_NAME        "uart2"
#define WIFI_UART_NAME        "uart3"   // uart3 WIFI 
#define BLUETOOTH_UART_NAME   "uart4"   // uart4 ����

#define DEBUG_UART_NAME       "uart4"   //�ɸ���Ϊuart3 WIFI �� uart4 ����

#define Query_JY901_data 0     /* "1"Ϊ���Բ�ѯ  "0"Ϊ������ȡ */

#if Query_JY901_data
char recv_buffer[128]; 				//����2�������ݻ������,
unsigned char recv_data_p=0x00;  //  ����2��������ָ��
#endif

/*----------------------- Variable Declarations -----------------------------*/
/* ALL_init �¼����ƿ�. */
extern struct rt_event init_event;

rt_device_t debug_uart_device;	
rt_device_t gyro_uart_device;	
rt_device_t wifi_uart_device;	

struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; /* ���ò��� */

struct rt_semaphore gyro_rx_sem;/* ���ڽ�����Ϣ���ź��� */
struct rt_semaphore debug_rx_sem;/* ���ڽ�����Ϣ���ź��� */
struct rt_semaphore wifi_rx_sem;/* ���ڽ�����Ϣ���ź��� */

uint8 debug_startup_flag = 0; //debug���� ��ʼ����ɱ�־λ
/*----------------------- Function Implement --------------------------------*/

/* �������ݻص����� */
static rt_err_t gyroscope_uart_input(rt_device_t dev, rt_size_t size)
{
    /* ���ڽ��յ����ݺ�����жϣ����ô˻ص�������Ȼ���ͽ����ź��� */
    rt_sem_release(&gyro_rx_sem);

    return RT_EOK;
}

static void gyroscope_thread_entry(void *parameter)
{
    unsigned char ch;

		while (1)
		{
				/* �Ӵ��ڶ�ȡһ���ֽڵ����ݣ�û�ж�ȡ����ȴ������ź��� */
				while (rt_device_read(gyro_uart_device, 0, &ch, 1) != 1)
				{
				  /* �����ȴ������ź������ȵ��ź������ٴζ�ȡ���� */
						rt_sem_take(&gyro_rx_sem, RT_WAITING_FOREVER);
				}
#if Query_JY901_data //���ߵ��� ��ѯģʽ
				
				recv_buffer[recv_data_p] = ch;
				recv_data_p++;
				if(recv_data_p>127)recv_data_p = 0;
		
#else 
				CopeSerial2Data(ch); //��������ģʽ ɸѡ���ݰ�
#endif
		}
}


/* �������ݻص����� */
static rt_err_t debug_uart_input(rt_device_t dev, rt_size_t size)
{
    /* ���ڽ��յ����ݺ�����жϣ����ô˻ص�������Ȼ���ͽ����ź��� */
    rt_sem_release(&debug_rx_sem);

    return RT_EOK;
}


static void debug_thread_entry(void *parameter)
{
    unsigned char ch;

		while (1)
		{
				/* �Ӵ��ڶ�ȡһ���ֽڵ����ݣ�û�ж�ȡ����ȴ������ź��� */
				while (rt_device_read(debug_uart_device, 0, &ch, 1) != 1)
				{
						/* �����ȴ������ź������ȵ��ź������ٴζ�ȡ���� */
						rt_sem_take(&debug_rx_sem, RT_WAITING_FOREVER);
				}
				ANO_DT_Data_Receive_Prepare(ch);

		}
}



int device_uart_init(void)
{

		/* ���� �߳̾�� */
		rt_thread_t gyroscope_tid;
		rt_thread_t debug_tid;
	  /* ����ϵͳ�еĴ����豸 */
		gyro_uart_device = rt_device_find(GYRO_UART_NAME);       
		debug_uart_device = rt_device_find(DEBUG_UART_NAME);
	  wifi_uart_device = rt_device_find(WIFI_UART_NAME);

		log_v("console serial: %s", RT_CONSOLE_DEVICE_NAME);	
		log_v("gyroscope serial: %s", gyro_uart_device);
		log_v("debug serial: %s", debug_uart_device);

	
    if (gyro_uart_device != RT_NULL){		
			
					/* �Զ�д�Լ��жϽӴ򿪴����豸 */
				rt_device_open(gyro_uart_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);

				rt_sem_init(&gyro_rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
				/* ���ý��ջص����� */
				rt_device_set_rx_indicate(gyro_uart_device, gyroscope_uart_input);
		}
		
		
		if(debug_uart_device != RT_NULL){
				rt_device_open(debug_uart_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
				config.baud_rate = BAUD_RATE_115200;
				config.data_bits = DATA_BITS_8;
				config.stop_bits = STOP_BITS_1;
				config.parity = PARITY_NONE;
				
				log_v("Debug_Uart_Baud Rate:%d",config.baud_rate);
				/* ���豸��ſ��޸Ĵ������ò��� */
				rt_device_control(debug_uart_device, RT_DEVICE_CTRL_CONFIG, &config);
			
				rt_sem_init(&debug_rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
				/* ���ý��ջص����� */
				rt_device_set_rx_indicate(debug_uart_device, debug_uart_input);
		}
//		if (wifi_uart_device != RT_NULL){		
//			
//					/* �Զ�д�Լ��жϽӴ򿪴����豸 */
//				rt_device_open(wifi_uart_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
//			
//				rt_sem_init(&wifi_rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
//				/* ���ý��ջص����� */
//				rt_device_set_rx_indicate(wifi_uart_device, wifi_uart_input);
//		}
    /* ���� serial �߳� */
		gyroscope_tid = rt_thread_create("gyro_uart",
																			gyroscope_thread_entry,
																			RT_NULL, 
																			512, 
																			12,
																			10);
		
		    /* ���� serial �߳� */
		debug_tid = 	rt_thread_create("debug_uart",
																			debug_thread_entry,
																			RT_NULL, 
																			512, 
																			13,
																			10);
    /* �����ɹ��������߳� */
    if (gyroscope_tid != RT_NULL){
				log_i("Uart_Init()");
				rt_thread_startup(gyroscope_tid);
				rt_event_send(&init_event, GYRO_EVENT); //�����¼�  ��ʾ��ʼ�����
    }
		
		/* �����ɹ��������߳� */
    if (debug_tid != RT_NULL){
				rt_thread_startup(debug_tid);
				debug_startup_flag = 1;
    }
		return 0;
}
INIT_APP_EXPORT(device_uart_init);





