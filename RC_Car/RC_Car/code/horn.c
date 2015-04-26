/*
 * horn.c
 *
 * Created: 4/25/2015 8:49:13 PM
 *  Author: Mike Weispfenning
 */

#include "../headers/horn.h"

#include <pololu/orangutan.h>

void horn_honk()
{
	// La Cucaracha!!!
	play("! T280 L4 V12 MS G8 G8 G8 >C4 >E4 G8 G8 G8 >C4 >E2 >C8 >C8 B8 B8 A8 A8 G4.");
}
