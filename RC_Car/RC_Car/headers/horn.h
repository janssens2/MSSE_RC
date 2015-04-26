/*
 * horn.h
 *
 * Created: 4/25/2015 8:48:36 PM
 *  Author: Mike Weispfenning
 */

#ifndef HORN_H_
#define HORN_H_

#include <stdint.h>

#define HORN_VERSION_SHORT 0x00
#define HORN_VERSION_LONG 0x01
#define HORN_VERSION_FULL 0x02

void horn_honk(uint8_t version);

#endif /* HORN_H_ */