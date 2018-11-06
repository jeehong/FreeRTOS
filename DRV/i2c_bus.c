#include "FreeRTOS.h"
#include "task.h"

#include "i2c_bus.h"
#include "stm32f2xx.h"

/*
 * RX8025 hardware resource
 * SCL:PB6
 * SDA:PB7
 */
#define	I2C_RX8025_RCC			RCC_AHB1Periph_GPIOB
#define	I2C_RX8025_GROUP		GPIOB
#define	I2C_RX8025_SCL			GPIO_Pin_6
#define	I2C_RX8025_SDA			GPIO_Pin_7

#define	I2C_BUS_SDA_HIGH(chip)		(chip.group_sda->BSRRL = chip.pin_sda)
#define	I2C_BUS_SDA_LOW(chip)		(chip.group_sda->BSRRH = chip.pin_sda)
#define	I2C_BUS_SCL_HIGH(chip)		(chip.group_scl->BSRRL = chip.pin_scl)
#define	I2C_BUS_SCL_LOW(chip)		(chip.group_scl->BSRRH = chip.pin_scl)


static void delayus(unsigned short us);

static void i2c_bus_stop(CHIP_LIST_e chip);
static __inline void i2c_bus_send_ack(CHIP_LIST_e chip, unsigned char ack);
static __inline unsigned char i2c_bus_get_ack(CHIP_LIST_e chip);
static BitAction I2C_BUS_SDA_STATE(CHIP_LIST_e chip);

static I2C_RESOURCE_t i2c_bus[chip_all];

void i2c_bus_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	unsigned char index = 0;

	i2c_bus[chip_rx8025].rcc_scl = I2C_RX8025_RCC;
	i2c_bus[chip_rx8025].rcc_sda = I2C_RX8025_RCC;
	i2c_bus[chip_rx8025].group_scl = I2C_RX8025_GROUP;
	i2c_bus[chip_rx8025].group_sda = I2C_RX8025_GROUP;
	i2c_bus[chip_rx8025].pin_scl = I2C_RX8025_SCL;
	i2c_bus[chip_rx8025].pin_sda = I2C_RX8025_SDA;
	i2c_bus[chip_rx8025].speed = GPIO_Speed_50MHz;

	for(index = chip_rx8025; index < chip_all; index++)
	{
		RCC_APB2PeriphClockCmd(i2c_bus[index].rcc_scl | i2c_bus[index].rcc_sda, ENABLE);

		GPIO_InitStructure.GPIO_Pin = i2c_bus[index].pin_scl | i2c_bus[index].rcc_sda;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = i2c_bus[index].speed;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
		GPIO_Init(i2c_bus[index].group_scl, &GPIO_InitStructure);
		GPIO_ResetBits(i2c_bus[index].group_scl, i2c_bus[index].pin_scl);
	}
}

/*  */
static void i2c_bus_sda_dir(CHIP_LIST_e chip, GPIO_DIR_e dir)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = i2c_bus[chip].pin_sda;
	GPIO_InitStructure.GPIO_Speed = i2c_bus[chip].speed;

	if(dir == input)
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	}
	else
	{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	}
	GPIO_Init(i2c_bus[chip].group_sda, &GPIO_InitStructure);
}

static void delayus(unsigned short us) 
{
	us *= 20;
	while(us--) ;
}

/*
 *       ____________
 * SCL:      
 *       _____
 * SDA :      \______
 */
static __inline void i2c_bus_start(CHIP_LIST_e chip)
{
	i2c_bus_sda_dir(chip, output);

	I2C_BUS_SDA_HIGH(i2c_bus[chip]);
	I2C_BUS_SCL_HIGH(i2c_bus[chip]);
	delayus(1);
	I2C_BUS_SDA_LOW(i2c_bus[chip]);
	delayus(1);
}

/*
 *             _______
 * SCL:  _____/    
 *        ____________
 * SDA : /     
 */
static __inline void i2c_bus_stop(CHIP_LIST_e chip)
{
	i2c_bus_sda_dir(chip, output);

	I2C_BUS_SDA_LOW(i2c_bus[chip]);
	I2C_BUS_SCL_HIGH(i2c_bus[chip]);
	delayus(2);
	I2C_BUS_SDA_HIGH(i2c_bus[chip]);
	delayus(2);
}

