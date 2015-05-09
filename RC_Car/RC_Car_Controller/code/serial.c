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
#include "../headers/motion_control.h"


serialMessage *g_serialMessage;
char tmpBuff[30];
 
void init_serial_tx(serialCommand **cmd) {
	serial_set_baud_rate(UART0, SERIAL_BAUD_RATE);
	serial_set_mode(UART0, SERIAL_CHECK);
	
	//TODO do we need to set up serial_receive_ring at all?
	*cmd = malloc(sizeof(serialCommand));
	g_serialMessage = (serialMessage *)calloc(1, sizeof(serialMessage));
}

void commandToSerialMessage(serialCommand *cmd, serialMessage *sm) {
	sm->start_char = SERIAL_START_CHAR;
	
	sm->delim_0 = SERIAL_DELIMITER_CHAR;
	sm->delim_1 = SERIAL_DELIMITER_CHAR;
	sm->delim_2 = SERIAL_DELIMITER_CHAR;
	sm->delim_3 = SERIAL_DELIMITER_CHAR;
	sm->delim_4 = SERIAL_DELIMITER_CHAR;
	sm->delim_5 = SERIAL_DELIMITER_CHAR;
	sm->delim_6 = SERIAL_DELIMITER_CHAR;
	
	sm->newline = '\n';
	sm->linefeed = '\r';
	
	sprintf(sm->x_val, "%+4d", cmd->x);
	sprintf(sm->y_val, "%+4d", cmd->y);
	sprintf(sm->c_val, "%1d", cmd->c);
	sprintf(sm->z_val, "%1d", cmd->z);
	
	sm->end_char = SERIAL_END_CHAR;
}

void serial_send_command(serialCommand *cmd) {
	
	commandToSerialMessage(cmd, g_serialMessage);
	
	memset( tmpBuff, 0, sizeof(tmpBuff));
	snprintf( tmpBuff, sizeof(tmpBuff), "%c %.d %.d %.d %.d %c\r\n", SERIAL_START_CHAR, cmd->x, cmd->y, cmd->c, cmd->z, SERIAL_END_CHAR );
	
	//serial_send(UART0, (char *)g_serialMessage, sizeof(serialMessage));
	serial_send(UART0, tmpBuff, sizeof(tmpBuff));
	
	while(!serial_send_buffer_empty(UART0)) {
		serial_check();
	}
}