/*
 * @brief Drive para controlar a cor do LED RGB da FRDM-KL25Z.
 * @author V�tor Cruz.
 */

#include "Cpu.h"
#include "Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "ledrgb_hal.h"
#include "ledrgb_drive.h"


// Cria o sem�foro/mutex para controlar a posse do LED RGB.
SemaphoreHandle_t ledSemaphore = NULL;


// Define uma tabela com os par�metros do LED RGB para gerar a cor desejada.
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
 * Inicializa o LED RGB, o seu sem�foro e executa uma anima��o com as cores.
 */
bool driveLedRgb_Init ( void ) {
	corLedRgbEnum_t cor = LED_RGB_VERDE;

// Inicializa sem�foro do LED RGB.
#if (USAR_MUTEX_LED_RGB==0)
	ledSemaphore = xSemaphoreCreateBinary();
#else
	ledSemaphore = xSemaphoreCreateMutex();
#endif

	// N�o conseguiu inicializar o sem�foro
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

// Executa uma anima��o das cores dos LEDs.
	if( !driveLedRgb_liga( cor++ ) )
		return false;

	for( ; cor < LED_RGB_QUANTIDADE_CORES; cor++ ) {
		OSA_TimeDelay(500);
		if( !driveLedRgb_trocaCor( cor ) )
			return false;
	}

	OSA_TimeDelay(500);

// Libera o LED para a pr�xima task.
	if( !driveLedRgb_desliga() )
		return false;

	return true;
}

/*
 * Rotina para ligar o led com uma cor desejada.
 */
bool driveLedRgb_liga ( corLedRgbEnum_t cor ) {

// Verifica se a cor � v�lida.
	if( cor >= LED_RGB_QUANTIDADE_CORES )
		return false;

#if (USAR_MUTEX_LED_RGB==1)
/*
 * No caso do mutex, verifica se a task j� possui o sem�foro, que � uma condi��o de falha j� que n�o
 * � poss�vel peg�-lo novamente na mesma task sem antes liber�-lo.
 */
	if (xSemaphoreGetMutexHolder( ledSemaphore ) == xTaskGetCurrentTaskHandle())
	{
		return false;
	}
#endif

	/* Para usar o led, a tarefa deve primeiro verificar se o recurso est� dispon�vel para ser usado.
	 * Na rotina xSemaphoreTake, a tarefa tenta adquirir o sem�foro, com a possibilidade de programar 3 tipos de
	 * tempo limite para aguardar a sua mudan�a:
	 *
	 * Colocando valor 0: A rotina n�o aguarda o sem�foro mudar, s� verifica o estado no momento da chamada.
	 *
	 * Colocando um valor em ticks ou usando a macro pdMS_TO_TICKS: o sistema vai aguardar o
	 * o tempo indicado em n�mero de ticks, que � o mecanismo de contagem de tempo do FreeRTOS.
	 *
	 * Colocando a macro portMAX_DELAY, que faz com que a rotina fique aguardando indefinidamente.
	 *
	 * A rotina retornar� o valor pdTRUE quando conseguir adquirir o sem�foro antes de esgotar o tempo limite de
	 * espera, e retornar� pdFALSE quando o tempo limite estiver esgotado e o sem�foro n�o foi devolvido.
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

	// Verifica se a cor � v�lida.
	if( cor >= LED_RGB_QUANTIDADE_CORES )
		return false;

#if (USAR_MUTEX_LED_RGB==1)
	/*
	 * No caso do mutex, verifica se a task j� possui o sem�foro, para que ent�o possa trocar a cor do LED.
	 * Se a task n�o possui o sem�foro, n�o � poss�vel efetuar a troca pois outra task est� com
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
	 * No caso do mutex, verifica se a task j� possui o sem�foro, para que ent�o possa liber�-lo.
	 * Se a task n�o possui o sem�foro, n�o � poss�vel liber�-lo e a rotina retorna uma condi��o de erro.
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

// Devolve o controle do sem�foro.
 	if( xSemaphoreGive( ledSemaphore ) == pdFALSE )
	{
		return false;
	}

	return true;
}

