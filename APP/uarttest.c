
/* Scheduler include files. */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo program include files. */
#include "serial.h"
#include "uarttest.h"


/* Library includes. */
#include "stm32f2xx.h"


#define comSTACK_SIZE				configMINIMAL_STACK_SIZE
#define comTX_LED_OFFSET			( 0 )
#define comRX_LED_OFFSET			( 1 )
#define comTOTAL_PERMISSIBLE_ERRORS ( 2 )

/* The Tx task will transmit the sequence of characters at a pseudo random
interval.  This is the maximum and minimum block time between sends. */
#define comTX_MAX_BLOCK_TIME		( ( TickType_t ) 0x96 )
#define comTX_MIN_BLOCK_TIME		( ( TickType_t ) 0x32 )
#define comOFFSET_TIME				( ( TickType_t ) 3 )

/* We should find that each character can be queued for Tx immediately and we
don't have to block to send. */
#define comNO_BLOCK					( ( TickType_t ) 0 )

/* The Rx task will block on the Rx queue for a long period. */
#define comRX_BLOCK_TIME			( ( TickType_t ) 0xffff )

/* The sequence transmitted is from comFIRST_BYTE to and including comLAST_BYTE. */
#define comFIRST_BYTE				( 'A' )
#define comLAST_BYTE				( 'z' )

#define comBUFFER_LEN				(100)
#define comINITIAL_RX_COUNT_VALUE	( 0 )

/* Handle to the com port used by both tasks. */
static xComPortHandle xPort = NULL;

/* The transmit task as described at the top of the file. */
static portTASK_FUNCTION_PROTO( vComTxTask, pvParameters );

/* The receive task as described at the top of the file. */
static portTASK_FUNCTION_PROTO( vComRxTask, pvParameters );


/* Check variable used to ensure no error have occurred.  The Rx task will
increment this variable after every successfully received sequence.  If at any
time the sequence is incorrect the the variable will stop being incremented. */
static volatile UBaseType_t uxRxLoops = comINITIAL_RX_COUNT_VALUE;

/*-----------------------------------------------------------*/

void vAltStartComTestTasks( UBaseType_t uxPriority, uint32_t ulBaudRate)
{
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	//xSerialPortInitMinimal( ulBaudRate, comBUFFER_LEN );

	/* The Tx task is spawned with a lower priority than the Rx task. */
	xTaskCreate( vComTxTask, "COMTx", comSTACK_SIZE, NULL, uxPriority + 1, ( TaskHandle_t * ) NULL ); 
	//xTaskCreate( vComRxTask, "COMRx", comSTACK_SIZE, NULL, uxPriority, ( TaskHandle_t * ) NULL );
}

void dbg_string(const char *fmt, ...)
{
	va_list vp;
	char dbg_buf[100];
	
	va_start(vp, fmt);
	vsprintf(dbg_buf, fmt, vp);
	va_end(vp);
	
	vSerialPutString(xPort, (signed char *)&dbg_buf, comNO_BLOCK);
}


/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vComTxTask, pvParameters )
{
	signed char txByte;
	/* Just to stop compiler warnings. */
	( void ) pvParameters;
	
	for(;;)
	{
		if(xQueueReceive( xCharsForTx, &txByte, 0) == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData(USART1, txByte);
		}
		vTaskDelay(1);
	}
} /*lint !e715 !e818 pvParameters is required for a task function even if it is not referenced. */
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vComRxTask, pvParameters )
{
	signed char cByteRxed;
	const signed char tx_buffer[] = "Terminal:";

	/* Just to stop compiler warnings. */
	( void ) pvParameters;

	for( ;; )
	{
			xSerialGetChar( xPort, &cByteRxed, comRX_BLOCK_TIME );

			if(cByteRxed == 13)
			{
				vSerialPutString(xPort, tx_buffer, strlen(tx_buffer));
			}
	}
} /*lint !e715 !e818 pvParameters is required for a task function even if it is not referenced. */
/*-----------------------------------------------------------*/

BaseType_t xAreComTestTasksStillRunning( void )
{
	BaseType_t xReturn;

	/* If the count of successful reception loops has not changed than at
	some time an error occurred (i.e. a character was received out of sequence)
	and we will return false. */
	if( uxRxLoops == comINITIAL_RX_COUNT_VALUE )
	{
		xReturn = pdFALSE;
	}
	else
	{
		xReturn = pdTRUE;
	}

	/* Reset the count of successful Rx loops.  When this function is called
	again we expect this to have been incremented. */
	uxRxLoops = comINITIAL_RX_COUNT_VALUE;

	return xReturn;
}

