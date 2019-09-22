typedef enum {
	LED_RGB_VERDE = 0,
	LED_RGB_VERMELHO,
	LED_RGB_AZUL,
	LED_RGB_AMARELO,
	LED_RGB_CIANO,
	LED_RGB_MAGENTA,
	LED_RGB_BRANCO,
	LED_RGB_QUANTIDADE_CORES
}corLedRgbEnum_t;

bool driveLedRgb_Init ( void );
bool driveLedRgb_liga ( corLedRgbEnum_t cor );
bool driveLedRgb_trocaCor ( corLedRgbEnum_t cor  );
bool driveLedRgb_desliga ( void );
