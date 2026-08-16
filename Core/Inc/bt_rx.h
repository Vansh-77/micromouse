#ifndef BT_RX_H
#define BT_RX_H

#include <stdint.h>

void bt_rx_init(void);

uint8_t bt_rx_available(void);
uint8_t bt_rx_read(uint8_t *data);

#endif
