/*********************************************************************************
*      notes.c    ���Աʼ�
*                                     	  0.log_a[A]������(Assert)        
* File          : notes.c             	  1.log_e[E]������(Error)
* Version       : V1.0          		  	  2.log_w[W]������(Warn)
* Author        : zengwangfa				   		3.log_i[I]����Ϣ(Info)
																					4.log_d[D]������(Debug)
* History       :													5.log_v[V]����ϸ(Verbose)
* Date          : 2019.01.25							����log�����ȼ��������
*******************************************************************************/


/*

Notes:

�ص㣺���뻷����Ϊc99��ϵͳĬ��Ϊc89��c89����ֻ�ܽ�������ں�������ǰ��

������c99������
				Options -> C/C++ -> Misc Controls ->�������롾--c99�� ->���漴��




1.JY901 ��λ������Ư��

��:ʹ��ǰ��ҪУ׼
��У׼ʱ��������Χ�ų�����
ʹ�ó�������ָ��:
		http://wiki.wit-motion.com/doku.php?id=wt901��������



2.RT-Thread�����������ͣ�

sample:
		//���ݴ����string��       ��ΪRT-Thread rt_kprintf()�����޷���������ͣ�����ֽ����ݴ����String�ͷ���.
		char str[100];
		sprintf(str,"Time:20%d-%d-%d %d:%d:%.3f\r\n",stcTime.ucYear,stcTime.ucMonth,stcTime.ucDay,stcTime.ucHour,stcTime.ucMinute,(float)stcTime.ucSecond+(float)stcTime.usMiliSecond/1000);
		rt_kprintf(str);




3.W25Q128 ID��ȡ����ȷ��

SPI��ʼ������ʱ:
		< SPI_BaudRatePrescaler_64 >  -> ���岨����Ԥ��Ƶ��ֵ  ����̫��Ҳ����̫С.






*/

