#ifndef __RC_DATA_H_
#define __RC_DATA_H_

#include "DataType.h"


typedef struct{
		uint16  THR;
		uint16  YAW;
		uint16  ROL;
		uint16  PIT;
	
}ReceiveDataType;

typedef struct{
		uint8  Depth_Lock;     //����ȡ�	 0x01 ���������0x02 �˹�����
		uint8  Direction_Lock; //������	 0x01 ����������0x02 ��ˮ��
		uint8  Move;      		 //���˶���	 0x01 ǰ����    0x02 ���ˡ�  0x00 ��������Ĭ�ϣ�
		uint8  Translation;    //��ƽ�ơ�	 0x01 ��ƽ�ơ�  0x02 ��ƽ�ơ�0x00 ��������Ĭ�ϣ�
	  uint8  Vertical;       //����ֱ��	 0x01 ���ϡ�    0x02 ���¡�  0x00 ��������Ĭ�ϣ�
		uint8  Rotate;         //����ת��	 0x01 ������    0x02 ������  0x00��ʾ��������Ĭ�ϣ�
		uint8  Power;          //�����š�	 0x00~0xff (0~255) ��ʾ�����Ŵ�С��4��λ�ɵ���LB�ӵ���LT�������ɷֱ�����4����λ���Ŵ�С
		uint8  Light;          //���ƹ⡿	 0x01 �򿪡�    0x00 �رգ�Ĭ�ϣ�
		uint8  Focus;          //���۽���  0x01 �۽���			0x02 �Ž���  0x00��ʾ��������Ĭ�ϣ�
		uint8  Zoom;           //���佹��  0x01 �Ŵ�			0x02 ��С��  0x00��ʾ��������Ĭ�ϣ�
		uint8  Yuntai;         //����̨��	 0x01 ���ϡ�    0x02 ���¡�  0x00��ʾ��������Ĭ�ϣ�
		uint8  Arm;            //����е�ۡ�0x01 �ſ���     0x02 �رա�  0x00��ʾ��������Ĭ�ϣ�
		uint8  Reserve;            //����е�ۡ�0x01 �ſ���     0x02 �رա�  0x00��ʾ��������Ĭ�ϣ�
}ControlDataType;





extern ReceiveDataType ReceiveData;



#endif 




