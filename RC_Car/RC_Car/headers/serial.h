/*
 * serial.h
 *
 * Created: 4/23/2015 8:20:25 PM
 *  Author: Jesse
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_

#define SERIAL_START_CHAR 'S'
#define SERIAL_END_CHAR 'E'
#define SERIAL_DELIMITER_CHAR ' '

#define SERIAL_BAUD_RATE 38400

typedef struct {
	char start_char;
	char delim_0;
	char x_val[5];
	char delim_1;
	char y_val[5];
	char delim_2;
	char end_char;
	char newline;
	char linefeed;
} serialMessage;

typedef struct {
	int16_t x;
	int16_t y;
	int16_t c;
	int16_t z;
} serialCommand;

void init_serial_rx( );

void serial_receive_bytes( );

void serial_receive_command( );

void getSerialCommand( serialCommand **ptr );

#endif /* SERIAL_H_ */