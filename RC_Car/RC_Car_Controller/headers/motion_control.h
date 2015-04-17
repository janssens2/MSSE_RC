/*
 * motion_control.h
 *
 * Created: 4/13/2015 8:05:44 PM
 *  Author: Mike Weispfenning
 */

#ifndef MOTION_CONTROL_H_
#define MOTION_CONTROL_H_

#define MOTION_CONTROL_JOYSTICK 0x00
#define MOTION_CONTROL_ACCELEROMETER 0x01

#include <stdint.h>

uint8_t motion_control_get_input_method();
int16_t motion_control_get_x_input();
int16_t motion_control_get_y_input();
int8_t motion_control_get_x_input_as_percentage();
int8_t motion_control_get_y_input_as_percentage();

#endif /* MOTION_CONTROL_H_ */