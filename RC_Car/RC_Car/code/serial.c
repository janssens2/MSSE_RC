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
unsigned char serial_receive_buffer_position = 0;
unsigned char serial_my_receive_buffer_postition = 0;

void init_serial_rx(serialCommand **cmd) {
	serial_set_baud_rate(UART0, SERIAL_BAUD_RATE);
	serial_set_mode(UART0, SERIAL_CHECK);
	serial_receive_ring(UART0, serial_receive_buffer, sizeof(serial_receive_buffer));
	
	*cmd = malloc(sizeof(serialCommand));
	g_serialMessage = (serialMessage *)malloc(sizeof(serialMessage));
	g_serialCommand = (serialCommand *)malloc( sizeof(serialCommand) );
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
		serial_my_received_buffer[serial_my_receive_buffer_position] = serial_receive_buffer[serial_receive_buffer_position];
		serial_my_receive_buffer_position++;
		
		if ( serial_my_receive_buffer_position && (serial_my_received_buffer[serial_my_receive_buffer_position-1] == '\r' || serial_my_received_buffer[serial_my_receive_buffer_position-1] == '\n') )
		{
			debug_print( DEBUG_IINFO, "received buffer size(%d)", serial_my_receive_buffer_position );
			debug_print( DEBUG_IINFO, "%s", serial_my_receive_buffer );
			
			serial_receive_command( serial_my_received_buffer );
			
			memset( serial_my_received_buffer, 0, sizeof( serial_my_received_buffer ) );
			serial_my_receive_buffer_postition = 0;
		}
		
		if (receive_buffer_position == sizeof(receive_buffer)-1)
		{
			receive_buffer_position = 0;
		}
		else
		{
			receive_buffer_position++;
		}
	}
}

void serial_receive_command( char commandBuffer[] )
{
	serialCommand myCmd;
	
	sscanf( commandBuffer, "S %d %d E", myCmd.x, myCmd.y );

	memcpy( g_serialCommand, &myCmd, sizeof(serialCommand) );
}
