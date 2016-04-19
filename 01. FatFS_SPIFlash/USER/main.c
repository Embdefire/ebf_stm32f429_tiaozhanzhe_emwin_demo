/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   ��1.5.1�汾�⽨�Ĺ���ģ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����  STM32 F429 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
/**
  ******************************************************************************
  *                              ͷ�ļ�
  ******************************************************************************
  */
#include "stm32f4xx.h"
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "ff.h"
/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT result; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[1024]={0};       		  /* file copy buffer */
BYTE textFileBuffer[] = "��ӭʹ��Ұ��STM32-V3 ISO������  �����Ǹ������ӣ��½��ļ�ϵͳ�����ļ�\r\n";

/**
  ******************************************************************************
  *                                ������
  ******************************************************************************
  */
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{    
	/* ��ʼ��LED */
	LED_GPIO_Config();
	
	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	Debug_USART_Config();
	
  printf("\r\n this is a fatfs test demo \r\n");

/*************************   spi flash �ļ�ϵͳ   *********************************************/
	//���ⲿFlash�����ļ�ϵͳ
	result = f_mount(&fs,"1:",1);
	printf("\r\n f_mount result=%d \r\n",result);
	
	//���û���ļ�ϵͳ�͸�ʽ�����������ļ�ϵͳ
	if(result == FR_NO_FILESYSTEM)
	{
		result=f_mkfs("1:",0,4096);							//��ʽ��
		printf("\r\nmkfs result=%d",result);
		result = f_mount(&fs,"1:",0);						//��ʽ������ȡ������
		printf("\r\n f_mount result=%d \r\n",result);
		result = f_mount(&fs,"1:",1);						//���¹���
		printf("\r\n f_mount result=%d \r\n",result);
	}
/**************************  flash   *****************************************/		
	//�ļ�ϵͳ���ԣ�д����
	//���ļ�������ļ����μ��򴴽���
	result = f_open(&fnew, "1:flashnewfile.txt", FA_CREATE_ALWAYS | FA_WRITE );
	printf("\r\f_open result=%d",result);
	if ( result == FR_OK )
	{
		result = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
		printf("\r\f_write result=%d",result);
		result = f_lseek(&fnew,sizeof(textFileBuffer)*50);
		result = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
		printf("\r\f_write result=%d",result);
		f_close(&fnew);      
	}
	//������
	result = f_open(&fnew, "1:flashnewfile.txt", FA_OPEN_EXISTING | FA_READ); 	 
	printf("\r\f_open result=%d",result);
	result = f_read(&fnew, buffer, sizeof(buffer), &br); 
	printf("\r\f_read result=%d",result);
	printf("\r\n %s ", buffer);

	/* Close open files */
	result = f_close(&fnew);	
  
	//����ʹ���ļ�ϵͳ��ȡ�������ļ�ϵͳ
	result = f_mount(NULL,"1:",1);
	while(1)
	{
	}

}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
