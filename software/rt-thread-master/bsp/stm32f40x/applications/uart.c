#include "init.h"
#include <rthw.h>

/*---------------------- Constant / Macro Definitions -----------------------*/

#define JY901_UART_NAME       "uart2"
#define DEBUG_UART_NAME       "uart3"

#define Query_JY901_data 0     /* "1"Ϊ���Բ�ѯ  "0"Ϊ������ȡ */

#if Query_JY901_data
char recv_buffer[128]; 				//����2�������ݻ������,
unsigned char recv_data_p=0x00;  //  ����2��������ָ��
#endif

/*----------------------- Variable Declarations -----------------------------*/
/* ALL_init �¼����ƿ�. */
extern struct rt_event init_event;

rt_device_t debug_uart_device;	

rt_device_t uart2_device;	
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT; /* ���ò��� */
static struct rt_semaphore rx_sem;/* ���ڽ�����Ϣ���ź��� */

u8 gyroscope_save[5] 		={0xFF,0xAA,0x00,0x00,0x00};	 //0x00-���ñ���  0x01-�ָ��������ò�����
u8 gyroscope_package[5] ={0xFF,0xAA,0x02,0xFF,0x07};	 //���ûش������ݰ���0xFF 0x07 Ϊ�������ݰ���
u8 gyroscope_rate[5] 		={0xFF,0xAA,0x03,0x06,0x00};	 //�������� 0x05-5Hz  0x06-10Hz(Ĭ��)  0x07-20Hz
u8 gyroscope_led[5] 		={0xFF,0xAA,0x1B,0x00,0x00}; 	 //0x00-����LED  0x01-�ر�LED   

/*----------------------- Function Implement --------------------------------*/

/* �������ݻص����� */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* ���ڽ��յ����ݺ�����жϣ����ô˻ص�������Ȼ���ͽ����ź��� */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void gyroscope_thread_entry(void *parameter)
{
    unsigned char ch;

		while (1)
		{
				/* �Ӵ��ڶ�ȡһ���ֽڵ����ݣ�û�ж�ȡ����ȴ������ź��� */
				while (rt_device_read(uart2_device, 0, &ch, 1) != 1)
				{
						/* �����ȴ������ź������ȵ��ź������ٴζ�ȡ���� */
						rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
				}
#if Query_JY901_data //���ߵ��� ��ѯģʽ
				
				recv_buffer[recv_data_p] = ch;
				recv_data_p++;
				if(recv_data_p>127)recv_data_p = 0;
		
#else 
				CopeSerial2Data(ch); //��������ģʽ

#endif

		}
	
}

/* ���� ����ģ�� ���ݰ� */
void on_gyroscope_package(void)
{
		rt_device_write(uart2_device, 0, gyroscope_package, 5);   //ON package �����ش����ݰ�
		rt_device_write(uart2_device, 0, gyroscope_save, 5);  //SAVE
}
MSH_CMD_EXPORT(on_gyroscope_package,turn on gyroscope_led);

/* ���� ����ģ�� LED */
void on_gyroscope_led(void)
{
		gyroscope_led[3] = 0x00;
		rt_device_write(uart2_device, 0, gyroscope_led, 5);   //ON LED
		rt_device_write(uart2_device, 0, gyroscope_save, 5);  //SAVE
}
MSH_CMD_EXPORT(on_gyroscope_led,turn on gyroscope_led);

/* �ر� ����ģ�� LED */
void off_gyroscope_led(void)
{
		gyroscope_led[3] = 0x01;
		rt_device_write(uart2_device, 0, gyroscope_led, 5);   //OFF LED
		rt_device_write(uart2_device, 0, gyroscope_save, 5);  //SAVE
}
MSH_CMD_EXPORT(off_gyroscope_led,turn off gyroscope_led);


int uart_gyroscope(void)
{
	  rt_thread_t gyroscope_tid;
	  /* ����ϵͳ�еĴ����豸 */
		uart2_device = rt_device_find(JY901_UART_NAME);
		debug_uart_device = rt_device_find(DEBUG_UART_NAME);
	
		LOG_I("gyroscope serial:  %s", uart2_device);
		LOG_I("debug serial:  %s", debug_uart_device);
		rt_device_open(debug_uart_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
	
    if (uart2_device != RT_NULL)
    {
					/* �Զ�д�Լ��жϽӴ򿪴����豸 */
					rt_device_open(uart2_device, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
					config.baud_rate = BAUD_RATE_9600;
					config.data_bits = DATA_BITS_8;
					config.stop_bits = STOP_BITS_1;
					config.parity = PARITY_NONE;
			
					/* ���豸��ſ��޸Ĵ������ò��� */
					rt_device_control(uart2_device, RT_DEVICE_CTRL_CONFIG, &config);
					rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
					/* ���ý��ջص����� */
					rt_device_set_rx_indicate(uart2_device, uart_input);
		}
    /* ���� serial �߳� */
		gyroscope_tid = rt_thread_create("gyroscope",
																			gyroscope_thread_entry,
																			RT_NULL, 
																			1024, 
																			25,
																			10);
    /* �����ɹ��������߳� */
    if (gyroscope_tid != RT_NULL)
    {
        rt_thread_startup(gyroscope_tid);
				rt_event_send(&init_event, GYRO_EVENT); //�����¼�  ��ʾ��ʼ�����
    }
		return 0;
}

INIT_APP_EXPORT(uart_gyroscope);





