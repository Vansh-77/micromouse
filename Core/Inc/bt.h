#ifndef BT_H
#define BT_H

#include<stdint.h>

void bt_write_char(char c);
void bt_write(const char *str);
void bt_printf(const char *format, ...);

void bt_init(void);

uint8_t bt_message_available(void);
void bt_get_message(char *buffer);


#endif
