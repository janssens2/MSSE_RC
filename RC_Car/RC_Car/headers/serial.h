/*
 * serial.h
 *
 * Created: 4/16/2015 9:56:08 PM
 *  Author: janssens
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_

#define BUFFER_SIZE 81
#define SERIAL_SEND_TIMEOUT 100

void wait_for_sending_to_finish();
void print_menu( );
void process_received_bytes( char bytes[] );
void strip( char bytes[], size_t mySize );
void check_for_new_bytes_received( );
void serial_print_string( const char myString[] );
void serial_print( char *format, ... );
void serial_print_char( const char myChar );
void serial_init( );

#endif /* SERIAL_H_ */