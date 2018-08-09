#include "FreeRTOS.h"
#include "task.h"

/* Library includes. */
#include  <stm32f2xx.h>

#include "app_led.h"

void  BSP_LED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_12);
	GPIO_SetBits(GPIOA, GPIO_Pin_11);	
	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
}

void vLed1Task( void *pvParameters )
{
	portTickType xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();
  	while(1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_12);
		vTaskDelayUntil(&xLastWakeTime, mainDELAY_MS(1900));	
		GPIO_ResetBits(GPIOA, GPIO_Pin_12);
		vTaskDelayUntil(&xLastWakeTime, mainDELAY_MS(100));	
	}
}

void vLed2Task( void *pvParameters )
{
	portTickType xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();
  	while(1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_11);
		vTaskDelayUntil(&xLastWakeTime, mainDELAY_MS(1900));	
		GPIO_ResetBits(GPIOA, GPIO_Pin_11);
		vTaskDelayUntil(&xLastWakeTime, mainDELAY_MS(100));	
	}
}

void vLed3Task( void *pvParameters )
{
	portTickType xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();
  	while(1)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
		vTaskDelayUntil(&xLastWakeTime, mainDELAY_MS(1900));
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		vTaskDelayUntil(&xLastWakeTime, mainDELAY_MS(100));
	}
}

void vLed4Task( void *pvParameters )
{
	portTickType xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();
  	while(1)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
		vTaskDelayUntil(&xLastWakeTime, mainDELAY_MS(500));
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		vTaskDelayUntil(&xLastWakeTime, mainDELAY_MS(500));
	}
}

