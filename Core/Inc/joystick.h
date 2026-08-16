#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

void joystick_init(void);
void joystick_update(void);

uint8_t joystick_available(void);
uint8_t joystick_get_angle(void);
uint8_t joystick_get_radius(void);
void joystick_motor_control(void);

#endif
