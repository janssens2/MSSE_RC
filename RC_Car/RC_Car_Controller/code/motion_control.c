/*
 * motion_control.c
 *
 * Created: 4/13/2015 8:06:21 PM
 *  Author: Mike Weispfenning
 */

#define MAX_JOYSTICK_VALUE 0xFF
#define MAX_ACCELEROMETER_VALUE 0x3FF

#include <stdbool.h>
#include <stdint.h>

#include "../headers/motion_control.h"
#include "../headers/nintendo_nunchuk.h"
#include "../headers/timers.h"

bool g_is_motion_control_initialized = false;
uint8_t g_motion_control_input_method = MOTION_CONTROL_JOYSTICK;
int16_t g_home_joystick_x = 0x00;
int16_t g_home_joystick_y = 0x00;
int16_t g_home_accelerometer_x = 0x00;
int16_t g_home_accelerometer_y = 0x00;
bool g_last_c_button_value = false;
bool g_last_z_button_value = false;

void motion_control_home_inputs();
void motion_control_refresh_data();

void motion_control_initialize()
{
	g_is_motion_control_initialized = true;

	motion_control_refresh_data();
	motion_control_home_inputs();

	timer_one_set_to_ten_milliseconds(&motion_control_refresh_data);
}

void motion_control_home_inputs()
{
	g_home_joystick_x = nunchuck_get_joystick_x();
	g_home_joystick_y = nunchuck_get_joystick_y();
	g_home_accelerometer_x = nunchuck_get_accelerometer_x();
	g_home_accelerometer_y = nunchuck_get_accelerometer_y();
}

void motion_control_verify_is_initialized()
{
	if (!g_is_motion_control_initialized)
	{
		motion_control_initialize();
		g_is_motion_control_initialized = true;
	}
}

uint8_t motion_control_get_input_method()
{
	return g_motion_control_input_method;
}

void motion_control_refresh_data()
{
	nunchuck_refresh_data();

	bool c_button_value = nunchuck_get_button_c();
	if (c_button_value && !g_last_c_button_value)
	{
		// Change the input method from joystick to accelerometer, or vice versa
		g_motion_control_input_method = g_motion_control_input_method ^ 0x01;
	}
	g_last_c_button_value = c_button_value;

	bool z_button_value = nunchuck_get_button_z();
	if (z_button_value && !g_last_z_button_value)
	{
		motion_control_home_inputs();
	}
	g_last_z_button_value = z_button_value;
}

int16_t motion_control_get_x_input()
{
	motion_control_verify_is_initialized();

	int16_t x = 0x00;

	if (g_motion_control_input_method == MOTION_CONTROL_JOYSTICK)
	{
		x = nunchuck_get_joystick_x() - g_home_joystick_x;
	}
	else if (g_motion_control_input_method == MOTION_CONTROL_ACCELEROMETER)
	{
		x = nunchuck_get_accelerometer_x() - g_home_accelerometer_x;
	}

	return x;
}

int16_t motion_control_get_y_input()
{
	motion_control_verify_is_initialized();

	int16_t y = 0x00;

	if (g_motion_control_input_method == MOTION_CONTROL_JOYSTICK)
	{
		y = nunchuck_get_joystick_y() - g_home_joystick_y;
	}
	else if (g_motion_control_input_method == MOTION_CONTROL_ACCELEROMETER)
	{
		y = nunchuck_get_accelerometer_y() - g_home_accelerometer_y;
	}

	return y;
}

int8_t motion_control_get_x_input_as_percentage()
{
	int32_t x = motion_control_get_x_input();

	if (g_motion_control_input_method == MOTION_CONTROL_JOYSTICK)
	{
		if (x > 0)
		{
			x = (x * 100) / (MAX_JOYSTICK_VALUE - g_home_joystick_x);
		}
		else
		{
			x = (x * 100) / g_home_joystick_x;
		}
	}
	else if (g_motion_control_input_method == MOTION_CONTROL_ACCELEROMETER)
	{
		if (x > 0)
		{
			x = (x * 100) / (MAX_ACCELEROMETER_VALUE - g_home_accelerometer_x);
		}
		else
		{
			x = (x * 100) / g_home_accelerometer_x;
		}
	}

	return x;
}

int8_t motion_control_get_y_input_as_percentage()
{
	int32_t y = motion_control_get_y_input();

	if (g_motion_control_input_method == MOTION_CONTROL_JOYSTICK)
	{
		if (y > 0)
		{
			y = (y * 100) / (MAX_JOYSTICK_VALUE - g_home_joystick_y);
		}
		else
		{
			y = (y * 100) / g_home_joystick_y;
		}
	}
	else if (g_motion_control_input_method == MOTION_CONTROL_ACCELEROMETER)
	{
		if (y > 0)
		{
			y = (y * 100) / (MAX_ACCELEROMETER_VALUE - g_home_accelerometer_y);
		}
		else
		{
			y = (y * 100) / g_home_accelerometer_y;
		}
	}

	return y;
}
