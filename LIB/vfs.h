/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_VFS_H
#define AOS_VFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include <vfs_conf.h>

int vfs_init(void);

int vfs_device_init(void);

int aos_open(const char *path, int flags);

ssize_t aos_read(int fd, void *buf, size_t nbytes);

ssize_t aos_write(int fd, const void *buf, size_t nbytes);

int aos_close(int fd);

#ifdef __cplusplus
}
#endif

#endif
