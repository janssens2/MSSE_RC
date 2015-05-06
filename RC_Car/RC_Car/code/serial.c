/*
 * serial.c
 *
 * Created: 4/23/2015 7:52:35 PM
 *  Author: Jesse
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pololu/orangutan.h>

#include "../headers/serial.h"
#include "../headers/serial_USB.h"

serialCommand *g_serialCommand;

char serial_receive_buffer[40] = {0x0};
char serial_my_receive_buffer[40] = {0x0};
uint8_t serial_receive_buffer_position = 0;
uint8_t serial_my_receive_buffer_position = 0;

void init_serial_rx( ) 
{
	serial_set_baud_rate(UART0, SERIAL_BAUD_RATE);
	serial_set_mode(UART0, SERIAL_CHECK);
	serial_receive_ring(UART0, serial_receive_buffer, sizeof(serial_receive_buffer));
	
	g_serialCommand = malloc(sizeof(serialCommand));
	
	memset( g_serialCommand, 0, sizeof(serialCommand) );
}

void getSerialCommand( serialCommand **ptr )
{
	*ptr = g_serialCommand;
}

void serial_receive_bytes( )
{
	while( serial_get_received_bytes(UART0) != serial_receive_buffer_position )
	{
		if ( serial_my_receive_buffer_position == sizeof( serial_my_receive_buffer ) - 1 )
		{
			memset( serial_my_receive_buffer, 0x0, sizeof( serial_my_receive_buffer ) );
			serial_my_receive_buffer_position = 0;
		}
		else if ( serial_my_receive_buffer_position == 0 && serial_receive_buffer[serial_receive_buffer_position] == 'S' )
		{
			serial_my_receive_buffer[serial_my_receive_buffer_position] = serial_receive_buffer[serial_receive_buffer_position];
			serial_my_receive_buffer_position++;
		}
		else if ( serial_receive_buffer[serial_receive_buffer_position] == 'E' )
		{
			serial_my_receive_buffer[serial_my_receive_buffer_position] = serial_receive_buffer[serial_receive_buffer_position];
			serial_my_receive_buffer_position++;
			serial_receive_command( );
			memset( serial_my_receive_buffer, 0x0, sizeof( serial_my_receive_buffer ) );
			serial_my_receive_buffer_position = 0;
		}
		else if ( serial_my_receive_buffer_position > 0 )
		{
			serial_my_receive_buffer[serial_my_receive_buffer_position] = serial_receive_buffer[serial_receive_buffer_position];
			serial_my_receive_buffer_position++;
		}
		
		if ( serial_receive_buffer_position == sizeof(serial_receive_buffer) - 1 )
		{
			serial_receive_buffer_position = 0;
		}
		else
		{
			serial_receive_buffer_position++;
		}
	}
}

void serial_receive_command( )
{
	int16_t x;
	int16_t y;
	int16_t c;
	int16_t z;
	int16_t myCnt = 0;

	myCnt = sscanf( serial_my_receive_buffer, "S %d %d %d %d E", &x, &y, &c, &z );

	if ( myCnt == 4 )
	{
		g_serialCommand->x = x;
		g_serialCommand->y = y * -1; // invert the y coord as - is forward because of motor position and + should be forward
		g_serialCommand->c = c;
		g_serialCommand->z = z;
	}
	
	debug_print( DEBUG_VERBOSE, "(%d) serial received: (%X) x:%d y:%d c:%d z:%d", myCnt, &g_serialCommand, g_serialCommand->x, g_serialCommand->y, g_serialCommand->c, g_serialCommand->z );
}