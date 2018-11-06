/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_inode.h>
#include <vfs.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <vfs_file.h>
#include <vfs_machine.h>

#ifdef IO_NEED_TRAP
#include <vfs_trap.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 256
#endif

static uint8_t g_vfs_init;
aos_mutex_t g_vfs_mutex;

int vfs_init(void)
{
    int ret = VFS_SUCCESS;

    if (g_vfs_init == 1) {
        return ret;
    }
	aos_mutex_new(g_vfs_mutex, ret);
    if (ret != VFS_SUCCESS) {
        return ret;
    }

    inode_init();

    g_vfs_init = 1;

    return ret;
}

int aos_open(const char *path, int flags)
{
    file_t  *file;
    inode_t *node;
    size_t len = 0;
    int ret = VFS_SUCCESS;

    if (path == NULL) {
        return -EINVAL;
    }

    len = strlen(path);
    if (len > PATH_MAX) {
        return -ENAMETOOLONG;
    }
	aos_mutex_lock(g_vfs_mutex, AOS_WAIT_FOREVER, ret);
    if (ret != VFS_SUCCESS) {
        return ret;
    }

    node = inode_open(path);

    if (node == NULL) {
        aos_mutex_unlock(g_vfs_mutex, ret);

        #ifdef IO_NEED_TRAP
            return trap_open(path, flags);
        #else
            return -ENOENT;
        #endif
    }

    node->i_flags = flags;
    file = new_file(node);

    aos_mutex_unlock(g_vfs_mutex, ret);

    if (file == NULL) {
        return -ENFILE;
    }

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->open) != NULL) {
            ret = (node->ops.i_fops->open)(file, path, flags);
        }

    } else {
        if ((node->ops.i_ops->open) != NULL) {
            ret = (node->ops.i_ops->open)(node, file);
        }
    }

    if (ret != VFS_SUCCESS) {
        del_file(file);
        return ret;
    }

    return get_fd(file);
}

int aos_close(int fd)
{
    int ret = VFS_SUCCESS;
    file_t  *f;
    inode_t *node;

    f = get_file(fd);

    if (f == NULL) {
        #ifdef IO_NEED_TRAP
            return trap_close(fd);
        #else
            return -ENOENT;
        #endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->close) != NULL) {
            ret = (node->ops.i_fops->close)(f);
        }

    } else {

        if ((node->ops.i_ops->close) != NULL) {
            ret = (node->ops.i_ops->close)(f);
        }
    }

	aos_mutex_lock(g_vfs_mutex, AOS_WAIT_FOREVER, ret);
    if (ret != VFS_SUCCESS) {
        return ret;
    }

    del_file(f);

    aos_mutex_unlock(g_vfs_mutex, ret);

    return ret;
}

ssize_t aos_read(int fd, void *buf, size_t nbytes)
{
    ssize_t  nread = -1;
    file_t  *f;
    inode_t *node;

    f = get_file(fd);

    if (f == NULL) {
        #ifdef IO_NEED_TRAP
            return trap_read(fd, buf, nbytes);
        #else
            return -ENOENT;
        #endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->read) != NULL) {
            nread = (node->ops.i_fops->read)(f, buf, nbytes);
        }
    } else {
        if ((node->ops.i_ops->read) != NULL) {
            nread = (node->ops.i_ops->read)(f, buf, nbytes);
        }
    }

    return nread;
}

ssize_t aos_write(int fd, const void *buf, size_t nbytes)
{
    ssize_t  nwrite = -1;
    file_t  *f;
    inode_t *node;

    f = get_file(fd);

    if (f == NULL) {
        #ifdef IO_NEED_TRAP
            return trap_write(fd, buf, nbytes);
        #else
            return -ENOENT;
        #endif
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->write) != NULL) {
            nwrite = (node->ops.i_fops->write)(f, (const char *)buf, nbytes);
        }
    } else {
        if ((node->ops.i_ops->write) != NULL) {
            nwrite = (node->ops.i_ops->write)(f, (const char *)buf, nbytes);
        }
    }

    return nwrite;
}

int aos_ioctl(int fd, int cmd, unsigned long arg)
{
    int ret = -ENOSYS;
    file_t  *f;
    inode_t *node;

    if (fd < 0) {
        return -EINVAL;
    }

    f = get_file(fd);

    if (f == NULL) {
        return -ENOENT;
    }

    node = f->node;

    if (INODE_IS_FS(node)) {
        if ((node->ops.i_fops->ioctl) != NULL) {
            ret = (node->ops.i_fops->ioctl)(f, cmd, arg);
        }
    } else {
        if ((node->ops.i_ops->ioctl) != NULL) {
            ret = (node->ops.i_ops->ioctl)(f, cmd, arg);
        }
    }

    return ret;
}
