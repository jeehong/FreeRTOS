#include "hal_cli.h"

static data_switch tx_data;
static data_switch rx_data;

void hal_cli_data_switch_register(data_switch tx, data_switch rx)
{
	tx_data = tx;
	rx_data = rx;
}

unsigned short hal_cli_data_tx(signed char *data, unsigned short len)
{
	return tx_data(data, len);
}

unsigned short hal_cli_data_rx(signed char *data, unsigned short len)
{
	return rx_data(data, len);
}

