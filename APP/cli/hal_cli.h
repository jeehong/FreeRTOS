#ifndef __HAL_CLI__
#define __HAL_CLI__

typedef unsigned short (*data_switch)(signed char *, unsigned short);

void hal_cli_data_switch_register(data_switch tx, data_switch rx);
unsigned short hal_cli_data_tx(signed char *data, unsigned short len);
unsigned short hal_cli_data_rx(signed char *data, unsigned short len);

#endif
