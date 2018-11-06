#ifndef __HAL_CLI__
#define __HAL_CLI__

typedef unsigned short (*data_switch)(char *, unsigned int);

void hal_cli_data_switch_register(data_switch tx, data_switch rx);
unsigned short hal_cli_data_tx(char *data, unsigned int len);
unsigned short hal_cli_data_rx(char *data, unsigned int len);

#endif
