#include "vfs_err.h"
#include "types.h"
#include "vfs_i2c.h"

#include "serial.h"
#include "i2c_bus.h"

#include "stm32f2xx_i2c.h"
#include "stm32f2xx.h"

#include <task.h>

/* i2c driver struct */
const static struct file_ops i2c_ops =
{
    vfs_i2c_open,	/* open */
    vfs_i2c_close,	/* close */
    vfs_i2c_read,	/* read */
    vfs_i2c_write,	/* write */
    NULL,			/* ioctl */
    /* NULL, */			/* poll */
};

const struct file_ops * vfs_i2c_opts(void)
{
	return &i2c_ops;
}

int vfs_i2c_open(inode_t *inode, file_t *fp)
{
    int ret = VFS_SUCCESS;              /* return value */
	
	i2c_bus_init();

	dbg_string("func:%s, line:%d\r\n", __func__, __LINE__);

    return ret;
}

int vfs_i2c_close(file_t *fp)
{
    int ret = VFS_SUCCESS;              /* return value */

	dbg_string("func:%s, line:%d\r\n", __func__, __LINE__);

    return ret;
}

ssize_t vfs_i2c_read(file_t *fp, void *buf, size_t nbytes)
{
    int ret = VFS_SUCCESS;              /* return value */
	uint8_t data[7] = {0};
	i2c_dev_t *i_arg = (i2c_dev_t *)fp->node->i_arg;

	i2c_bus_read_rx8025(chip_rx8025, i_arg->config.dev_addr, *(uint8_t *)buf, data, 7);
	dbg_string("func:%s, line:%d %x %x %x %x %x %x %x\r\n", __func__, __LINE__, 
		data[0], data[1], data[2], data[3], data[4], data[5], data[6]);

    return ret;
}

ssize_t vfs_i2c_write(file_t *fp, const void *buf, size_t nbytes)
{
    int ret = VFS_SUCCESS;              /* return value */
	const unsigned char data[7] = {1};
	i2c_dev_t *i_arg = (i2c_dev_t *)fp->node->i_arg;

	i2c_bus_write_rx8025(chip_rx8025, i_arg->config.dev_addr, 0, data, 7);
	dbg_string("func:%s, line:%d\r\n", __func__, __LINE__);

    return ret;
}
