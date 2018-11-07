#include <string.h>

#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_register.h>

extern vfs_port_mutex_t g_vfs_mutex;

int aos_register_driver(const char *path, file_ops_t *ops, void *arg)
{
    inode_t *node = NULL;
    int err = 0;
		int ret;

    vfs_port_mutex_lock(g_vfs_mutex, VFS_CONFIG_WAIT_FOREVER, err);
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
        vfs_port_mutex_new(node->mutex, ret);
    }

    /* step out critical area for type is allocated */
    vfs_port_mutex_unlock(g_vfs_mutex, err);
    if (err != 0) {
		node->i_name[0] = '\0';

        memset(node, 0, sizeof(inode_t));
        return err;
    }

    return ret;
}

int aos_unregister_driver(const char *path)
{
    int err = 0;
	int ret;

    vfs_port_mutex_lock(g_vfs_mutex, VFS_CONFIG_WAIT_FOREVER, err);
    if (err != 0) {
        return err;
    }

    ret = inode_release(path);

    vfs_port_mutex_unlock(g_vfs_mutex, err);
    if (err != 0) {
        return err;
    }

    return ret;
}
