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
#include "./drivers/fatfs_flash_spi.h"

extern void drawpng(void);
extern void drawbmp(void);
extern void drawJPG(void);

FATFS   fs[3];													  /* Work area (file system object) for logical drives */
FIL     xbffile;

KEY  Key1,Key2;
GUI_XBF_DATA XBF_Data;
GUI_FONT     XBF_Font;

// USER START (Optionally insert additional static code)
#if 0
/*
*********************************************************************************************************
*
*       _cbGetData
*
* Function description
*   Callback function for getting font data
*
* Parameters:
*   Off      - Position of XBF file to be read
*   NumBytes - Number of requested bytes
*   pVoid    - Application defined pointer
*   pBuffer  - Pointer to buffer to be filled by the function
*
* Return value:
*   0 on success, 1 on error
*********************************************************************************************************
*/
static int _cbGetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
{
	FRESULT result;
	FIL *FontFile;

	/* The pVoid pointer may be used to get a file handle */
	FontFile = (FIL *)pVoid;

	/*
	* Set file pointer to the required position
	*/
	result =f_lseek(FontFile, Offset);
	if (result != FR_OK) 
	{
		return 1; /* Error */
	}

	/*
	* Read data into buffer
	*/
	result = f_read(FontFile, pBuffer, NumBytes,0);
	if(result!=FR_OK)
	{
		return 1; /* Error */
	}
	return 0;

}

/*
*********************************************************************************************************
*	? ? ?: _ShowXBF
*	????: ??XBF??
*	?    ?:?
*	? ? ?: ?
*********************************************************************************************************
*/
static void _ShowXBF(void) 
{
	FRESULT result; 
	result = f_open(&xbffile, "0:xinsongti25.xbf", FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
	if (result != FR_OK)
	{
		return;

	}
	//
	// Create XBF font
	//
	GUI_XBF_CreateFont(&XBF_Font,       // Pointer to GUI_FONT structure in RAM
					 &XBF_Data,         // Pointer to GUI_XBF_DATA structure in RAM
					 GUI_XBF_TYPE_PROP_EXT, 		// Font type to be created
					 _cbGetData,        // Pointer to callback function
					 &xbffile);        // Pointer to be passed to GetData function
}

#else
//
/*
*********************************************************************************************************
*
*       _cbGetData
*
* Function description
*   Callback function for getting font data
*
* Parameters:
*   Off      - Position of XBF file to be read
*   NumBytes - Number of requested bytes
*   pVoid    - Application defined pointer
*   pBuffer  - Pointer to buffer to be filled by the function
*
* Return value:
*   0 on success, 1 on error
*********************************************************************************************************
*/
static int _cbGetData(U32 Offset, U16 NumBytes, void * pVoid, void * pBuffer)
{
	SPI_FLASH_BufferRead(pBuffer,60*4096+Offset,NumBytes);
	return 0;
}

static void _ShowXBF(void) 
{
	//
	// Create XBF font
	//
	GUI_XBF_CreateFont(&XBF_Font,       // Pointer to GUI_FONT structure in RAM
					   &XBF_Data,         // Pointer to GUI_XBF_DATA structure in RAM
					   GUI_XBF_TYPE_PROP_EXT, 		// Font type to be created
					   _cbGetData,        // Pointer to callback function
					   0);        // Pointer to be passed to GetData function
}
#endif
// USER END

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	FRESULT result; 

	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
  /*��ʼ��SDRAMģ��*/
  SDRAM_Init();
	
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
	
	/* �����ļ�ϵͳ */
  result = f_mount(&fs[0],"0:",1);
	result = f_mount(&fs[1],"1:",1);	/* Mount a logical drive */
	/* Init the STemWin GUI Library */
  GUI_Init();
	
	
	if(result==FR_OK)
	{
		_ShowXBF();
		GUI_UC_SetEncodeUTF8();
		GUI_SetDefaultFont(&XBF_Font);
	}
	GUI_SetBkColor(GUI_WHITE);
  GUI_Clear(); 
  GUI_SetFont(GUI_FONT_32B_ASCII);
  GUI_SetColor(GUI_BLUE);
  GUI_DispStringAt("KEY1:show png file",100,100);
  GUI_DispStringAt("KEY2:show bmp file",100,140);
	while(1)
	{
		Key_RefreshState(&Key1);//ˢ�°���״̬
		Key_RefreshState(&Key2);//ˢ�°���״̬
		
		if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//���������¹�
		{
			LED2_TOGGLE;
      drawpng();
			Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
		}
		if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//���������¹�
		{
			LED3_TOGGLE;	   
      drawbmp();   //  drawJPG();//  
			Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
		}		
	  GUI_Delay(10);
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
