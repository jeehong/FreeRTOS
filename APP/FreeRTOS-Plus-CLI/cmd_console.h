#ifndef __CMD_CONSOLE_H__
#define	__CMD_CONSOLE_H__

#include "FreeRTOS.h" 

void vUARTCommandConsoleStart( unsigned short usStackSize, UBaseType_t uxPriority );

#endif
