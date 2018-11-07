#ifndef __VFS_DRIVER_H__
#define __VFS_DRIVER_H__

#include <vfs_inode.h>

#ifdef __cplusplus
extern "C" {
#endif

int aos_register_driver(const char *path, file_ops_t *fops, void *arg);
int aos_unregister_driver(const char *path);


#ifdef __cplusplus
}
#endif

#endif    /*__VFS_DRIVER_H__*/

