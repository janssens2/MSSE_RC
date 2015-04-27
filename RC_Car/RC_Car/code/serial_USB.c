/*
 * serial.c
 *
 * Created: 4/16/2015 9:56:19 PM
 *  Author: janssens
 */ 

#include <pololu/orangutan.h>
#include <stdio.h>
#include <string.h>

#include "../headers/serial_USB.h"
#include "../headers/pid.h"
#include "../headers/horn.h"

char myBuffer[BUFFER_SIZE];
char my_received_buffer[BUFFER_SIZE];
uint8_t bytes_received = 0;

// receive_buffer: A ring buffer that we will use to receive bytes on USB_COMM.
// The OrangutanSerial library will put received bytes in to
// the buffer starting at the beginning (receiveBuffer[0]).
// After the buffer has been filled, the library will automatically
// start over at the beginning.
char receive_buffer[BUFFER_SIZE];

// receive_buffer_position: This variable will keep track of which bytes in the receive buffer
// we have already processed.  It is the offset (0-31) of the next byte
// in the buffer to process.
unsigned char receive_buffer_position = 0;

// send_buffer: A buffer for sending bytes on USB_COMM.
char send_buffer[BUFFER_SIZE];

enum debug_level_t gDebugLevel = DEBUG_ERROR;

void wait_for_sending_to_finish()
{
	int32_t timeout = SERIAL_SEND_TIMEOUT;
	// need to have a timer here that can timeout
	// this helps in the case the serial port is not connected and
	// we could hang here until reset.
	while(!serial_send_buffer_empty(USB_COMM) && ( 0 < timeout-- ) )
	{
		serial_check();		// USB_COMM port is always in SERIAL_CHECK mode
	}
}

void print_menu( )
{
	serial_print( "Menu: <operation> [value]" );
	serial_print( "   ? : To view this help again" );
}

void process_received_bytes( char bytes[] )
{
	SPid *tmpPid;
	char operation;
	int32_t value = 0;
	
	// may need this -Wl,-u,vfscanf -lscanf_flt for float reads...
	sscanf( bytes, "%c %ld", &operation, &value);

	switch(operation)
	{
		//
		case '?':
			// print the menu again
			print_menu();
			break;
		case '0':
			gDebugLevel = DEBUG_ERROR;
			break;		
		case '1':
			gDebugLevel = DEBUG_WARN;
			break;
		case '2':
			gDebugLevel = DEBUG_IINFO;
			break;
		case '3':
			gDebugLevel = DEBUG_INFO;
			break;
		case '4':
			gDebugLevel = DEBUG_VERBOSE;
			break;
			
		case 'm':
			// modify motor 1 speed
			getMotorPid( &tmpPid, 1 );
			tmpPid->targetRef = value;
			break;

		case 'n':
			// modify motor 2 speed
			getMotorPid( &tmpPid, 2 );
			tmpPid->targetRef = value;
			break;
			
		case 'h':
			// use the horn
			switch( value )
			{
				case 1:
					horn_honk( HORN_VERSION_FULL );
					break;
				case 2:
					horn_honk( HORN_VERSION_LONG );
					break;
				case 3:
					horn_honk( HORN_VERSION_SHORT );
					break;
				default:
					
					break;
			}
			break;

		default:
			// something wrong ????
			serial_print( "Unknown Operation, try again!" );
			break;
	}
}

void strip( char bytes[], size_t mySize )
{
	uint32_t counter = 0;
	
	for( ; counter < mySize && bytes[counter] != '\0'; counter++ )
	{
		if ( bytes[counter] != 32 && bytes[counter] != 45 &&
		bytes[counter] != 63 &&
		(bytes[counter] < 48 || bytes[counter] > 57) &&
		(bytes[counter] < 65 || bytes[counter] > 122) )
		{
			bytes[counter] = 0x0;
		}
	}
}

