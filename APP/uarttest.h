
#ifndef __UARTTEST_H
#define __UARTTEST_H


void vAltStartComTestTasks( UBaseType_t uxPriority, uint32_t ulBaudRate);
void vStartComTestTasks( UBaseType_t uxPriority, eCOMPort ePort, eBaud eBaudRate );
BaseType_t xAreComTestTasksStillRunning( void );
void vComTestUnsuspendTask( void );

#endif

