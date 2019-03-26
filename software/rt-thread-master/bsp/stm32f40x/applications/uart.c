#define LOG_TAG    "uart"

#include "init.h"
#include <rthw.h>
#include <string.h>
#include "drv_ano.h"
#include "wifi.h"
#include <rtdevice.h>
#include <elog.h>
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

u8 gyroscope_save_array[5] 		={0xFF,0xAA,0x00,0x00,0x00};	 //0x00-���ñ���  0x01-�ָ��������ò�����
u8 gyroscope_package_array[5] ={0xFF,0xAA,0x02,0x1F,0x00};	 //���ûش������ݰ���0x1F 0x00 Ϊ <ʱ��> <���ٶ�> <���ٶ�> <�Ƕ�> <�ų�>��
u8 gyroscope_rate_array[5] 		={0xFF,0xAA,0x03,0x06,0x00};	 //�������� 0x05-5Hz  0x06-10Hz(Ĭ��)  0x07-20Hz
u8 gyroscope_led_array[5] 		={0xFF,0xAA,0x1B,0x00,0x00}; 	 //�����ڶ�λ 0x00-����LED  0x01-�ر�LED   
u8 gyroscope_baud_array[5] 		={0xFF,0xAA,0x04,0x02,0x00}; 	 //0x06 - 115200

u8 debug_startup_flag = 1; //debug���� ��ʼ����ɱ�־λ
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
/* ���� ����ģ�� �������� */
void gyroscope_save(void)
{
			rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //������ٶ�У׼
			log_i("JY901 Save successed!");
}
MSH_CMD_EXPORT(gyroscope_save,gyroscope_save);


/* ���� ����ģ�� ���ٶ�У׼ */
void gyroscope_Acc_calibration_enter(void)
{
			u8 Acc_calibration_enter[5]={0xFF,0xAA,0x01,0x01,0x00};
			rt_device_write(gyro_uart_device, 0, Acc_calibration_enter, 5);   //ON LED
			log_i("Acc_calibrationing... ");
			rt_thread_mdelay(500);
			log_i("calibration OK, Next -> [gyroscope_save]");
}
MSH_CMD_EXPORT(gyroscope_Acc_calibration_enter,gyroscope_Acc_calibration_enter);

/* ���� ����ģ�� �ų� У׼ */
void gyroscope_Mag_calibration_enter(void)
{
			u8 Mag_calibration_enter[5]={0xFF,0xAA,0x01,0x02,0x00};
			rt_device_write(gyro_uart_device, 0, Mag_calibration_enter, 5);   //����ų�У׼
			log_i("Mag_calibrationing... ");
			rt_thread_mdelay(2000);
			log_i("After completing the rotation of the three axes... ");
			log_i("Nest -> [gyroscope_Mag_calibration_exit] ");

}
MSH_CMD_EXPORT(gyroscope_Mag_calibration_enter,gyroscope_Mag_calibration_enter);


/* �˳� ����ģ�� �ų�У׼ */
void gyroscope_Mag_calibration_exit(void)
{
			u8 Mag_calibration_exit[5]={0xFF,0xAA,0x01,0x00,0x00};       
			rt_device_write(gyro_uart_device, 0, Mag_calibration_exit, 5);   //�˳��ų�У׼
			rt_thread_mdelay(100);
			gyroscope_save();                                           //������
			log_i("Mag_calibration OK & Saved! ");
}
MSH_CMD_EXPORT(gyroscope_Mag_calibration_exit,gyroscope_Mag_calibration_exit);



/*  ����ģ��  ��λ */
void gyroscope_reset(void)
{
		gyroscope_save_array[3] = 0x01;
		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //����
		log_i("JY901 Reset!");
}
MSH_CMD_EXPORT(gyroscope_reset,gyroscope reset);


/* ���� ����ģ�� ���ݰ� */
void gyroscope_package_open(void)
{
		gyroscope_save_array[3] = 0x00;
		rt_device_write(gyro_uart_device, 0, gyroscope_package_array, 5);   //ON package �����ش����ݰ�
		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //SAVE
		log_i("Open successed! JY901: 1.Time  2.Acc  3.Gyro  4.Angle  5.Mag OPEN!");
}
MSH_CMD_EXPORT(gyroscope_package_open,gyroscope package open);

/* ���� ����ģ�� LED */
static int gyroscope_led(int argc, char **argv)
{
	  int result = 0;
    if (argc != 2){
        log_i("Proper Usage: gyroscope_led on/off\n");
				result = -RT_ERROR;
        goto _exit;
    }

		if( !strcmp(argv[1],"on") ){
				gyroscope_led_array[3] = 0x00;
				log_i("Operation is successful! gyroscope_led on\n");
		}
		else if( !strncmp(argv[1],"off",3) ){
				gyroscope_led_array[3] = 0x01;
				log_i("Operation is successful! gyroscope_led off\n");
		}
		else {
				log_e("Error! Proper Usage: gyroscope_led on/off\n");goto _exit;
		}
		rt_device_write(gyro_uart_device, 0, gyroscope_led_array, 5);   //ON LED
		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //����
		
_exit:
    return result;
}
MSH_CMD_EXPORT(gyroscope_led, gyroscope_led on/off);

/* ���� ����ģ�� ������Ϊ9600 */
void gyroscope_baud_9600(void)
{
		gyroscope_baud_array[3] = 0x02;
		rt_device_write(gyro_uart_device, 0, gyroscope_baud_array, 5);   //ON LED
		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //����
		log_i("JY901 baud:9600 ");
}
MSH_CMD_EXPORT(gyroscope_baud_9600,Modify JY901 baud rate);

/* ���� ����ģ�� ������Ϊ9600 */
void gyroscope_baud_115200(void)
{
		gyroscope_baud_array[3] = 0x06;
		rt_device_write(gyro_uart_device, 0, gyroscope_baud_array, 5);   //ON LED
		rt_device_write(gyro_uart_device, 0, gyroscope_save_array, 5);  //����
		log_i("JY901 baud:115200 ");
}
MSH_CMD_EXPORT(gyroscope_baud_115200,Modify JY901 baud rate);


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





