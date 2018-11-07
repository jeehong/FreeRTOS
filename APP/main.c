/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Library includes. */
#include  <stm32f2xx.h>
#include "app_led.h"
#include "serial.h"

#include "app_cli.h"

/* vfs */
#include "vfs.h"
#include "vfs_register.h"
#include "vfs_err.h"
#include "vfs_i2c.h"

/* The check task uses the sprintf function so requires a little more stack. */
#define mainLED_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )

/* The time between cycles of the 'check' task. */

int main(void)
{
	int fd_i2c = 0;
	uint8_t write_buf = 0;
    uint8_t read_buf = 0;
	char *i2c_path = "/dev/i2c";
	i2c_dev_t i2c_dev_test =
	{
		I2C1,			/* .port */
		{
			I2C_AcknowledgedAddress_7bit,			/* .config.address_width */
			100000,		/* .config.freq */
			0x33,		/* .config.mode */
			0x64		/* .config.dev_addr */
		},
		NULL
	};

#ifdef DEBUG
  debug();
#endif
	
	SystemInit();
	vfs_init();

	aos_register_driver(i2c_path, vfs_i2c_opts(), &i2c_dev_test);

	BSP_LED_Init();
	serial_task_init(1);
	app_cli_init();

	fd_i2c = vfs_open(i2c_path, 0);
	
	vfs_write(fd_i2c, &write_buf, sizeof(read_buf));
	vfs_read(fd_i2c, &read_buf, sizeof(read_buf));
	vfs_close(fd_i2c);

	/* Start the tasks defined within this file/specific to this demo. */
	xTaskCreate( vByteTask, "Byte", mainLED_TASK_STACK_SIZE, NULL, 4, NULL );
	xTaskCreate( vLed1Task, "Led1", mainLED_TASK_STACK_SIZE, NULL, 3, NULL );
	xTaskCreate( vLed2Task, "Led2", mainLED_TASK_STACK_SIZE, NULL, 3, NULL );
	xTaskCreate( vLed3Task, "Led3", mainLED_TASK_STACK_SIZE, NULL, 3, NULL );
	xTaskCreate( vLed4Task, "Led4", mainLED_TASK_STACK_SIZE, NULL, 3, NULL );

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was not enough heap space to create the
	idle task. */
	return 0;
}




