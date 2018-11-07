#ifndef __VFS_H__
#define __VFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

int vfs_init(void);

int vfs_open(const char *path, int flags);

ssize_t vfs_read(int fd, void *buf, size_t nbytes);

ssize_t vfs_write(int fd, const void *buf, size_t nbytes);

int vfs_close(int fd);

#ifdef __cplusplus
}
#endif

#endif
