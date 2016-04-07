#ifndef __APP_LED_H__
#define __APP_LED_H__


	
#define mainDELAY						( ( TickType_t ) 100 / portTICK_PERIOD_MS )
	 
void  BSP_LED_Init(void);
void vLed1Task( void *pvParameters );
void vLed2Task( void *pvParameters );
void vLed3Task( void *pvParameters );
void vLed4Task( void *pvParameters );
	 

#endif
