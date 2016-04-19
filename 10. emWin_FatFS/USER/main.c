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
#include "stm32f4xx.h"
#include "./Bsp/led/bsp_led.h" 
#include "./Bsp/beep/bsp_beep.h" 
#include "./Bsp/usart/bsp_debug_usart.h"
#include "./Bsp/systick/bsp_SysTick.h"
#include "./Bsp/TouchPad/bsp_touchpad.h"
#include "./Bsp/key/bsp_key.h"
#include "./Bsp/sdram/bsp_sdram.h"
#include "./Bsp/lcd/bsp_lcd.h"
#include "./Bsp/touch/gt9xx.h"
#include "./Bsp/usb/usbh_bsp.h"
#include "GUI.h"
#include "DIALOG.h"


extern void SpeedTest(void);
extern WM_HWIN Createfatfs(void);

KEY Key1,Key2;



/*
*********************************************************************************************************
* Description : ��ʼ��BL8782 wifiģ��ʹ�����ţ����ر�ʹ��
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static void BL8782_PDN_INIT(void)
{
  /*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOG, ENABLE); 	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(GPIOG, &GPIO_InitStructure);	
  
  GPIO_ResetBits(GPIOG,GPIO_Pin_9);  //�ر�BL_8782wifiʹ��
}
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
//	static uint8_t count=0;
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
	BL8782_PDN_INIT();
  
	/* ��ʼ��LED */
	LED_GPIO_Config();
	/* ��ʼ�������� */
	Beep_GPIO_Config();
	Key1_GPIO_Config();
	Key2_GPIO_Config();
	KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	Debug_USART_Config();
	
	/* ��ʼ�������� */
	GTP_Init_Panel();	//��������ʼ������������ϵͳ�δ�ʱ����ʼ��
	
	/* ��ʼ��ϵͳ�δ�ʱ�� */
	SysTick_Init();
	
	TPAD_Init();
	
  /*��ʼ��SDRAMģ��*/
  SDRAM_Init();

	/* Init the STemWin GUI Library */
  GUI_Init();
	
	Createfatfs();
	
	/* Init Host Library */
	USBH_Init(&USB_OTG_Core,
			USB_OTG_HS_CORE_ID,
            &USB_Host,
            &USBH_MSC_cb,
            &USR_cb);
	while(1)
	{
		USBH_Process(&USB_OTG_Core, &USB_Host);
		Key_RefreshState(&Key1);//ˢ�°���״̬
		Key_RefreshState(&Key2);//ˢ�°���״̬
		
		if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//���������¹�
		{
			LED2_TOGGLE;			
			Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
		}
		if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//���������¹�
		{
			LED3_TOGGLE;			
			Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
		}
		
	  GUI_Delay(10);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