static __inline void i2c_bus_send_ack(CHIP_LIST_e chip, unsigned char ack)
{
	i2c_bus_sda_dir(chip, output);
	if(ack)
		I2C_BUS_SDA_HIGH(i2c_bus[chip]);	
	else
		I2C_BUS_SDA_LOW(i2c_bus[chip]);	
	delayus(1);
	I2C_BUS_SCL_HIGH(i2c_bus[chip]);	
	delayus(2);	
	I2C_BUS_SCL_LOW(i2c_bus[chip]);
}

/*
 *            ____
 * SCL:  ____/    \_____
 *       __          ___
 * SDA :   \________/   
 */

static __inline unsigned char i2c_bus_get_ack(CHIP_LIST_e chip)
{
	unsigned char ack;

	i2c_bus_sda_dir(chip, input);
	delayus(1);
	I2C_BUS_SCL_HIGH(i2c_bus[chip]);
	delayus(1);
	ack = I2C_BUS_SDA_STATE(chip);
	I2C_BUS_SCL_LOW(i2c_bus[chip]);

	return ack;
}

//----------------------------------------------------------------------------------
unsigned char i2c_bus_write_byte(CHIP_LIST_e chip, unsigned char value)
{
	unsigned char index;
	unsigned char ack = 0;
	
	i2c_bus_sda_dir(chip, output);
	taskENTER_CRITICAL();
	for(index = 0x80; index > 0; index >>= 1)
	{ 
		if (index & value)
			I2C_BUS_SDA_HIGH(i2c_bus[chip]);
		else
			I2C_BUS_SDA_LOW(i2c_bus[chip]);
		
		delayus(2);
		I2C_BUS_SCL_HIGH(i2c_bus[chip]);
		delayus(4);
		I2C_BUS_SCL_LOW(i2c_bus[chip]);
		delayus(2);
	}
	ack = i2c_bus_get_ack(chip);			
	taskEXIT_CRITICAL();
	return ack;				
}

unsigned char i2c_bus_read_byte(CHIP_LIST_e chip, unsigned char ack)
{
	unsigned char index;
	unsigned char val = 0;
	
	i2c_bus_sda_dir(chip, input);
	taskENTER_CRITICAL();
	for(index = 0x80; index > 0; index >>= 1)
	{
		I2C_BUS_SCL_HIGH(i2c_bus[chip]);
		delayus(3);
		
		if(I2C_BUS_SDA_STATE(chip))
			val = (val | index);
		I2C_BUS_SCL_LOW(i2c_bus[chip]);
		delayus(2);
	}
	
	i2c_bus_send_ack(chip, ack);
	taskEXIT_CRITICAL();
	return val;
}

void i2c_bus_write_rx8025(CHIP_LIST_e chip, 
								unsigned char addr, 
								unsigned char reg, 
								const unsigned char *pdata, 
								unsigned char len)
{
	unsigned char index;

	if(len == 0)
		return;
	i2c_bus_start(chip);
	
	i2c_bus_write_byte(chip, addr);
	i2c_bus_write_byte(chip, reg);
	for(index = 0; index < len; index++)
	{
		i2c_bus_write_byte(chip, pdata[index]);
	}
	i2c_bus_stop(chip);
}

void i2c_bus_read_rx8025(CHIP_LIST_e chip, 
								unsigned char addr, 
								unsigned char reg, 
								unsigned char *pdata, 
								unsigned char len)
{
	unsigned char index;

	i2c_bus_start(chip);
	i2c_bus_read_byte(chip, addr | 0x01);
	i2c_bus_get_ack(chip);
	for(index = 0; index < len; index++)
	{
		pdata[index] = i2c_bus_read_byte(chip, (index + 1) == len ? 1 : 0);
	}
	i2c_bus_stop(chip);
}

static BitAction I2C_BUS_SDA_STATE(CHIP_LIST_e chip)
{
	return (((i2c_bus[chip].group_sda->IDR & i2c_bus[chip].pin_sda) != (u32)Bit_RESET) ? Bit_SET : Bit_RESET);
}

