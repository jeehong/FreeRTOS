/* Scheduler includes. */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"

/* Library includes. */
#include "stm32f2xx.h"

/* Demo application includes. */
#include "serial.h"
/*-----------------------------------------------------------*/

/* Misc defines. */
#define serINVALID_QUEUE				((QueueHandle_t )0)
#define serNO_BLOCK						((TickType_t )0)
#define serTX_BLOCK_TIME				(40 / portTICK_PERIOD_MS)
#define	serBAUD_RATE					(115200)

#define comSTACK_SIZE				configMINIMAL_STACK_SIZE
#define comTX_LED_OFFSET			(0)
#define comRX_LED_OFFSET			(1)
#define comTOTAL_PERMISSIBLE_ERRORS (2)

/* The Tx task will transmit the sequence of characters at a pseudo random
interval.  This is the maximum and minimum block time between sends. */
#define comTX_MAX_BLOCK_TIME		((TickType_t)0x96)
#define comTX_MIN_BLOCK_TIME		((TickType_t)0x32)
#define comOFFSET_TIME				((TickType_t)3)

/* We should find that each character can be queued for Tx immediately and we
don't have to block to send. */
#define comNO_BLOCK					((TickType_t)0)

/* The Rx task will block on the Rx queue for a long period. */
#define comRX_BLOCK_TIME			((TickType_t)0xffff)

#define comBUFFER_LEN				(100)
#define comINITIAL_RX_COUNT_VALUE	(0)
/*-----------------------------------------------------------*/


/* The queue used to hold received characters. */
static QueueHandle_t ser_rx_queue;
static QueueHandle_t ser_tx_queue;

/*-----------------------------------------------------------*/

/* UART interrupt handler. */
void USART1_IRQHandler(void);

/* Handle to the com port used by both tasks. */
static xComPortHandle xPort = NULL;

/* The receive task as described at the top of the file. */
static portTASK_FUNCTION_PROTO(dataLinktask, pvParameters);

/*-----------------------------------------------------------*/

void serial_task_init(UBaseType_t uxPriority)
{
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xPort = serial_bsp_init(serBAUD_RATE, comBUFFER_LEN);

	/* The Tx task is spawned with a lower priority than the Rx task. */
	xTaskCreate(dataLinktask, "Serial", comSTACK_SIZE * 2, NULL, uxPriority, ( TaskHandle_t * )NULL);
}

void dbg_string(const char *fmt, ...)
{
	va_list vp;
	char dbg_buf[100];
	
	va_start(vp, fmt);
	vsprintf(dbg_buf, fmt, vp);
	va_end(vp);
	
	vSerialPutString(xPort, (signed char *)&dbg_buf, strlen(dbg_buf));
}

static portTASK_FUNCTION(dataLinktask, pvParameters)
{
	signed char txByte;
	signed char cByteRxed;

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	for( ;; )
	{
		xSerialGetChar(xPort, &cByteRxed, comRX_BLOCK_TIME);

		if(xQueueReceive(ser_tx_queue, &txByte, 0) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData(USART1, txByte);
		}
		vTaskDelay(1);
	}
} /*lint !e715 !e818 pvParameters is required for a task function even if it is not referenced. */


/*-----------------------------------------------------------*/

/*
 * See the serial2.h header file.
 */
xComPortHandle serial_bsp_init(unsigned long baud_rate, unsigned portBASE_TYPE uxQueueLength)
{
	xComPortHandle xReturn;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Create the queues used to hold Rx/Tx characters. */
	ser_rx_queue = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	ser_tx_queue = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed char ) );
	
	/* If the queue/semaphore was created correctly then setup the serial port
	hardware. */
	if( ( ser_rx_queue != serINVALID_QUEUE ) && ( ser_tx_queue != serINVALID_QUEUE ) )
	{
		/* Enable USART1 clock */
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_AHB1Periph_GPIOA, ENABLE );	
		
		/* Connect PXx to USARTx_Tx*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);

		/* Connect PXx to USARTx_Rx*/
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
		
		/* Configure USART1 Rx (PA10) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init( GPIOA, &GPIO_InitStructure );
		
		/* Configure USART1 Tx (PA9) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_Init( GPIOA, &GPIO_InitStructure );

		USART_InitStructure.USART_BaudRate = baud_rate;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No ;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		
		USART_Init( USART1, &USART_InitStructure );
		
		USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
		
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_KERNEL_INTERRUPT_PRIORITY;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init( &NVIC_InitStructure );
		
		USART_Cmd( USART1, ENABLE );		
	}
	else
	{
		xReturn = ( xComPortHandle ) 0;
	}

	/* This demo file only supports a single port but we have to return
	something to comply with the standard demo header file. */
	return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed char *prx_char, TickType_t xBlockTime )
{
	/* The port handle is not required as this driver only supports one port. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( ser_rx_queue, prx_char, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString(xComPortHandle pxPort, signed char const  *pcString, unsigned short usStringLength)
{
	signed char *p;

	/* A couple of parameters that this port does not use. */
	( void ) usStringLength;
	( void ) pxPort;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART1. */
	( void ) pxPort;

	/* Send each character in the string, one at a time. */
	p = (signed char *)pcString;
	while(*p != '\0')
	{
		xSerialPutChar(pxPort, *p, serNO_BLOCK);
		p++;
	}
}
/*-----------------------------------------------------------*/

unsigned short serial_tx(char *data, unsigned int len)
{
	vSerialPutString(xPort, (signed char *)data, len);
	return 0;
}

unsigned short serial_rx(char *data, unsigned int len)
{
	if(xQueueReceive(ser_rx_queue, data, portMAX_DELAY))
		return pdTRUE;
	else
		return pdFALSE;
}


signed portBASE_TYPE xSerialPutChar(xComPortHandle pxPort, signed char cOutChar, TickType_t xBlockTime)
{
	signed portBASE_TYPE xReturn;

	if(xQueueSend(ser_tx_queue, &cOutChar, xBlockTime) == pdPASS)
	{
		xReturn = pdPASS;
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
	else
	{
		xReturn = pdFAIL;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
	/* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

void USART1_IRQHandler( void )
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	char cChar;
    
	if( USART_GetITStatus( USART1, USART_IT_TXE ) == SET )
	{
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		while(xQueueReceiveFromISR( ser_tx_queue, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData( USART1, cChar );
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET)
                ;
		}
		USART_ClearITPendingBit(USART1, USART_IT_TXE);
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	}	
    
	if( USART_GetITStatus( USART1, USART_IT_RXNE ) == SET )
	{
		cChar = USART_ReceiveData( USART1 );
		xQueueSendFromISR( ser_rx_queue, &cChar, &xHigherPriorityTaskWoken );
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}	
	
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}





	
