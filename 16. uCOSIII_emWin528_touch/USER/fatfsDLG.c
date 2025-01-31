/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.30                          *
*        Compiled Jul  1 2015, 10:50:32                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/
// USER START (Optionally insert additional includes)
#include "includes.h"
// USER END

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
FIL     file;													/* file objects */
FRESULT result; 
UINT    bw;            					      /* File R/W count */
static TCHAR object[2]={"0:"};
extern GUI_FONT     XBF_Font;
char displaystr[768]={0};
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "fatfs", 0, 0, 0, 800, 480, 0, 0x0, 0 },
  { RADIO_CreateIndirect, "object", GUI_ID_RADIO0, 20, 40, 110, 60, 0, 0x1e02, 0 },
  { BUTTON_CreateIndirect, "ViewDir", GUI_ID_BUTTON0, 150, 20, 110, 30, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "RootDir", GUI_ID_MULTIEDIT0, 300, 20, 480, 110, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "ReadTest", GUI_ID_BUTTON1, 150, 100, 110, 30, 0, 0x0, 0 },
  { MULTIEDIT_CreateIndirect, "ReadText", GUI_ID_MULTIEDIT1, 20, 150, 760, 320, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)
/*
*********************************************************************************************************
*	函 数 名: ViewRootDir
*	功能说明: 显示根目录下的文件名
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ViewRootDir(const TCHAR  *path,WM_HWIN hWin)
{
	DIR DirInf;
	FILINFO FileInf;
	uint32_t cnt = 0;
	char lfname[256];
	
	MULTIEDIT_SetText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0),"");


	/* 打开根文件夹 */
	result = f_opendir(&DirInf, path); /* 如果不带参数，则从当前目录开始 */
	if (result != FR_OK)
	{
		sprintf(displaystr,"open root directory fail (%d)\n", result);
		MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0),displaystr);
		return;
	}

	/* 读取当前文件夹下的文件和目录 */
	FileInf.lfname = lfname;
	FileInf.lfsize = 256;

	sprintf(displaystr,"    Type    |     Size     |   Simpligy Name   |  Complete Name\n");
	MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0),displaystr);
	
	for (cnt = 0; ;cnt++)
	{
		result = f_readdir(&DirInf,&FileInf); 		/* 读取目录项，索引会自动下移 */
		if (result != FR_OK || FileInf.fname[0] == 0)
		{
			break;
		}

		if (FileInf.fname[0] == '.')
		{
			continue;
		}

		/* 判断是文件还是子目录 */
		if (FileInf.fattrib & AM_DIR)
		{
			sprintf(displaystr,"(0x%02d)Path  ", FileInf.fattrib);
			MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0),displaystr);
		}
		else
		{
			sprintf(displaystr,"(0x%02d)file  ", FileInf.fattrib);
			MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0),displaystr);
		}

		/* 打印文件大小, 最大4G */
		sprintf(displaystr," %10d ", FileInf.fsize);
		MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0),displaystr);
		sprintf(displaystr,"     %s |", FileInf.fname);	/* 短文件名 */
		MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0),displaystr);
		sprintf(displaystr,"    %s\n", (char *)FileInf.lfname);	/* 长文件名 */
		MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT0),displaystr);
	}
}

void com_gbk2utf8(const char *src, char *str)
{
	uint32_t j=0,k=0;
	uint16_t gbkdata=0;
	uint16_t UCbuffer[512]={0};
	for(j=0,k=0;src[j]!='\0';k++)
	{
		if((uint8_t)src[j]>0x80)
		{
			gbkdata=src[j+1]+src[j]*256;
			UCbuffer[k]=ff_convert(gbkdata,1);
			j+=2;
		}
		else
		{
			UCbuffer[k]=0x00ff&src[j];
			j+=1;
		}
	}
	UCbuffer[k]='\0';
	GUI_UC_ConvertUC2UTF8(UCbuffer,2*k+2,str,k*3);
	str[k*3]=0;
}

