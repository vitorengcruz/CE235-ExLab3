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

#ifdef __cplusplus
extern "C" {
#endif 

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "ledrgb_hal.h"

extern SemaphoreHandle_t ledSemaphore;

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
    	ledrgb_clearRedLed();
    	if( xSemaphoreGive( ledSemaphore ) != pdTRUE )
		{
		// Implementação "Burra" para testes no debug.
			while(1);
		}
		estadoLedVermelho = 1;
    	OSA_TimeDelay(500);
    	break;
    case 1:
		/* Para usar o led, a tarefa deve primeiro verificar se o recurso está disponível para ser usado.
		 * Na rotina xSemaphoreTake, a tarefa tenta adquirir o semáforo, com a possibilidade de programar 3 tipos de
		 * tempo limite para aguardar a sua mudança:
		 *
		 * Colocando valor 0: A rotina não aguarda o semáforo mudar, só verifica o estado atual.
		 *
		 * Colocando um valor em ticks ou usando a macro pdMS_TO_TICKS: o sistema vai aguardar o
		 * o tempo indicado em número de ticks, que é o mecanismo de contagem de tempo do FreeRTOS.
		 *
		 * Colocando a macro portMAX_DELAY, que faz com que a rotina fique aguardando indefinidamente.
		 *
		 * A rotina retornará o valor pdTRUE quando conseguir adquirir o semáforo antes de esgotar o tempo limite de
		 * espera, e retornará pdFALSE quando o tempo limite estiver esgotado.
		 */
    	if( xSemaphoreTake( ledSemaphore, pdMS_TO_TICKS(1500) ) == pdFALSE ) {
    	// Implementação "Burra" para testes no debug.
    		while(1);
    	}
    	ledrgb_setRedLed();
    	estadoLedVermelho = 0;
		OSA_TimeDelay(1000);
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
		/* Para usar o led, a tarefa deve primeiro verificar se o recurso está disponível para ser usado.
		 * Na rotina xSemaphoreTake, a tarefa tenta adquirir o semáforo, com a possibilidade de programar 3 tipos de
		 * tempo limite para aguardar a sua mudança:
		 *
		 * Colocando valor 0: A rotina não aguarda o semáforo mudar, só verifica o estado atual.
		 *
		 * Colocando um valor em ticks ou usando a macro pdMS_TO_TICKS: o sistema vai aguardar o
		 * o tempo indicado em número de ticks, que é o mecanismo de contagem de tempo do FreeRTOS.
		 *
		 * Colocando a macro portMAX_DELAY, que faz com que a rotina fique aguardando indefinidamente.
		 *
		 * A rotina retornará o valor pdTRUE quando conseguir adquirir o semáforo antes de esgotar o tempo limite de
		 * espera, e retornará pdFALSE quando o tempo limite estiver esgotado.
		 */
    	if( xSemaphoreTake( ledSemaphore, pdMS_TO_TICKS(1500) ) == pdFALSE ) {
    	// Implementação "Burra" para testes no debug.
    		while(1);
    	}
    	ledrgb_setGreenLed();
    	estadoLedVerde = 1;
    	OSA_TimeDelay(1000);
		break;
    case 1:
    	ledrgb_clearGreenLed();
    	if( xSemaphoreGive( ledSemaphore ) != pdTRUE )
		{
		// Implementação "Burra" para testes no debug.
			while(1);
		}
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
