/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   用1.5.1版本库建的工程模板
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 F429 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
#include "includes.h"

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/* --------------- APPLICATION GLOBALS ---------------- */
static  OS_TCB       AppTaskStartTCB;
static  CPU_STK      AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];

static  OS_TCB       AppTaskObj0TCB;
static  CPU_STK      AppTaskObj0Stk[APP_CFG_TASK_OBJ0_STK_SIZE];

static  OS_TCB       AppTaskObj1TCB;
static  CPU_STK      AppTaskObj1Stk[APP_CFG_TASK_OBJ1_STK_SIZE];



/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void  AppTaskStart (void  *p_arg);
static  void  AppTaskCreate(void);
static  void  AppObjCreate (void);

static  void  AppTaskObj0    (void  *p_arg);
static  void  AppTaskObj1    (void  *p_arg);

KEY Key1,Key2;
OS_TMR      MyTmr;
/*
*********************************************************************************************************
*                                                main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*
*********************************************************************************************************
*/

int main(void)
{
	OS_ERR   err;
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_ERR  cpu_err;
#endif                 

	Mem_Init();                                                 /* Initialize Memory Managment Module                   */
	Math_Init();                                                /* Initialize Mathematical Module                       */

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
	CPU_NameSet((CPU_CHAR *)"STM32F429II",
							(CPU_ERR  *)&cpu_err);
#endif

	BSP_IntDisAll();                                            /* Disable all Interrupts.                              */

	OSInit(&err);                                               /* Init uC/OS-III.                                      */
	App_OS_SetAllHooks();

	OSTaskCreate(&AppTaskStartTCB,                              /* Create the start task                                */
								"App Task Start",
								AppTaskStart,
								0u,
								APP_CFG_TASK_START_PRIO,
								&AppTaskStartStk[0u],
								AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10u],
								APP_CFG_TASK_START_STK_SIZE,
								0u,
								0u,
								0u,
							 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 &err);

	OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */

	while (DEF_ON) {                                            /* Should Never Get Here.                               */
			;
	}
}

//按键定时扫描的定时器
static void cbTimer(OS_TMR *p_tmr, void *p_arg)
{
	(void)p_arg;
	GT9xx_GetOnePiont();//触摸屏定时扫描

	Key_RefreshState(&Key1);//刷新按键状态
	Key_RefreshState(&Key2);//刷新按键状态
	if(Key_AccessTimes(&Key1,KEY_ACCESS_READ)!=0)//按键被按下过
	{
		LED2_TOGGLE;
		Key_AccessTimes(&Key1,KEY_ACCESS_WRITE_CLEAR);
	}
	if(Key_AccessTimes(&Key2,KEY_ACCESS_READ)!=0)//按键被按下过
	{
		LED3_TOGGLE;
		Key_AccessTimes(&Key2,KEY_ACCESS_WRITE_CLEAR);
	}
}

/*
*********************************************************************************************************
*                                          STARTUP TASK
*
* Description : This is an example of a startup task.  As mentioned in the book's text, you MUST
*               initialize the ticker only once multitasking has started.
*
* Arguments   : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
*
* Returns     : none
*
* Notes       : 1) The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
*********************************************************************************************************
*/

