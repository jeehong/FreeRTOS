#ifndef __I2C_BUS_H__
#define	__I2C_BUS_H__

#include "stm32f2xx.h"

typedef struct {
	unsigned int rcc_scl;
	unsigned int rcc_sda;
	GPIO_TypeDef* group_scl;
	GPIO_TypeDef* group_sda;
	unsigned short pin_scl;
	unsigned short pin_sda;
	GPIOSpeed_TypeDef speed;
} I2C_RESOURCE_t;

typedef enum  {
	input = 0,
	output = 1,
} GPIO_DIR_e;

typedef enum {
	chip_rx8025 = 0,
	chip_all,
} CHIP_LIST_e;

void i2c_bus_init(void);
unsigned char i2c_bus_write_byte(CHIP_LIST_e chip, unsigned char value);
unsigned char i2c_bus_read_byte(CHIP_LIST_e chip, unsigned char ack);
void i2c_bus_write_rx8025(CHIP_LIST_e chip, unsigned char addr, unsigned char reg, const unsigned char *pdata, unsigned char len);
void i2c_bus_read_rx8025(CHIP_LIST_e chip, unsigned char addr, unsigned char reg, unsigned char *pdata, unsigned char len);

#endif

