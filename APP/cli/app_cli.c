/* FreeRTOS includes. */
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+CLI includes. */
#include "mid_cli.h" 
#include "hal_cli.h" 
#include "serial.h" 

static BaseType_t hi_handle(const char * const input, char *dest, size_t len, const char * const help_info);
static BaseType_t info_handle(const char * const input, char *dest, size_t len, const char * const help_info);
static BaseType_t clear_handle(const char * const input, char *dest, size_t len, const char * const help_info);

static const struct cli_module_t hi =
{
	"hi",
	"Nice to meet you!\r\n",
	hi_handle,
	0
};

static const struct cli_module_t info =
{
	"info",
	"Device information.\r\n",
	info_handle,
	0
};

static const struct cli_module_t clear =
{
	"clear",
	"Clear Terminal.\r\n",
	clear_handle,
	0
};

static BaseType_t hi_handle(const char * const input, char *dest, size_t len, const char * const help_info)
{
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) help_info;
	( void ) len;
	configASSERT(dest);

	/* Generate a table of task stats. */
    sprintf(dest, "    %s", help_info);
	
	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

static BaseType_t info_handle(const char * const input, char *dest, size_t len, const char * const help_info)
{
	unsigned char index;
	const static char *dev_info[] =
	{
		" 	Dev:	STM32F205RGT6 \r\n",
		" 	Cpu:	ARM 32-bit Cortex-M3\r\n",
		" 	Freq: 	120 MHz max,150 DMIPS/1.25 DMIPS/MHz\r\n",
		" 	Mem:	1 Mbyte of Flash memory\r\n",
		" 	Ram:	128 + 4 Kbytes of SRAM\r\n",
		NULL
	};

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) help_info;
	( void ) len;
	configASSERT(dest);

	/* Generate a table of task stats. */
    sprintf(dest, "    %s", help_info);
	for(index = 0; dev_info[index] != NULL; index++)
	{
		strcat(dest, dev_info[index]);
	}

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

static BaseType_t clear_handle(const char * const input, char *dest, size_t len, const char * const help_info)
{
	const static char *string = "\033[H\033[J";

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) help_info;
	( void ) len;
	configASSERT(dest);

	/* Generate a table of task stats. */
	strcpy(dest, string);

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

static void app_cli_default_register(void)
{
	hal_cli_data_switch_register(serial_tx, serial_rx);
	/* Register all the command line commands defined immediately above. */
	mid_cli_module_register(&hi);
	mid_cli_module_register(&info);
    mid_cli_module_register(&clear);
}

void app_cli_init(void)
{
	mid_cli_init(1000, 3);

	/* Register commands with the FreeRTOS+CLI command interpreter. */
	app_cli_default_register();
}