static  void  AppTaskStart (void *p_arg)
{
	OS_ERR      err;
 (void)p_arg;
 
	CPU_Init();                                                 /* Initialize the uC/CPU services                       */

	BSP_Init();                                                 /* Initialize BSP functions                             */

#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN
	CPU_IntDisMeasMaxCurReset();
#endif

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
	APP_TRACE_DBG(("Creating Application kernel objects\n\r"));
#endif	
	AppObjCreate(); 
/* Create Applicaiton kernel objects                    */
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
	APP_TRACE_DBG(("Creating Application Tasks\n\r"));
#endif	
	AppTaskCreate();                                            /* Create Application tasks                             */

	OSTmrCreate((OS_TMR              *)&MyTmr,
               (CPU_CHAR            *)"MyTimer",          
               (OS_TICK              )2,                
               (OS_TICK              )2,                
               (OS_OPT               )OS_OPT_TMR_PERIODIC,
               (OS_TMR_CALLBACK_PTR  )cbTimer,          
               (void                *)0,
               (OS_ERR              *)err);
  OSTmrStart((OS_TMR *)&MyTmr,(OS_ERR *)err);
							 
	while (DEF_TRUE)
	{                                          /* Task body, always written as an infinite loop.       */
		LED3_TOGGLE;
		APP_TRACE_DBG(("cpuusage:%d\n",OSStatTaskCPUUsage));
		OSTimeDlyHMSM(0u, 0u, 0u, 200u,
									OS_OPT_TIME_HMSM_STRICT,
									&err);
	}
}

/*
*********************************************************************************************************
*                                          AppTaskCreate()
*
* Description : Create Application Tasks.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskCreate (void)
{
	OS_ERR  os_err;

	/* ---------- CREATE KERNEL OBJECTS TEST TASK --------- */
	OSTaskCreate(&AppTaskObj0TCB,
							 "Kernel Objects Task 0",
								AppTaskObj0,
								0,
								APP_CFG_TASK_OBJ0_PRIO,
							 &AppTaskObj0Stk[0],
								AppTaskObj0Stk[APP_CFG_TASK_OBJ0_STK_SIZE / 10u],
								APP_CFG_TASK_OBJ0_STK_SIZE,
								0u,
								0u,
								0,
							 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 &os_err);
 OSTaskCreate(&AppTaskObj1TCB,
							 "Kernel Objects Task 0",
								AppTaskObj1,
								0,
								APP_CFG_TASK_OBJ1_PRIO,
							 &AppTaskObj1Stk[0],
								AppTaskObj1Stk[APP_CFG_TASK_OBJ1_STK_SIZE / 10u],
								APP_CFG_TASK_OBJ1_STK_SIZE,
								0u,
								0u,
								0,
							 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
							 &os_err);

}


/*
*********************************************************************************************************
*                                          AppObjCreate()
*
* Description : Create Application Kernel Objects.
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   : AppTaskStart()
*
* Note(s)     : none.
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
  KeyCreate(&Key1,GetPinStateOfKey1);
	KeyCreate(&Key2,GetPinStateOfKey2);
}
/*
*********************************************************************************************************
*                                          AppTaskObj0()
*
* Description : Test uC/OS-III objects.
*
* Argument(s) : p_arg is the argument passed to 'AppTaskObj0' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Caller(s)   : This is a task
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  void  AppTaskObj0 (void  *p_arg)
{
	OS_ERR  os_err;

	(void)p_arg;

	while (DEF_TRUE) 
	{
		if(TPAD_Scan(0))
		{
			BEEP_ON;
			OSTimeDlyHMSM(0u, 0u, 0u, 100u,
									OS_OPT_TIME_HMSM_STRICT,
									&os_err);
		}
		BEEP_OFF;
		OSTimeDlyHMSM(0u, 0u, 1u, 0u,
									OS_OPT_TIME_HMSM_STRICT,
									&os_err);
	}
}


/*
*********************************************************************************************************
*                                          AppTaskObj1()
*
* Description : Test uC/OS-III objects.
*
* Argument(s) : p_arg is the argument passed to 'AppTaskObj1' by 'OSTaskCreate()'.
*
* Return(s)   : none
*
* Caller(s)   : This is a task
*
* Note(s)     : none.
*********************************************************************************************************
*/
//extern void SpeedTest(void);
extern void Createfatfs(void);
static  void  AppTaskObj1 (void  *p_arg)
{
//	OS_ERR  os_err;
	(void)p_arg;
//	SpeedTest();
//	OSTimeDlyHMSM(0u, 0u, 3u, 0u,
//								OS_OPT_TIME_HMSM_STRICT,
//								&os_err);
	
	while (DEF_TRUE) 
	{
    Createfatfs();		
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
