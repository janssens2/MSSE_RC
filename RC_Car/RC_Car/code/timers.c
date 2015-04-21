/*
 * timers.c
 *
 * Created: 4/11/2015 11:00:25 AM
 *  Author: Mike Weispfenning
 */

#include <pololu/orangutan.h>
#include <stdlib.h>

#include "../headers/timers.h"

void (*g_timer_one_interrupt_handler)();
void (*g_timer_two_interrupt_handlerA)();
void (*g_timer_two_interrupt_handlerB)();
void (*g_timer_three_interrupt_handler)();

void timer_one_set_to_ten_milliseconds(void (*timer_one_interrupt_handler)())
{
	//cli();
	
	g_timer_one_interrupt_handler = timer_one_interrupt_handler;

	TCCR1A = 0x80;
	TCCR1B = 0x1B;
	TCCR1C = 0x00;

	// Set ICR1 to 3,125
	ICR1 = 0x0C35;

	// Enable output compare A match interrupt
	TIMSK1 = 0x02;

	//sei();
}

void timer_two_set_to_fast_pwm( void (*timer_two_interrupt_handlerA)(), void (*timer_two_interrupt_handlerB)() )
{
	//cli();
	
	// COM2A = 0b00 Set OC2A on Compare Match, clear OC2A at BOTTOM (inverting mode) (table 17-3)
	// COM2B = 0b00 Set OC2B on Compare Match, clear OC2B at BOTTOM (inverting mode) (table 17-6)
	// WGM   = 0b011 Set Fast PWM mode (table 17-8)
	// CS    = 0b010 Prescaler of 8 (table 17-9)
	TCCR2A = (0 << COM2A1) | (0 << COM2A0) | (0 << COM2B1) | (0 << COM2B0) | (0x0 << 2) | (1 << WGM21) | (1 << WGM20);
	TCCR2B = (0 << WGM22) | (0 << CS22) | (1 << CS21) | (0 << CS20);
	
	// Initialize all PWMs to 0% duty cycle (braking)
	OCR2A = OCR2B = 0x00;
	
	if ( timer_two_interrupt_handlerA != NULL )
	{
		// enable interrupts to happen if the user passes in a function to handle them
		g_timer_two_interrupt_handlerA = timer_two_interrupt_handlerA;
		TIMSK2 |= (1 << OCIE2A);
	}
	if ( timer_two_interrupt_handlerB != NULL )
	{
		// enable interrupts to happen if the user passes in a function to handle them
		g_timer_two_interrupt_handlerB = timer_two_interrupt_handlerB;
		TIMSK2 |= (1 << OCIE2B);
	}
	
	//sei();
}

void timer_three_set_to_one_hundred_milliseconds(void (*timer_three_interrupt_handler)())
{
	//cli();
	
	g_timer_three_interrupt_handler = timer_three_interrupt_handler;

	TCCR3A = 0x80;
	TCCR3B = 0x1B;
	TCCR3C = 0x00;

	ICR3 = 0x7A12;

	TIMSK3 = 0x02;

	//sei();
}

ISR (TIMER1_COMPA_vect)
{
	if ( g_timer_one_interrupt_handler != NULL )
		(*g_timer_one_interrupt_handler)();
}

ISR (TIMER2_COMPA_vect)
{
	if ( g_timer_two_interrupt_handlerA != NULL )
		(*g_timer_two_interrupt_handlerA)();
}

ISR (TIMER2_COMPB_vect)
{
	if ( g_timer_two_interrupt_handlerB != NULL )
		(*g_timer_two_interrupt_handlerB)();
}

ISR (TIMER3_COMPA_vect)
{
	if ( g_timer_three_interrupt_handler != NULL )
		(*g_timer_three_interrupt_handler)();
}