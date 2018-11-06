/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>

#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>
#include <vfs_machine.h>

extern aos_mutex_t g_vfs_mutex;

int aos_register_driver(const char *path, file_ops_t *ops, void *arg)
{
    inode_t *node = NULL;
    int err = 0;
		int ret;

    aos_mutex_lock(g_vfs_mutex, AOS_WAIT_FOREVER, err);
    if (err != 0) {
        return err;
    }

    ret = inode_reserve(path, &node);
    if (ret == VFS_SUCCESS) {
        /* now populate it with char specific information */
        INODE_SET_CHAR(node);

        node->ops.i_ops = ops;
        node->i_arg     = arg;

        /* creat device lock. */
        aos_mutex_new(node->mutex, ret);
    }

    /* step out critical area for type is allocated */
    aos_mutex_unlock(g_vfs_mutex, err);
    if (err != 0) {
        if (node->i_name != NULL) {
            aos_free(node->i_name);
        }

        memset(node, 0, sizeof(inode_t));
        return err;
    }

    return ret;
}

int aos_unregister_driver(const char *path)
{
    int err = 0;
	int ret;

    aos_mutex_lock(g_vfs_mutex, AOS_WAIT_FOREVER, err);
    if (err != 0) {
        return err;
    }

    ret = inode_release(path);

    aos_mutex_unlock(g_vfs_mutex, err);
    if (err != 0) {
        return err;
    }

    return ret;
}
