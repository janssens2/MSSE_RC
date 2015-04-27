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

serialMessage *g_serialMessage;
serialCommand *g_serialCommand;

char serial_receive_buffer[80];
char serial_my_receive_buffer[80];
uint8_t serial_receive_buffer_position = 0;
uint8_t serial_my_receive_buffer_position = 0;

void init_serial_rx( ) {
	serial_set_baud_rate(UART0, SERIAL_BAUD_RATE);
	serial_set_mode(UART0, SERIAL_CHECK);
	serial_receive_ring(UART0, serial_receive_buffer, sizeof(serial_receive_buffer));
	
	g_serialCommand = malloc(sizeof(serialCommand));
	g_serialMessage = (serialMessage *)malloc(sizeof(serialMessage));
}

void getSerialCommand( serialCommand **ptr )
{
	*ptr = g_serialCommand;
}

void serial_send_command(serialCommand *cmd) {
	
	commandToSerialMessage(cmd, g_serialMessage);
	
	serial_send(UART0, (char *)g_serialMessage, sizeof(serialMessage));
	
	while(!serial_send_buffer_empty(UART0)) {
		serial_check();
	}
}

void serial_receive_bytes( )
{
	while( serial_get_received_bytes(UART0) != serial_receive_buffer_position )
	{
		serial_my_receive_buffer[serial_my_receive_buffer_position] = serial_receive_buffer[serial_receive_buffer_position];
		serial_my_receive_buffer_position++;
		
		if ( serial_my_receive_buffer_position && (serial_my_receive_buffer[serial_my_receive_buffer_position-1] == '\n' || serial_my_receive_buffer[serial_my_receive_buffer_position] == '\r') )
		{
			debug_print( DEBUG_IINFO, "received buffer size(%d)", serial_my_receive_buffer_position );
			debug_print( DEBUG_IINFO, "data: '%s'", serial_my_receive_buffer );
			
			serial_receive_command( serial_my_receive_buffer );
			
			memset( serial_my_receive_buffer, 0, sizeof( serial_my_receive_buffer ) );
			serial_my_receive_buffer_position = 0;
		}
		
		if (serial_receive_buffer_position == sizeof(serial_receive_buffer)-1)
		{
			serial_receive_buffer_position = 0;
		}
		else
		{
			serial_receive_buffer_position++;
		}
	}
}

void serial_receive_command( char commandBuffer[] )
{
	//serialCommand myCmd;
		
	//sscanf( commandBuffer, "S %d %d E", &myCmd.x, &myCmd.y );
	sscanf( commandBuffer, "S %d %d E", &g_serialCommand->x, &g_serialCommand->y );

	//memcpy( g_serialCommand, &myCmd, sizeof(serialCommand) );
	
	debug_print( DEBUG_IINFO, "serial received: x:%d y:%d", g_serialCommand->x, g_serialCommand->y );
}
