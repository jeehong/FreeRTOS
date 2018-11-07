#ifndef __VFS_FILE_H__
#define __VFS_FILE_H__

#ifdef __cplusplus
extern "C" {
#endif

int get_fd(file_t *file);

file_t *get_file(int fd);

file_t *new_file(inode_t *node);

void del_file(file_t *file);

#ifdef __cplusplus
}
#endif

#endif
