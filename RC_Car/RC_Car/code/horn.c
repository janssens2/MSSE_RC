/*
 * horn.c
 *
 * Created: 4/25/2015 8:49:13 PM
 *  Author: Mike Weispfenning
 */

#include <stdint.h>

#include "../headers/horn.h"
#include "../headers/serial_USB.h"

#include <pololu/orangutan.h>

const char la_cucaracha_short[] PROGMEM = "! T280 L4 V12 MS G8 G8 G8 >C4 >E8 G8 G8 G8 >C4 >E2 >C8 >C8 B8 B8 A8 A8 G4.";
const char la_cucaracha_long[] PROGMEM = "! T280 L4 V12 MS G8 G8 G8 >C4 >E8 G8 G8 G8 >C4 >E2 >C8 >C8 B8 B8 A8 A8 G4. G8 G8 G8 B4 >D8 G8 G8 G8 B4 >D2 >G8 >A8 >G8 >F8 >E8 >D8 >C2.";
const char la_cucaracha_full[] PROGMEM = "! T280 L4 V12 MS G8 G8 G8 >C4 >E8 G8 G8 G8 >C4 >E2 >C8 >C8 B8 B8 A8 A8 G4. G8 G8 G8 B4 >D8 G8 G8 G8 B4 >D2 >G8 >A8 >G8 >F8 >E8 >D8 >C2. G8 G8 >C8 >C8 >E8 >E8 >G4 >E2 >G8 >A8 >G8 >F8 >E8 >G8 >F4 >D2 G8 G8 B8 B8 >D8 >D8 >F4 >D2 >G8 >A8 >G8 >F8 >E8 >D8 >E4 >C2";

void horn_honk(uint8_t version)
{
	if (is_playing())
	{
		debug_print( DEBUG_IINFO, "Horn already playing, don't interrupt the magic!" );
		return;
	}

	// La Cucaracha!!!
	if (version == HORN_VERSION_SHORT)
	{
		play_from_program_space(la_cucaracha_short);
	}
	else if (version == HORN_VERSION_LONG)
	{
		play_from_program_space(la_cucaracha_long);
	}
	else if (version == HORN_VERSION_FULL)
	{
		play_from_program_space(la_cucaracha_full);
	}
}
