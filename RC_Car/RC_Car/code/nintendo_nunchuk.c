/*
 * nintendo_nunchuk.c
 *
 * Created: 4/3/2015 10:28:45 AM
 * Author: Mike Weispfenning
 */

#include <stdbool.h>

#include <pololu/orangutan.h>
#include <compat/twi.h>

#include "../headers/nintendo_nunchuk.h"
#include "../headers/two_wire_interface.h"

#define NUNCHUCK_ADDRESS 0x52

bool is_initialized = false;
uint8_t nunchuck_data[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void nunchuck_initialize()
{
	twi_start((NUNCHUCK_ADDRESS << 1) | TW_WRITE);
	twi_write(0x40);
	twi_write(0x00);
	twi_stop();

	is_initialized = true;
}

void nunchuck_check_is_initialized()
{
	if (!is_initialized)
	{
		nunchuck_initialize();
	}
}

void nunchuck_conversion_command()
{
	nunchuck_check_is_initialized();

	twi_start((NUNCHUCK_ADDRESS << 1) | TW_WRITE);
	twi_write(0x00);
	twi_stop();

	// This delay was added due to testing.
	// No delay causes a read error on the
	// next communication with the nunchuck
	delay_ms(1);
}

uint8_t nunchuck_decode_byte(uint8_t data)
{
	return (data ^ 0x17) + 0x17;
}

void nunchuck_refresh_data()
{
	nunchuck_check_is_initialized();
	nunchuck_conversion_command();

	uint8_t data_count = 6;

	twi_start((NUNCHUCK_ADDRESS << 1) | TW_READ);
	for (uint8_t i = 0; i < data_count; i++)
	{
		bool is_last_byte = (i == (data_count - 1));
		uint8_t data = is_last_byte ? twi_read_with_nak() : twi_read_with_ack();
		nunchuck_data[i] = nunchuck_decode_byte(data);
	}
	twi_stop();
}

uint8_t nunchuck_get_joystick_x()
{
	return nunchuck_data[0];
}

uint8_t nunchuck_get_joystick_y()
{
	return nunchuck_data[1];
}

uint16_t nunchuck_get_accelerometer_x()
{
	return (nunchuck_data[2] << 2) | ((nunchuck_data[5] | 0x0C) >> 2);
}

uint16_t nunchuck_get_accelerometer_y()
{
	return (nunchuck_data[3] << 2) | ((nunchuck_data[5] | 0x30) >> 4);
}

uint16_t nunchuck_get_accelerometer_z()
{
	return (nunchuck_data[4] << 2) | ((nunchuck_data[5] | 0xC0) >> 6);
}

bool nunchuck_get_button_c()
{
	return (nunchuck_data[5] & 0x02) == 0;
}

bool nunchuck_get_button_z()
{
	return (nunchuck_data[5] & 0x01) == 0;
}
