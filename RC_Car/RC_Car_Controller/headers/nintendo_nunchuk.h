/*
 * nintendo_nunchuk.h
 *
 * Created: 4/3/2015 10:26:43 AM
 * Author: Mike Weispfenning
 */

#ifndef NINTENDO_NUNCHUK_H_
#define NINTENDO_NUNCHUK_H_

#include <stdbool.h>
#include <stdint.h>

void nunchuck_refresh_data();
uint8_t nunchuck_get_joystick_x();
uint8_t nunchuck_get_joystick_y();
uint16_t nunchuck_get_accelerometer_x();
uint16_t nunchuck_get_accelerometer_y();
uint16_t nunchuck_get_accelerometer_z();
bool nunchuck_get_button_c();
bool nunchuck_get_button_z();

#endif /* NINTENDO_NUNCHUK_H_ */