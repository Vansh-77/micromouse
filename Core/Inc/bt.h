#ifndef BT_H
#define BT_H

#include<stdint.h>

void bt_write_char(char c);
void bt_write(const char *str);
void bt_printf(const char *format, ...);
uint8_t bt_read_char(char *c);
uint8_t bt_read(char *buffer, uint8_t max_len);


#endif
