#ifndef __APP_LED_H__
#define __APP_LED_H__

#define mainDELAY_MS(ms)			( ( TickType_t ) ms / portTICK_PERIOD_MS )

void  BSP_LED_Init(void);
void vLed1Task( void *pvParameters );
void vLed2Task( void *pvParameters );
void vLed3Task( void *pvParameters );
void vLed4Task( void *pvParameters );
	 

#endif
