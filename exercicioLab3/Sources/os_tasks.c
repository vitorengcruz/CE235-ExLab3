/* ###################################################################
**     Filename    : os_tasks.c
**     Project     : exercicioLab2
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-08-29, 16:47, # CodeGen: 1
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Settings    :
**     Contents    :
**         Task2_task - void Task2_task(os_task_param_t task_init_data);
**         Task1_task - void Task1_task(os_task_param_t task_init_data);
**
** ###################################################################*/
/*!
** @file os_tasks.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup os_tasks_module os_tasks module documentation
**  @{
*/         
/* MODULE os_tasks */

#include "Cpu.h"
#include "Events.h"
#include "rtos_main_task.h"
#include "os_tasks.h"

#include "ledrgb_drive.h"

#ifdef __cplusplus
extern "C" {
#endif 

/*
** ===================================================================++
**     Callback    : Task2_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void Task2_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	uint8_t estadoLedVermelho = 0;

#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    switch( estadoLedVermelho ) {
    case 0:
    	if( !driveLedRgb_liga(LED_RGB_VERMELHO) )
    		while(1);
		estadoLedVermelho = 1;
    	OSA_TimeDelay(1000);
    	break;
    case 1:
    	if( !driveLedRgb_desliga() )
    		while(1);
    	estadoLedVermelho = 0;
		OSA_TimeDelay(500);
		break;
    }
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/*
** ===================================================================
**     Callback    : Task1_task
**     Description : Task function entry.
**     Parameters  :
**       task_init_data - OS task parameter
**     Returns : Nothing
** ===================================================================
*/
void Task1_task(os_task_param_t task_init_data)
{
  /* Write your local variable definition here */
	uint8_t estadoLedVerde = 0;
  
#ifdef PEX_USE_RTOS
  while (1) {
#endif
    /* Write your code here ... */
    
    switch( estadoLedVerde ) {
    case 0:
    	if( !driveLedRgb_liga(LED_RGB_VERDE) )
    		while(1);
    	estadoLedVerde = 1;
    	OSA_TimeDelay(1000);
		break;
    case 1:
    	if( !driveLedRgb_desliga() )
    		while(1);
		estadoLedVerde = 0;
		OSA_TimeDelay(500);
		break;
    }
    
    
#ifdef PEX_USE_RTOS   
  }
#endif    
}

/* END os_tasks */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
