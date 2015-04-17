/*
 * two_wire_interface.h
 *
 * Created: 4/3/2015 10:23:49 AM
 * Author: Mike Weispfenning
 */

#ifndef TWO_WIRE_INTERFACE_H_
#define TWO_WIRE_INTERFACE_H_

void twi_start(uint8_t address);
void twi_stop();
void twi_write(uint8_t data);
uint8_t twi_read_with_ack();
uint8_t twi_read_with_nak();

#endif /* TWO_WIRE_INTERFACE_H_ */
