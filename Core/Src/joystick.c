#include "joystick.h"
#include "bt_rx.h"
#include "motor.h"
#include "uart.h"
#include <math.h>


static uint8_t state = 0;

static uint8_t value = 0;

static volatile uint8_t angle = 0;
static volatile uint8_t radius = 0;
static volatile uint8_t data_ready = 0;


void joystick_init(void)
{
    state = 0;
    value = 0;

    angle = 0;
    radius = 0;

    data_ready = 0;
}


void joystick_update(void)
{
    uint8_t c;

    while (bt_rx_read(&c))
    {
        switch (state)
        {
            /*
             * FF
             */
            case 0:
                if (c == 0xFF)
                    state = 1;
                break;
            /*
             * 01
             */
            case 1:
                if (c == 0x01)
                    state = 2;
                else
                    state = 0;
                break;
            /*
             * 02
             */
            case 2:
                if (c == 0x02)
                    state = 3;
                else
                    state = 0;
                break;
            /*
             * 01
             */
            case 3:
                if (c == 0x01)
                    state = 4;
                else
                    state = 0;
                break;
            /*
             * 02
             */
            case 4:
                if (c == 0x02)
                    state = 5;
                else
                    state = 0;
                break;
            /*
             * 00
             */
            case 5:
                if (c == 0x00)
                    state = 6;
                else
                    state = 0;
                break;
            /*
             * VALUE
             */
            case 6:
                value = c;
                state = 7;
                break;
            /*
             * FINAL 00
             */
            case 7:
                if (c == 0x00)
                {
                    /*
                     * Upper 5 bits = angle
                     * Lower 3 bits = radius
                     */
                    angle = (value >> 3) & 0x1F;
                    radius = value & 0x07;
                    data_ready = 1;
                }
                state = 0;
                break;
        }
    }
}


uint8_t joystick_available(void)
{
    return data_ready;
}


uint8_t joystick_get_angle(void)
{
    return angle;
}


uint8_t joystick_get_radius(void)
{
    data_ready = 0;

    return radius;
}


void joystick_motor_control(void){

//	if(!joystick_available())return; // we dont need it for continuous motor control

	float theta = joystick_get_angle() * 15.0f * (3.14159265f / 180.0f);
	uint8_t radius = joystick_get_radius();
	if (radius > 6)radius = 6;
    float magnitude = radius / 6.0f;
// at theta 90 forward = 1 and turn = 0
    float forward = sinf(theta) * magnitude;
    float turn    = cosf(theta) * magnitude;

    float left  = forward + turn;
    float right = forward - turn;

    if (left > 1.0f)  left = 1.0f;
    if (left < -1.0f) left = -1.0f;

    if (right > 1.0f)  right = 1.0f;
    if (right < -1.0f) right = -1.0f;
    uart_printf("left= %d right= %d\r\n", (int16_t)(left * 100.0f),(int16_t)(right * 100.0f));
    motor_left((int16_t)(left * 100.0f));
    motor_right((int16_t)(right * 100.0f));


}