void check_for_new_bytes_received( )
{
	while(serial_get_received_bytes(USB_COMM) != receive_buffer_position)
	{
		// Remember the byte that was received, ignore if user pushed too many bytes
		if ( bytes_received > sizeof(my_received_buffer) )
		{
			bytes_received = 0;
			memset( my_received_buffer, 0, sizeof(my_received_buffer) );
			serial_print( "Buffer Exceeded, try again!" );
		}
		else
		{
			if ( receive_buffer[receive_buffer_position] == 0x8 || receive_buffer[receive_buffer_position] == 0x7F )
			{
				// treat this as a backspace
				bytes_received = (bytes_received > 0) ? bytes_received - 1 : bytes_received;
				my_received_buffer[bytes_received] = '\0';
			}
			else
			{
				my_received_buffer[bytes_received] = receive_buffer[receive_buffer_position];
				bytes_received++;
			}
			
			// echo what was typed on the serial console back to the serial console
			serial_print_char( receive_buffer[receive_buffer_position] );
		}
		
		// Increment receive_buffer_position, but wrap around when it gets to
		// the end of the buffer.
		if (receive_buffer_position == sizeof(receive_buffer)-1)
		{
			receive_buffer_position = 0;
		}
		else
		{
			receive_buffer_position++;
		}
	}
	
	if ( bytes_received && (my_received_buffer[bytes_received-1] == '\r' || my_received_buffer[bytes_received-1] == '\n') )
	{
		// echo string to serial console
		serial_print( "%s", my_received_buffer );
		
		// check makes sure we have atleast 3 bytes of data from user interface
		if ( bytes_received > 1)
		{
			strip( my_received_buffer, sizeof(my_received_buffer) );
			process_received_bytes( my_received_buffer );
		}
		
		memset( my_received_buffer, 0, sizeof(my_received_buffer) );
		bytes_received = 0;
	}
}

void serial_print_string( const char myString[] )
{
	uint32_t parmSize = strlen(myString);
	parmSize = (BUFFER_SIZE - 2 > parmSize) ? parmSize+2 : BUFFER_SIZE;
	
	wait_for_sending_to_finish();
	memset( send_buffer, 0, BUFFER_SIZE );
	strncpy( send_buffer, myString, parmSize );
	send_buffer[parmSize-2] = '\r';
	send_buffer[parmSize-1] = '\n';
	serial_send( USB_COMM, send_buffer, BUFFER_SIZE );
	wait_for_sending_to_finish();
	
}

void debug_print( uint8_t dbgLvl, char *format, ... )
{
	if ( dbgLvl > gDebugLevel)
	{
		return;
	}
	
	va_list args;
	va_start( args, format );
	
	memset( myBuffer, 0, BUFFER_SIZE );
	vsnprintf( myBuffer, BUFFER_SIZE, format, args );
	
	serial_print_string( myBuffer );
}

void serial_print( char *format, ... )
{
	va_list args;
	va_start( args, format );
	
	memset( myBuffer, 0, BUFFER_SIZE );
	vsnprintf( myBuffer, BUFFER_SIZE, format, args );
	
	serial_print_string( myBuffer );
}

void serial_print_char( const char myChar )
{
	wait_for_sending_to_finish();
	memset( send_buffer, 0, sizeof(send_buffer) );
	send_buffer[0] = myChar;
	serial_send( USB_COMM, send_buffer, 1 );
}

void serial_init( )
{
	// Set the baud rate to 9600 bits per second.  Each byte takes ten bit
	// times, so you can get at most 960 bytes per second at this speed.
	serial_set_baud_rate(USB_COMM, 9600);

	// Start receiving bytes in the ring buffer.
	serial_receive_ring(USB_COMM, receive_buffer, sizeof(receive_buffer));
	
	memset( my_received_buffer, 0, sizeof(my_received_buffer));
	
	serial_print_string( "USB Serial Initialized" );
	serial_print_string( "" );
	print_menu();
}