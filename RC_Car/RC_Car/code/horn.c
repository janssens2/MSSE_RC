/*
 * horn.c
 *
 * Created: 4/25/2015 8:49:13 PM
 *  Author: Mike Weispfenning
 */

#include <stdint.h>

#include "../headers/horn.h"

#include <pololu/orangutan.h>

void horn_honk(uint8_t version)
{
	// La Cucaracha!!!
	if (version == HORN_VERSION_SHORT)
	{
		play("! T280 L4 V12 MS G8 G8 G8 >C4 >E8 G8 G8 G8 >C4 >E2 >C8 >C8 B8 B8 A8 A8 G4.");
	}
	else if (version == HORN_VERSION_LONG)
	{
		play("! T280 L4 V12 MS G8 G8 G8 >C4 >E8 G8 G8 G8 >C4 >E2 >C8 >C8 B8 B8 A8 A8 G4. G8 G8 G8 B4 >D8 G8 G8 G8 B4 >D2 >G8 >A8 >G8 >F8 >E8 >D8 >C2.");
	}
}