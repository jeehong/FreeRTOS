#include <vfs_conf.h>
#include <vfs_err.h>
#include <vfs_inode.h>
#include <stdio.h>

#define MAX_FILE_NUM (VFS_CONFIG_DEV_NODES * 2)
static file_t files[MAX_FILE_NUM];

file_t *new_file(inode_t *node)
{
    file_t *f;
    int idx;

    for (idx = 0; idx < MAX_FILE_NUM; idx++) {
        f = &files[idx];

        if (f->node == NULL) {
            goto got_file;
        }
    }

    return NULL;

got_file:
    f->node = node;
    f->f_arg = NULL;
    f->offset = 0;
    inode_ref(node);
    return f;
}

void del_file(file_t *file)
{
    inode_unref(file->node);
    file->node = NULL;
}

int get_fd(file_t *file)
{
    return (file - files) + VFS_CONFIG_FD_OFFSET;
}

file_t *get_file(int fd)
{
    file_t *f;

    fd -= VFS_CONFIG_FD_OFFSET;

    if (fd < 0) {
        return NULL;
    }

    if (fd >= MAX_FILE_NUM) {
        return NULL;
    }

    f = &files[fd];
    return f->node ? f : NULL;
}
