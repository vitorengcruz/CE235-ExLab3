/*
 * @brief Drive para controlar a cor do LED RGB da FRDM-KL25Z.
 * @author Vítor Cruz.
 */

#include "Cpu.h"
#include "Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "ledrgb_hal.h"
#include "ledrgb_drive.h"


// Cria o semáforo/mutex para controlar a posse do LED RGB.
SemaphoreHandle_t ledSemaphore = NULL;


// Define uma tabela com os parâmetros do LED RGB para gerar a cor desejada.
typedef struct {
	uint8_t ledVermelho:1;
	uint8_t ledVerde:1;
	uint8_t ledAzul:1;
}posicaoLedRgbStruct_t;

posicaoLedRgbStruct_t palhetaCores[LED_RGB_QUANTIDADE_CORES] = {
	{ .ledVermelho = 0, .ledVerde = 1, .ledAzul = 0 },			// Verde
	{ .ledVermelho = 1, .ledVerde = 0, .ledAzul = 0 },			// Vermelho
	{ .ledVermelho = 0, .ledVerde = 0, .ledAzul = 1 },			// Azul
	{ .ledVermelho = 1, .ledVerde = 1, .ledAzul = 0 },			// Amarelo
	{ .ledVermelho = 0, .ledVerde = 1, .ledAzul = 1 },			// Ciano
	{ .ledVermelho = 1, .ledVerde = 0, .ledAzul = 1 },			// Magenta
	{ .ledVermelho = 1, .ledVerde = 1, .ledAzul = 1 }			// Branco
};

/*
 * Inicializa o LED RGB, o seu semáforo e executa uma animação com as cores.
 */
bool driveLedRgb_Init ( void ) {
	corLedRgbEnum_t cor = LED_RGB_VERDE;

// Inicializa semáforo do LED RGB.
#if (USAR_MUTEX_LED_RGB==0)
	ledSemaphore = xSemaphoreCreateBinary();
#else
	ledSemaphore = xSemaphoreCreateMutex();
#endif

	// Não conseguiu inicializar o semáforo
	if( ledSemaphore == NULL ) {
		return false;
	}

#if (USAR_MUTEX_LED_RGB==0)
	if( xSemaphoreGive( ledSemaphore ) == pdFALSE )
	{
		return false;
	}
#endif
// Inicializa os pinos do LED RGB.
	ledrgb_init();

// Executa uma animação das cores dos LEDs.
	if( !driveLedRgb_liga( cor++ ) )
		return false;

	for( ; cor < LED_RGB_QUANTIDADE_CORES; cor++ ) {
		OSA_TimeDelay(500);
		if( !driveLedRgb_trocaCor( cor ) )
			return false;
	}

	OSA_TimeDelay(500);

// Libera o LED para a próxima task.
	if( !driveLedRgb_desliga() )
		return false;

	return true;
}

/*
 * Rotina para ligar o led com uma cor desejada.
 */
bool driveLedRgb_liga ( corLedRgbEnum_t cor ) {

// Verifica se a cor é válida.
	if( cor >= LED_RGB_QUANTIDADE_CORES )
		return false;

#if (USAR_MUTEX_LED_RGB==1)
/*
 * No caso do mutex, verifica se a task já possui o semáforo, que é uma condição de falha já que não
 * é possível pegá-lo novamente na mesma task sem antes liberá-lo.
 */
	if (xSemaphoreGetMutexHolder( ledSemaphore ) == xTaskGetCurrentTaskHandle())
	{
		return false;
	}
#endif

	/* Para usar o led, a tarefa deve primeiro verificar se o recurso está disponível para ser usado.
	 * Na rotina xSemaphoreTake, a tarefa tenta adquirir o semáforo, com a possibilidade de programar 3 tipos de
	 * tempo limite para aguardar a sua mudança:
	 *
	 * Colocando valor 0: A rotina não aguarda o semáforo mudar, só verifica o estado no momento da chamada.
	 *
	 * Colocando um valor em ticks ou usando a macro pdMS_TO_TICKS: o sistema vai aguardar o
	 * o tempo indicado em número de ticks, que é o mecanismo de contagem de tempo do FreeRTOS.
	 *
	 * Colocando a macro portMAX_DELAY, que faz com que a rotina fique aguardando indefinidamente.
	 *
	 * A rotina retornará o valor pdTRUE quando conseguir adquirir o semáforo antes de esgotar o tempo limite de
	 * espera, e retornará pdFALSE quando o tempo limite estiver esgotado e o semáforo não foi devolvido.
	 */

	if( xSemaphoreTake( ledSemaphore, portMAX_DELAY ) == pdFALSE ) {
		return false;
	}

// Liga os LEDs correspondentes a cor desejada.
	if( palhetaCores[cor].ledVermelho )
		ledrgb_setRedLed();
	else
		ledrgb_clearRedLed();

	if( palhetaCores[cor].ledVerde )
		ledrgb_setGreenLed();
	else
		ledrgb_clearGreenLed();

	if( palhetaCores[cor].ledAzul )
		ledrgb_setBlueLed();
	else
		ledrgb_clearBlueLed();

	return true;
}

bool driveLedRgb_trocaCor ( corLedRgbEnum_t cor  ) {

	// Verifica se a cor é válida.
	if( cor >= LED_RGB_QUANTIDADE_CORES )
		return false;

#if (USAR_MUTEX_LED_RGB==1)
	/*
	 * No caso do mutex, verifica se a task já possui o semáforo, para que então possa trocar a cor do LED.
	 * Se a task não possui o semáforo, não é possível efetuar a troca pois outra task está com
	 * a posse do recurso neste momento, sendo assim retorna falha.
	 */
	if (xSemaphoreGetMutexHolder( ledSemaphore ) != xTaskGetCurrentTaskHandle())
	{
	  return false;
	}
#endif

	// Liga os LEDs correspondentes a cor desejada.
	if( palhetaCores[cor].ledVermelho )
		ledrgb_setRedLed();
	else
		ledrgb_clearRedLed();

	if( palhetaCores[cor].ledVerde )
		ledrgb_setGreenLed();
	else
		ledrgb_clearGreenLed();

	if( palhetaCores[cor].ledAzul )
		ledrgb_setBlueLed();
	else
		ledrgb_clearBlueLed();

	return true;
}

bool driveLedRgb_desliga ( void ) {
#if (USAR_MUTEX_LED_RGB==1)
	/*
	 * No caso do mutex, verifica se a task já possui o semáforo, para que então possa liberá-lo.
	 * Se a task não possui o semáforo, não é possível liberá-lo e a rotina retorna uma condição de erro.
	 */
	if (xSemaphoreGetMutexHolder( ledSemaphore ) != xTaskGetCurrentTaskHandle())
	{
	  return false;
	}
#endif

// Desliga todas as cores.
	ledrgb_clearRedLed();
	ledrgb_clearGreenLed();
	ledrgb_clearBlueLed();

// Devolve o controle do semáforo.
 	if( xSemaphoreGive( ledSemaphore ) == pdFALSE )
	{
		return false;
	}

	return true;
}