/*
*********************************************************************************************************
*	函 数 名: ReadFileData
*	功能说明: 读取文件test.txt前128个字符，并打印到串口
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void ReadFileData(const TCHAR  *path,WM_HWIN hWin)
{	
  TCHAR filepath[20]={0};
	uint32_t bw;
	char buf[512];  
	
	sprintf(filepath,"%sReadme.txt",path);
	/* 打开文件 */
	result = f_open(&file, filepath, FA_OPEN_EXISTING | FA_READ);
	if (result !=  FR_OK)
	{
		sprintf(displaystr,"Don't Find File : %s\n",filepath);
		MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT1),displaystr);
		return;
	}

	if(file.fsize<(sizeof(buf) - 1))
		/* 读取文件 */
		result = f_read(&file, &buf, file.fsize, &bw);
	else
		/* 读取文件 */
		result = f_read(&file, &buf, sizeof(buf) - 1, &bw);
	if (bw > 0)
	{
		buf[bw] = 0;
		com_gbk2utf8(buf,displaystr);
		MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT1),"testifle ->:\n");
		MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT1),displaystr);
	}
	else
	{
		sprintf(displaystr,"%s ->:\n",filepath);
		MULTIEDIT_AddText(WM_GetDialogItem(hWin, GUI_ID_MULTIEDIT1),displaystr);
	}

	/* 关闭文件*/
	f_close(&file);
}
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
	uint8_t num;
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'object'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0);
    RADIO_SetFont(hItem, GUI_FONT_20B_ASCII);
    RADIO_SetText(hItem, "SD Card", 0);
    RADIO_SetText(hItem, "Spi Flash", 1);
    //
    // Initialization of 'ViewDir'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON0);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
    //
    // Initialization of 'RootDir'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0);
    MULTIEDIT_SetText(hItem, "");
    MULTIEDIT_SetFont(hItem, GUI_FONT_16B_ASCII);
    MULTIEDIT_SetTextColor(hItem, MULTIEDIT_CI_EDIT, GUI_BLUE);
	  MULTIEDIT_SetAutoScrollH(hItem,1);
		MULTIEDIT_SetAutoScrollV(hItem,1);
    MULTIEDIT_SetReadOnly(hItem,1);
    //
    // Initialization of 'SpeedTest'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_BUTTON1);
    BUTTON_SetFont(hItem, GUI_FONT_20B_ASCII);
    //
    // Initialization of 'SpeedText'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT1);
    MULTIEDIT_SetText(hItem, "");
    MULTIEDIT_SetFont(hItem, &XBF_Font);
    MULTIEDIT_SetTextColor(hItem, MULTIEDIT_CI_EDIT, GUI_BLUE);
		MULTIEDIT_SetAutoScrollH(hItem,1);
		MULTIEDIT_SetAutoScrollV(hItem,1);
		MULTIEDIT_SetWrapWord(hItem);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case GUI_ID_RADIO0: // Notifications sent by 'object'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT0), "");
			  MULTIEDIT_SetText(WM_GetDialogItem(pMsg->hWin, GUI_ID_MULTIEDIT1), "");
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON0: // Notifications sent by 'WriteTest'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
			  num=RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0));
			  sprintf(object,"%d:",num);
        ViewRootDir(object,pMsg->hWin);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case GUI_ID_BUTTON1: // Notifications sent by 'ReadTest'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
				num=RADIO_GetValue(WM_GetDialogItem(pMsg->hWin, GUI_ID_RADIO0));
			  sprintf(object,"%d:",num);
        ReadFileData(object,pMsg->hWin);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
//	case WM_PAINT:
//		GUI_SetFont(&XBF_Font);
//		GUI_SetTextMode(GUI_TM_TRANS);
//	  GUI_SetColor(GUI_BLUE);
//	break;
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       Createfatfs
*/
void Createfatfs(void) {


  GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  while(1)
  {
    GUI_Delay(20);
  }
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/
