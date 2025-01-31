/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                    MICRIUM BOARD SUPPORT PACKAGE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                         STM3240G-EVAL
*                                        Evaluation Board
*
* Filename      : bsp.c
* Version       : V1.00
* Programmer(s) : FF
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "includes.h"

FATFS   fs[2];													  /* Work area (file system object) for logical drives */

GUI_XBF_DATA XBF_Data;
GUI_FONT     XBF_Font;


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
#define  BSP_INT_SRC_NBR                                 91

/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          LOCAL DATA TYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            LOCAL TABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             REGISTERS
*********************************************************************************************************
*/

#define  BSP_REG_DEM_CR                       (*(CPU_REG32 *)0xE000EDFC)
#define  BSP_REG_DWT_CR                       (*(CPU_REG32 *)0xE0001000)
#define  BSP_REG_DWT_CYCCNT                   (*(CPU_REG32 *)0xE0001004)
#define  BSP_REG_DBGMCU_CR                    (*(CPU_REG32 *)0xE0042004)

/*
*********************************************************************************************************
*                                            REGISTER BITS
*********************************************************************************************************
*/

#define  BSP_DBGMCU_CR_TRACE_IOEN_MASK                   0x10
#define  BSP_DBGMCU_CR_TRACE_MODE_ASYNC                  0x00
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_01                0x40
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_02                0x80
#define  BSP_DBGMCU_CR_TRACE_MODE_SYNC_04                0xC0
#define  BSP_DBGMCU_CR_TRACE_MODE_MASK                   0xC0

#define  BSP_BIT_DEM_CR_TRCENA                    DEF_BIT_24

#define  BSP_BIT_DWT_CR_CYCCNTENA                 DEF_BIT_00

// USER START (Optionally insert additional static code)
#if 0
FIL     xbffile;
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
	result = f_open(&xbffile, "0:新宋体25.xbf", FA_OPEN_EXISTING | FA_READ | FA_OPEN_ALWAYS);
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

#include "./drivers/fatfs_flash_spi.h"
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
	SPI_FLASH_BufferRead(pBuffer,Offset+60*4096,NumBytes);
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
/*
*********************************************************************************************************
* Description : 初始化BL8782 wifi模块使能引脚，并关闭使能
* Argument(s) : none.
* Return(s)   : none.
*********************************************************************************************************
*/
static void BL8782_PDN_INIT(void)
{
  /*定义一个GPIO_InitTypeDef类型的结构体*/
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd ( WIFI_PDN_GPIO_CLK, ENABLE); 							   
  GPIO_InitStructure.GPIO_Pin = WIFI_PDN_PIN;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
  GPIO_Init(WIFI_PDN_GPIO_PORT, &GPIO_InitStructure);	
  
  WIFI_PDN_DISABLE();  //关闭BL_8782wifi使能
}
// USER END

/*
*********************************************************************************************************
*                                               BSP_Init()
*
* Description : Initialize the Board Support Package (BSP).
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : (1) This function SHOULD be called before any other BSP function is called.
*
*               (2) CPU instruction / data tracing requires the use of the following pins :
*                   (a) (1) Aysynchronous     :  PB[3]
*                       (2) Synchronous 1-bit :  PE[3:2]
*                       (3) Synchronous 2-bit :  PE[4:2]
*                       (4) Synchronous 4-bit :  PE[6:2]
*
*                   (c) The application may wish to adjust the trace bus width depending on I/O
*                       requirements.
*               (3) The voltage scaling allows optimizing the power consumption when the device is
*                   clocked below the maximum system frequency, to update the voltage scaling value
*                   regarding system frequency refer to product datasheet.
*********************************************************************************************************
*/

