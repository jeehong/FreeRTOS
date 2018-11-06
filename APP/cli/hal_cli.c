#include "hal_cli.h"

static data_switch tx_data;
static data_switch rx_data;

void hal_cli_data_switch_register(data_switch tx, data_switch rx)
{
	tx_data = tx;
	rx_data = rx;
}

unsigned short hal_cli_data_tx(char *data, unsigned int len)
{
	return tx_data(data, len);
}

unsigned short hal_cli_data_rx(char *data, unsigned int len)
{
	return rx_data(data, len);
}

