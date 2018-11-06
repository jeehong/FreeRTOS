/* FreeRTOS includes. */
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

/* FreeRTOS+CLI includes. */
#include "mid_cli.h" 
#include "hal_cli.h" 
#include "serial.h" 

build_var(info, "Device information.", 0);
build_var(clear, "Clear Terminal.", 0);

static BaseType_t info_main( char *dest, argv_attribute argv, const char * const help_info)
{
	unsigned char index;
	const static char *dev_info[] =
	{
		" 	Dev:	STM32F205RGT6 \r\n",
		" 	Cpu:	ARM 32-bit Cortex-M3\r\n",
		" 	Freq: 	120 MHz max,1.25 DMIPS/MHz\r\n",
		" 	Mem:	1 Mbytes of Flash memory\r\n",
		" 	Ram:	128 Kbytes of SRAM\r\n",
		NULL
	};

	(void) help_info;
	configASSERT(dest);

	for(index = 0; dev_info[index] != NULL; index++)
	{
		strcat(dest, dev_info[index]);
	}

	return pdFALSE;
}

static BaseType_t clear_main( char *dest, argv_attribute argv, const char * const help_info)
{
	const static char *string = "\033[H\033[J";

	(void) help_info;
	configASSERT(dest);

	strcpy(dest, string);

	return pdFALSE;
}

static void app_cli_default_register(void)
{
	hal_cli_data_switch_register(serial_tx, serial_rx);
	/* Register all the command line commands defined immediately above. */
	mid_cli_register(&info);
    mid_cli_register(&clear);
}

void app_cli_init(void)
{
	mid_cli_init(1000, 3, "Terminal: ", NULL);

	/* Register commands with the FreeRTOS+CLI command interpreter. */
	app_cli_default_register();
}