void  BSP_Init (void)
{	
	FRESULT result; 
	/*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
	
	/* 设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/*初始化SDRAM模块*/
  SDRAM_Init();
  
	BL8782_PDN_INIT();
  
	/* 初始化LED */
	LED_GPIO_Config();
	/* 初始化蜂鸣器 */
	Beep_GPIO_Config();
	Key1_GPIO_Config();
	Key2_GPIO_Config();
	/* 初始化调试串口，一般为串口1 */
	Debug_USART_Config();

	printf("TouchPAD Demo\n");

		/* 初始化触摸屏 */
	GTP_Init_Panel();
	
	BSP_Tick_Init();
		
	TPAD_Init();
  

	/* 挂载文件系统 */
  result = f_mount(&fs[0],"0:",1);	/* Mount a logical drive */
  result = f_mount(&fs[1],"1:",1);	/* Mount a logical drive */
	
	/* Init the STemWin GUI Library */
  GUI_Init();
	 /* Setup layer configuration during startup */
//  GUI_SetBkColor(GUI_TRANSPARENT);
//  GUI_SelectLayer(1);
//  GUI_Clear();
//  GUI_SetBkColor(GUI_TRANSPARENT); 
//  GUI_SelectLayer(0);

//	SpeedTest();
//	GUI_Delay(2000);
  if(result==FR_OK)
	{
		_ShowXBF();
		GUI_UC_SetEncodeUTF8();
		GUI_SetDefaultFont(&XBF_Font);
	}
}


/*
*********************************************************************************************************
*                                            BSP_CPU_ClkFreq()
*
* Description : Read CPU registers to determine the CPU clock frequency of the chip.
*
* Argument(s) : none.
*
* Return(s)   : The CPU clock frequency, in Hz.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    RCC_ClocksTypeDef  rcc_clocks;

    RCC_GetClocksFreq(&rcc_clocks);
    return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}




/*
*********************************************************************************************************
*                                            BSP_Tick_Init()
*
* Description : Initialize all the peripherals that required OS Tick services (OS initialized)
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/
void BSP_Tick_Init (void)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;

    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */

#if (OS_VERSION >= 30000u)
    cnts  = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;       /* Determine nbr SysTick increments.                    */
#else
    cnts  = cpu_clk_freq / (CPU_INT32U)OS_TICKS_PER_SEC;        /* Determine nbr SysTick increments.                    */
#endif

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */
//	SysTick_Config(cnts);   //这里默认的是最低优先级
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DelayUS
*	功能说明: 这里的延时采用CPU的内部计数实现，32位计数器
*             	OSSchedLock(&err);
*				bsp_DelayUS(5);
*				OSSchedUnlock(&err); 根据实际情况看看是否需要加调度锁或选择关中断
*	形    参：n : 延迟长度，单位1 us
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t _ulDelayTime)
{
  uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
		    	 
	ticks = _ulDelayTime * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */ 		 
	tcnt = 0;
	told = (uint32_t)CPU_TS_TmrRd();         /* 刚进入时的计数器值 */

	while(1)
	{
		tnow = (uint32_t)CPU_TS_TmrRd();	
		if(tnow != told)
		{	
		    /* 32位计数器是递增计数器 */    
			if(tnow > told)
			{
				tcnt += tnow - told;	
			}
			/* 重新装载 */
			else 
			{
				tcnt += UINT32_MAX - told + tnow;	
			}	
			
			told = tnow;

			/*时间超过/等于要延迟的时间,则退出 */
			if(tcnt >= ticks)break;
		}  
	}
}

/*
*********************************************************************************************************
*                                              BSP_IntDis()
*
* Description : Disable interrupt.
*
* Argument(s) : int_id      Interrupt to disable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDis (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcDis(int_id + 16);
    }
}

/*
*********************************************************************************************************
*                                           BSP_IntDisAll()
*
* Description : Disable ALL interrupts.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}
/*
*********************************************************************************************************
*                                               BSP_IntEn()
*
* Description : Enable interrupt.
*
* Argument(s) : int_id      Interrupt to enable.
*
* Return(s)   : none.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  BSP_IntEn (CPU_DATA  int_id)
{
    if (int_id < BSP_INT_SRC_NBR) {
        CPU_IntSrcEn(int_id + 16);
    }
}
