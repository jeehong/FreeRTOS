#ifndef __VFS_CONFIG_H__
#define __VFS_CONFIG_H__

#include "stdlib.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#ifdef __cplusplus
extern "C" {
#endif
#define VFS_FALSE    0u
#define VFS_TRUE     1u

#define VFS_CONFIG_DEV_NODES    25

#define VFS_CONFIG_POLL_SUPPORT 1
#define VFS_CONFIG_FD_OFFSET    64

#define VFS_CONFIG_WAIT_FOREVER 0

/* 
 * the max length(byte) of the file path
 * make sure that the max length more than 2 bytes
 */
#define VFS_CONFIG_FILE_NAME_MAX_CHARS	256

/* the mutex interface associated with the operating system(current:FreeRTOS) */
#define vfs_port_mutex_t QueueHandle_t
#define vfs_port_mutex_new(m, r) do {m = xSemaphoreCreateMutex(); if(m == NULL) r = -1; else r = VFS_SUCCESS; }while(0)
#define vfs_port_mutex_lock(m, arg, r) do {if(xSemaphoreTake(m, portMAX_DELAY) != pdTRUE) return r = -1; else r = VFS_SUCCESS; } while(0)
#define vfs_port_mutex_unlock(m, r) do {if(xSemaphoreGive(m) != pdTRUE) return -1; else r = VFS_SUCCESS; } while(0)


#ifdef __cplusplus
}
#endif

#endif

