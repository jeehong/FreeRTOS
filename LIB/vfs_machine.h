#ifndef __VFS_MACHINE_H__
#define __VFS_MACHINE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

/* memory allocate related */
#define aos_malloc(size) pvPortMalloc(size)
#define aos_free(obj) vPortFree(obj)

/* mutex related */
#define aos_mutex_t QueueHandle_t
#define aos_mutex_new(m, r) do {m = xSemaphoreCreateMutex(); if(m == NULL) r = -1; else r = VFS_SUCCESS; }while(0)
#define aos_mutex_lock(m, arg, r) do {if(xSemaphoreTake(m, portMAX_DELAY) != pdTRUE) return r = -1; else r = VFS_SUCCESS; } while(0)
#define aos_mutex_unlock(m, r) do {if(xSemaphoreGive(m) != pdTRUE) return -1; else r = VFS_SUCCESS; } while(0)

#ifdef __cplusplus
}
#endif

#endif
